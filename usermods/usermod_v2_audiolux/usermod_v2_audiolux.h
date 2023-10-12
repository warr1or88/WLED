#pragma once

#include "wled.h"

#include "pixl.h"
#include "Logging.h"

#define LOGLEVEL LOG_LEVEL_INFOS

using namespace pixl;

//========================================================================================================================

class AudioLux {

	int NUM_LEDS;
	LEDStrip ledstrip = LEDStrip(NUM_LEDS);
	// LEDs alleds;
	Input* input;
	Visualization* viz;
	TriangleAnimation* anim;

	public:

	AudioLux() {}

	void initEffect() {
		
		NUM_LEDS = SEGMENT.virtualWidth() * SEGMENT.virtualHeight();

		ledstrip = LEDStrip(NUM_LEDS);
		// alleds = LEDs(&ledstrip, 0, NUM_LEDS);

		input = new RandomInput();

		// Add all the components to the looper so they update every frame
		Looper* looper = Looper::instance();
		looper->clearAll();
		looper->addInput(input);
		looper->addVisualization(viz);
		looper->addAnimation(anim);
		// looper->setUpdatesPerSecond(30);
	}

	void setVizTwinkleVisualization() {
		viz = new TwinkleVisualization(input, NUM_LEDS);
	}

	void loop() {
		Looper::instance()->loop();
	}
};

//========================================================================================================================

AudioLux audioLux;

static const char _data_FX_mode_TwinkleVisualization[] PROGMEM = "AudioLux - Twinkle";
static const char _data_FX_mode_HueVisualization[] PROGMEM = "AudioLux - Hue";


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


class AudioLuxUsermod : public Usermod {

  public:

    AudioLuxUsermod(const char *name, bool enabled):Usermod(name, enabled) {} //WLEDMM

    void setup() {
		
		if(!enabled) return;

      	strip.addEffect(255, &mode_TwinkleVisualization, _data_FX_mode_TwinkleVisualization);
      	strip.addEffect(255, &mode_HueVisualization, _data_FX_mode_HueVisualization);

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