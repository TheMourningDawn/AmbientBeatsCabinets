#ifndef LEDANIMATIONS_H
#define LEDANIMATIONS_H

#include "application.h"
#include "SpectrumEqualizerClient.h"
#include "AmbientBeatsLEDAnimations.h"
#include "FastLED.h"

FASTLED_USING_NAMESPACE;

#define NUM_LEDS 139

class LEDAnimations : public AmbientBeatsLEDAnimations
{
public:
    LEDAnimations(SpectrumEqualizerClient* eq);

    CRGB leds[NUM_LEDS];

    int runAnimation();
    int toggleAudioReactive();

    void clearAllLeds();
    void fillColor();

    void randomSilon();
    void rainbow();
    void rainbowSlide();
    void confetti();
    void sinelon();
    void bpm();
    void juggle();

    void waterfall();
    void waterfallBorder(int frequencyValue, int sensitivityValueMinThreshold, int brightness);
    void waterfallBorderRemote();
    void waterfallRainbowBorder();

    void equalizerBorderOnly();
    void equalizerLeft(int frequencyValue, int sensitivityThreshold, bool direction);
    void equalizerRight(int frequencyValue, int sensitivityThreshold, bool direction);
};

#endif
