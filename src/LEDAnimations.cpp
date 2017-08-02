#ifndef LEDANIMATIONS_CPP
#define LEDANIMATIONS_CPP

#include "application.h"
#include "LEDAnimations.h"

CRGB borderLeds[NUM_LEDS];

SpectrumEqualizerClient *equalizer;

uint16_t globalSensitivity = 500;
uint8_t frequencyMode[7] = {0, 1, 2, 3, 4, 5, 6};
int currentHue = 0;

typedef void (LEDAnimations::*AnimationList)();

AnimationList animationList[] = {&LEDAnimations::waterfall, &LEDAnimations::waterfallBorderRemote,
            &LEDAnimations::waterfallRainbowBorder, &LEDAnimations::confetti,
            &LEDAnimations::juggle, &LEDAnimations::rainbow, &LEDAnimations::equalizerLeftToRightBottomToTop,
            &LEDAnimations::equalizerRightToLeftBottomToTop, &LEDAnimations::equalizerRightToLeftTopToBottom,
            &LEDAnimations::equalizerBorderOnly, &LEDAnimations::equalizerBorderOnlyReversed};

LEDAnimations::LEDAnimations() : equalizer(new SpectrumEqualizerClient()) {
    numberOfPatterns = ARRAY_SIZE(animationList) - 1;
    currentPattern = 0;
}

LEDAnimations::LEDAnimations(SpectrumEqualizerClient *eq) : equalizer(eq) {
    numberOfPatterns = ARRAY_SIZE(animationList) - 1;
    currentPattern = 0;
}

int LEDAnimations::runCurrentAnimation() {
    equalizer->readAudioFrequencies();
    (this->*animationList[currentPattern])();
}

int LEDAnimations::nextPattern() {
    currentPattern++;
    currentPattern = wrapToRange(currentPattern, 0, numberOfPatterns);
    clearAllLeds();
    return currentPattern;
}

int LEDAnimations::previousPattern() {
    currentPattern--;
    currentPattern = wrapToRange(currentPattern, 0, numberOfPatterns);
    clearAllLeds();
    return currentPattern;
}

void LEDAnimations::nextFrequencyMode() {
    int wrapEnd = frequencyMode[6];
    for(int i=6;i>0;i--) {
        frequencyMode[i] = frequencyMode[i-1];
    }
    frequencyMode[0] = wrapEnd;
}

void LEDAnimations::previousFrequencyMode() {
    int wrapBegining = frequencyMode[0];
    for(int i=0;i<6;i++) {
        frequencyMode[i] = frequencyMode[i+1];
    }
    frequencyMode[6] = wrapBegining;
}

int LEDAnimations::clampToRange(int numberToClamp, int lowerBound, int upperBound) {
    if (numberToClamp > upperBound) {
        return upperBound;
    } else if (numberToClamp < lowerBound) {
        return lowerBound;
    }
    return numberToClamp;
}

int LEDAnimations::clampSensitivity(int sensitivity) {
    return clampToRange(sensitivity, 0, 4096);
}

int LEDAnimations::wrapToRange(int numberToWrap, int lowerBound, int upperBound) {
    if (numberToWrap > upperBound) {
        return lowerBound;
    } else if (numberToWrap < lowerBound) {
        return upperBound;
    }
    return numberToWrap;
}

void LEDAnimations::clearAllLeds() {
  for(uint8_t j=0;j<NUM_LEDS;j++) {
      borderLeds[j] = 0;
  }
}

void LEDAnimations::rainbow() {
    // fill_rainbow(borderLeds, NUM_LEDS, currentHue, 7);
    fill_rainbow(borderLeds, NUM_LEDS, currentHue, 7);
}

// random colored speckles that blink in and fade smoothly
void LEDAnimations::confetti() {
    uint8_t position = random16(NUM_LEDS);
    uint16_t frequencyThreshold = clampSensitivity(globalSensitivity + 600);

    fadeToBlackBy(borderLeds, NUM_LEDS, 10);

    borderLeds[position] += CHSV(currentHue + random8(64), 200, 255);
}

// a colored dot sweeping back and forth, with fading trails
void LEDAnimations::sinelon() {
    uint16_t frequencyThreshold = clampSensitivity(globalSensitivity + 600);

    fadeToBlackBy(borderLeds, NUM_LEDS, 5);
    int pos = beatsin16(13, 0, NUM_LEDS);
    borderLeds[pos] += CHSV(currentHue, 255, 192);

}

