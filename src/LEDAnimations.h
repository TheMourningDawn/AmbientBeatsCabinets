#ifndef LEDANIMATIONS_H
#define LEDANIMATIONS_H

#include "application.h"
#include "SpectrumEqualizerClient.h"
#include "FastLED.h"

FASTLED_USING_NAMESPACE;

#define NUM_LEDS 140
#define BRIGHTNESS 230

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

class LEDAnimations
{
  private:
    SpectrumEqualizerClient *equalizer;
  public:
    CRGB leds[NUM_LEDS];
    int currentHue;

    LEDAnimations();
    LEDAnimations(SpectrumEqualizerClient* eq);

    int runCurrentAnimation();
    int nextPattern();
    int previousPattern();
    void nextFrequencyMode();
    void previousFrequencyMode();

    int clampToRange(int numberToClamp, int lowerBound, int upperBound);
    int clampSensitivity(int sensitivity);
    int wrapToRange(int numberToWrap, int lowerBound, int upperBound);

    void randomSilon();

    void clearAllLeds();
    void fillSolid();
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

    bool getMusicReactive();
    void setMusicReactive(bool newMusicReactiveValue);

    int getNumberOfPatterns();
};

#endif
