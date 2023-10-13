#pragma once

#include "wled.h"

#include "pixl.h"
#include "Logging.h"

#define LOGLEVEL LOG_LEVEL_INFOS

using namespace pixl;

//========================================================================================================================

class WLEDAudioInput : public Input {
  public:
    WLEDAudioInput();
    void update() {
		if (!usermods.getUMData(&um_data, USERMOD_ID_AUDIOREACTIVE)) {
			// add support for no audio
			um_data = simulateSound(SEGMENT.soundSim);
		}
	}
    float getInput(int index = 0) {
		if (index == 0) {
			uint8_t samplePeak = *(uint8_t*)um_data->u_data[3];
			return 0.0; // TODO map 0-255 to expected range
		} else if (index == 1) {
			uint8_t *fftResult = (uint8_t*) um_data->u_data[2];
			return 0.0; // TODO map 0-255 to expected range
		} else {
			return 0.0;
		}
	}
  private:
    um_data_t *um_data;
};

class AudioLux {

	int NUM_LEDS;
	LEDStrip ledstrip = LEDStrip(NUM_LEDS);
	LEDs leds = LEDs(&ledstrip, 0, 0, false);
	Input* input;
	Visualization* viz;
	Animation* anim;
	Looper* looper = Looper::instance();

	public:

	AudioLux() {}

	void initEffect() {
		
		NUM_LEDS = SEGMENT.virtualWidth() * SEGMENT.virtualHeight();

		ledstrip = LEDStrip(NUM_LEDS);
		leds = LEDs(&ledstrip, 0, NUM_LEDS, false);

		input = new RandomInput();

		// Add all the components to the looper so they update every frame
		
		looper->clearAll();
		delete viz;
		delete anim;
		looper->addInput(input);
		// looper->setUpdatesPerSecond(30);
	}

	void setVizTwinkleVisualization() {
		initEffect();
		viz = new TwinkleVisualization(input, NUM_LEDS);
		looper->addVisualization(viz);
	}

	void setVizHueVisualization() {
		initEffect();
		viz = new HueVisualization(input, NUM_LEDS);
		anim = new PassThroughAnimation(viz, leds);
		looper->addVisualization(viz);
		looper->addAnimation(anim);
	}

	void setFireVisualization() {
		initEffect();
		viz = new FireVisualization(input, NUM_LEDS);
		looper->addVisualization(viz);
	}

	void loop() {
		Looper::instance()->loop();
		for(int i = 0; i < NUM_LEDS; i++) {
			SEGMENT.setPixelColor(i, ledstrip.leds[i]); // TODO: UGLY copy for now
		}
	}
};

//========================================================================================================================

AudioLux audioLux;

static const char _data_FX_mode_TwinkleVisualization[] PROGMEM = "AudioLux - Twinkle";
static const char _data_FX_mode_HueVisualization[] PROGMEM = "AudioLux - Hue";
static const char _data_FX_mode_FireVisualization[] PROGMEM = "AudioLux - Fire";


uint16_t mode_TwinkleVisualization() { 
	if (SEGENV.call == 0) {
		audioLux.setVizTwinkleVisualization();
	}
	audioLux.loop();
	return FRAMETIME;
}

uint16_t mode_HueVisualization() { 
	if (SEGENV.call == 0) {
		audioLux.setVizHueVisualization();
	}
	audioLux.loop();
	return FRAMETIME;
}

uint16_t mode_FireVisualization() { 
	if (SEGENV.call == 0) {
		audioLux.setFireVisualization();
	}
	audioLux.loop();
	return FRAMETIME;
}


class AudioLuxUsermod : public Usermod {

  public:

    AudioLuxUsermod(const char *name, bool enabled):Usermod(name, enabled) {} //WLEDMM

    void setup() {
		
		if(!enabled) return;

      	strip.addEffect(255, &mode_TwinkleVisualization, _data_FX_mode_TwinkleVisualization);
      	strip.addEffect(255, &mode_HueVisualization, _data_FX_mode_HueVisualization);
      	strip.addEffect(255, &mode_FireVisualization, _data_FX_mode_FireVisualization);

		initDone = true;
    }

    void loop() {
      if (!enabled || strip.isUpdating()) return;

      // do your magic here
      if (millis() - lastTime > 1000) {
        //USER_PRINTLN("I'm alive!");
        lastTime = millis();
      }

	  
    }

    uint16_t getId()
    {
      return USERMOD_ID_AUDIOLUX;
    }

};