// colored stripes pulsing at a defined Beats-Per-Minute (BPM)
void LEDAnimations::bpm() {
    uint8_t BeatsPerMinute = 120;
    CRGBPalette16 palette = PartyColors_p;
    uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
    for (int i = 0; i < NUM_LEDS; i++) { //9948
        borderLeds[i]= ColorFromPalette(palette, currentHue + (i * 2), beat - currentHue + (i * 10));
    }
}

// eight colored dots, weaving in and out of sync with each other
void LEDAnimations::juggle() {
    int frequencyValue = equalizer->frequenciesLeftChannel[frequencyMode[0]];
    uint16_t frequencyThreshold = clampSensitivity(globalSensitivity + 600);

    fadeToBlackBy(borderLeds, NUM_LEDS, 20);
    byte dothue = 0;
    if (frequencyValue > frequencyThreshold) {
        for (int i = 0; i < 8; i++) {
            int currentLocation = beatsin16(i + 7, 0, NUM_LEDS);
            borderLeds[currentLocation] |= CHSV(dothue, 200, 255);
            dothue += 32;
        }
    }
}

void LEDAnimations::waterfall() {
    int sensitivityValueMinThreshold = clampSensitivity(globalSensitivity + 700);
    int brightness = 255;
    waterfallBorder(equalizer->frequenciesLeftChannel[frequencyMode[4]], sensitivityValueMinThreshold, brightness);
}

void LEDAnimations::waterfallBorder(int frequencyValue, int frequencyValueMinThreshold, int brightness) {
    if (frequencyValue > frequencyValueMinThreshold) {
        int mappedFrequencyValue = map(frequencyValue, frequencyValueMinThreshold, 4096, 0, 255);
        mappedFrequencyValue = (mappedFrequencyValue + 120) % 255; //offsetting the base color...
        borderLeds[NUM_LEDS / 2] = CHSV(mappedFrequencyValue, brightness, 255);
    } else {
        borderLeds[NUM_LEDS / 2] = CRGB(0, 0, 0);
    }
    memmove(&borderLeds[0], &borderLeds[1], NUM_LEDS / 2 * sizeof(CRGB));
    memmove(&borderLeds[NUM_LEDS / 2 + 1], &borderLeds[NUM_LEDS / 2], NUM_LEDS / 2 * sizeof(CRGB));
}

void LEDAnimations::waterfallBorderRemoteAndSpectrum(int frequencyValue, int frequencyThreshold) {
    if (frequencyValue > frequencyThreshold) {
        int mappedFrequencyValue = currentHue;
        borderLeds[NUM_LEDS / 2] = CHSV(mappedFrequencyValue, 200, 255);
    } else {
        borderLeds[NUM_LEDS / 2] = CRGB(0, 0, 0);
    }
    memmove(&borderLeds[0], &borderLeds[1], NUM_LEDS / 2 * sizeof(CRGB));
    memmove(&borderLeds[NUM_LEDS / 2 + 1], &borderLeds[NUM_LEDS / 2], NUM_LEDS / 2 * sizeof(CRGB));
}

void LEDAnimations::waterfallBorderRemote() {
    borderLeds[NUM_LEDS / 2] = CHSV(currentHue, 200, 255);
    memmove(&borderLeds[0], &borderLeds[1], NUM_LEDS / 2 * sizeof(CRGB));
    memmove(&borderLeds[NUM_LEDS / 2 + 1], &borderLeds[NUM_LEDS / 2], NUM_LEDS / 2 * sizeof(CRGB));
}

uint8_t hueCounter = 0;
void LEDAnimations::waterfallRainbowBorder() {
    borderLeds[NUM_LEDS / 2] = CHSV(hueCounter, 200, 255);
    memmove(&borderLeds[0], &borderLeds[1], NUM_LEDS / 2 * sizeof(CRGB));
    memmove(&borderLeds[NUM_LEDS / 2 + 1], &borderLeds[NUM_LEDS / 2], NUM_LEDS / 2 * sizeof(CRGB));
    hueCounter++;
}

void LEDAnimations::equalizerLeftToRightBottomToTop() {
    fadeToBlackBy(borderLeds, NUM_LEDS, 40);
    equalizerLeftBorder(equalizer->frequenciesLeftChannel[frequencyMode[0]], clampSensitivity(globalSensitivity + 400), false);
    equalizerTopBorder(equalizer->frequenciesLeftChannel[frequencyMode[2]], clampSensitivity(globalSensitivity + 400), true);
    equalizerRightBorder(equalizer->frequenciesLeftChannel[frequencyMode[6]], clampSensitivity(globalSensitivity + 400), false);
}

