#ifndef LEDANIMATIONS_H
#define LEDANIMATIONS_H

#include "application.h"
#include "SpectrumEqualizerClient.h"
#include "FastLED.h"

FASTLED_USING_NAMESPACE;

#define NUM_LEDS 150

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

class LEDAnimations
{
  private:
    SpectrumEqualizerClient *equalizer;
  public:
    CRGB borderLeds[NUM_LEDS];
    uint8_t numberOfPatterns;
    int currentPattern;
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
    void rainbow();
    void confetti();
    void sinelon();
    void bpm();
    void juggle();

    void waterfall();
    void waterfallCascading();
    void waterfallBorder(int frequencyValue, int sensitivityValueMinThreshold, int brightness);

    void waterfallBorderRemoteAndSpectrum(int frequencyValue, int sensitivityThreshold);
    void waterfallBorderRemote();
    void waterfallBorderCascading(int frequencyValue, int sensitivityThreshold);
    void waterfallRainbowBorder();

    void equalizerBorderOnly();
    void equalizerBorderOnlyReversed();
    void equalizerLeftToRightBottomToTop();
    void equalizerRightToLeftBottomToTop();
    void equalizerRightToLeftTopToBottom();
    void equalizerLeftBorder(int frequencyValue, int sensitivityThreshold, bool direction);
    void equalizerRightBorder(int frequencyValue, int sensitivityThreshold, bool direction);
    void equalizerTopBorder(int frequencyValue, int sensitivityThreshold, bool direction);
};

#endif