void LEDAnimations::equalizerRightToLeftBottomToTop() {
    fadeToBlackBy(borderLeds, NUM_LEDS, 40);
    equalizerLeftBorder(equalizer->frequenciesLeftChannel[frequencyMode[0]], clampSensitivity(globalSensitivity + 400), false);
    equalizerTopBorder(equalizer->frequenciesLeftChannel[frequencyMode[2]], clampSensitivity(globalSensitivity + 400), false);
    equalizerRightBorder(equalizer->frequenciesLeftChannel[frequencyMode[6]], clampSensitivity(globalSensitivity + 400), false);
}

void LEDAnimations::equalizerRightToLeftTopToBottom() {
  fadeToBlackBy(borderLeds, NUM_LEDS, 40);
  equalizerLeftBorder(equalizer->frequenciesLeftChannel[frequencyMode[0]], clampSensitivity(globalSensitivity + 400), true);
  equalizerTopBorder(equalizer->frequenciesLeftChannel[frequencyMode[2]], clampSensitivity(globalSensitivity + 400), false);
  equalizerRightBorder(equalizer->frequenciesLeftChannel[frequencyMode[6]], clampSensitivity(globalSensitivity + 400), true);
}

void LEDAnimations::equalizerBorderOnly() {
  fadeToBlackBy(borderLeds, NUM_LEDS, 40);
  equalizerLeftBorder(equalizer->frequenciesLeftChannel[frequencyMode[0]], clampSensitivity(globalSensitivity + 400), false);
  equalizerRightBorder(equalizer->frequenciesLeftChannel[frequencyMode[6]], clampSensitivity(globalSensitivity + 400), false);
}

void LEDAnimations::equalizerBorderOnlyReversed() {
  fadeToBlackBy(borderLeds, NUM_LEDS, 40);
  equalizerLeftBorder(equalizer->frequenciesLeftChannel[frequencyMode[6]], clampSensitivity(globalSensitivity + 400), false);
  equalizerRightBorder(equalizer->frequenciesLeftChannel[frequencyMode[0]], clampSensitivity(globalSensitivity + 400), false);
}

void LEDAnimations::equalizerLeftBorder(int frequencyValue, int sensitivityThreshold, bool direction) {
    int ledsInSection = 62;
    if (frequencyValue > sensitivityThreshold) {
        int numberToLight = map(frequencyValue, 0, 3500, 0, ledsInSection);
        CRGB color = CHSV(map(frequencyValue, sensitivityThreshold, 4096, 0, 255), 200, 255);
        if (direction == true) {
            for(int i = ledsInSection - numberToLight;i<ledsInSection;i++) {
                borderLeds[i] = color;
            }
        } else {
            for(int i = 0;i<numberToLight;i++) {
                borderLeds[i] = color;
            }
        }
    }
}

void LEDAnimations::equalizerRightBorder(int frequencyValue, int sensitivityThreshold, bool direction) {
    int ledsInSection = 62;
    int locationOffset = 84;
    if (frequencyValue > sensitivityThreshold) {
        int numberToLight = map(frequencyValue, sensitivityThreshold, 3500, 0, ledsInSection);
        CRGB color = CHSV(map(frequencyValue, sensitivityThreshold, 4096, 0, 255), 200, 255);
        if (direction == true) {
            for(int i = locationOffset;i<locationOffset + numberToLight;i++) {
                borderLeds[i] = color;
            }
        } else {
            for(int i = locationOffset + ledsInSection - numberToLight;i<locationOffset + ledsInSection;i++) {
                borderLeds[i] = color;
            }
        }
    }
}

void LEDAnimations::equalizerTopBorder(int frequencyValue, int sensitivityThreshold, bool direction) {
    int ledsInSection = 20;
    int locationOffset = 63;
    for(int i = locationOffset;i<ledsInSection + locationOffset;i++) {
        borderLeds[i].fadeToBlackBy(45);
    }
    if (frequencyValue > sensitivityThreshold) {
        int numberToLight = map(frequencyValue, sensitivityThreshold, 3500, 0, ledsInSection);
        CRGB color = CHSV(map(frequencyValue, sensitivityThreshold, 4096, 0, 255), 200, 255);
        if (direction == true) {
            for(int i = locationOffset;i<locationOffset + numberToLight;i++) {
                borderLeds[i] = color;
            }
        } else {
            for(int i = locationOffset + ledsInSection - numberToLight;i<locationOffset + ledsInSection;i++) {
                borderLeds[i] = color;
            }
        }
    }
}

#endif
