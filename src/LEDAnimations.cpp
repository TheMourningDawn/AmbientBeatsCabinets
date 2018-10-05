#ifndef LEDANIMATIONS_CPP
#define LEDANIMATIONS_CPP

#include "application.h"
#include "LEDAnimations.h"

CRGB leds[NUM_LEDS];

SpectrumEqualizerClient *equalizer;

uint16_t globalSensitivity = 500;
uint8_t frequencyMode[7] = {0, 1, 2, 3, 4, 5, 6};
int currentPattern = 0;
int currentHue = 120;
bool musicReactive = true;

typedef void (LEDAnimations::*AnimationList)();
AnimationList animationList[] = {&LEDAnimations::waterfall, &LEDAnimations::sinelon, &LEDAnimations::waterfallRainbowBorder,
            &LEDAnimations::confetti, &LEDAnimations::juggle, &LEDAnimations::fillSolid, &LEDAnimations::rainbow,
            &LEDAnimations::rainbowSlide};

int numberOfPatterns = ARRAY_SIZE(animationList) - 1;

LEDAnimations::LEDAnimations() : equalizer(new SpectrumEqualizerClient()) { }

LEDAnimations::LEDAnimations(SpectrumEqualizerClient *eq) : equalizer(eq) { }

int LEDAnimations::getNumberOfPatterns() {
   return numberOfPatterns;
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
      leds[j] = 0;
  }
}

void LEDAnimations::fillSolid() {
  fill_solid(leds, NUM_LEDS, currentHue);
}

void LEDAnimations::rainbow() {
    fill_rainbow(leds, NUM_LEDS, currentHue);
}

void LEDAnimations::rainbowSlide() {
   fill_rainbow(leds, NUM_LEDS, currentHue);
   currentHue++;
}

// random colored speckles that blink in and fade smoothly
void LEDAnimations::confetti() {
    uint8_t position = random16(NUM_LEDS);
    int frequencyValue = equalizer->frequenciesLeftChannel[frequencyMode[0]];
    uint16_t frequencyThreshold = clampSensitivity(globalSensitivity + 600);

    fadeToBlackBy(leds, NUM_LEDS, 10);

    if(!musicReactive || frequencyValue > frequencyThreshold) {
      leds[position] += CHSV(currentHue + random8(64), 200, 255);
    }
}

// a colored dot sweeping back and forth, with fading trails
void LEDAnimations::sinelon() {
    int frequencyValue = equalizer->frequenciesLeftChannel[frequencyMode[0]];
    uint16_t frequencyThreshold = clampSensitivity(globalSensitivity + 600);

    fadeToBlackBy(leds, NUM_LEDS, 1);

    if(!musicReactive || (frequencyValue > frequencyThreshold)) {
      int pos = beatsin16(13, 0, NUM_LEDS);
      leds[pos] += CHSV(currentHue, 255, 192);
    }
}

// eight colored dots, weaving in and out of sync with each other
void LEDAnimations::juggle() {
    int frequencyValue = equalizer->frequenciesLeftChannel[frequencyMode[0]];
    uint16_t frequencyThreshold = clampSensitivity(globalSensitivity + 600);

    fadeToBlackBy(leds, NUM_LEDS, 20);
    byte dothue = 0;
    if(!musicReactive || frequencyValue > frequencyThreshold) {
        for (int i = 0; i < 8; i++) {
            int currentLocation = beatsin16(i + 7, 0, NUM_LEDS);
            leds[currentLocation] |= CHSV(dothue, 200, 255);
            dothue += 32;
        }
    }
}

void LEDAnimations::waterfall() {
    int sensitivityValueMinThreshold = clampSensitivity(globalSensitivity + 700);
    waterfallBorder(equalizer->frequenciesLeftChannel[frequencyMode[4]], sensitivityValueMinThreshold, BRIGHTNESS);
}

void LEDAnimations::waterfallBorder(int frequencyValue, int frequencyValueMinThreshold, int brightness) {
    if(!musicReactive || frequencyValue > frequencyValueMinThreshold) {
        int mappedFrequencyValue = map(frequencyValue, frequencyValueMinThreshold, 4096, 0, 255);
        mappedFrequencyValue = (mappedFrequencyValue + 120) % 255; //offsetting the base color...
        leds[NUM_LEDS / 2] = CHSV(mappedFrequencyValue, brightness, 255);
    } else {
        leds[NUM_LEDS / 2] = CRGB(0, 0, 0);
    }
    // When doing a memmove, don't move outside of the bounds of the array!!!
    memmove(&leds[0], &leds[1], (NUM_LEDS / 2) * sizeof(CRGB));
    memmove(&leds[NUM_LEDS / 2 + 1], &leds[NUM_LEDS / 2], (NUM_LEDS / 2) * sizeof(CRGB));
}

void LEDAnimations::waterfallBorderRemote() {
    leds[NUM_LEDS / 2] = CHSV(currentHue, 200, 255);
    memmove(&leds[0], &leds[1], (NUM_LEDS / 2) * sizeof(CRGB));
    memmove(&leds[NUM_LEDS / 2 + 1], &leds[NUM_LEDS / 2], (NUM_LEDS / 2) * sizeof(CRGB));
}

uint8_t hueCounter = 0;
void LEDAnimations::waterfallRainbowBorder() {
    leds[NUM_LEDS / 2] = CHSV(hueCounter, 200, 255);
    memmove(&leds[0], &leds[1], (NUM_LEDS / 2) * sizeof(CRGB));
    memmove(&leds[NUM_LEDS / 2 + 1], &leds[NUM_LEDS / 2], (NUM_LEDS / 2) * sizeof(CRGB));
    hueCounter++;
}

void LEDAnimations::equalizerBorderOnly() {
  fadeToBlackBy(leds, NUM_LEDS, 40);
  equalizerLeft(equalizer->frequenciesLeftChannel[frequencyMode[0]], clampSensitivity(globalSensitivity + 400), false);
  equalizerRight(equalizer->frequenciesLeftChannel[frequencyMode[6]], clampSensitivity(globalSensitivity + 400), false);
}

void LEDAnimations::equalizerLeft(int frequencyValue, int sensitivityThreshold, bool direction) {
    if (frequencyValue > sensitivityThreshold) {
        int numberToLight = map(frequencyValue, 0, 3500, 0, NUM_LEDS/2);
        CRGB color = CHSV(map(frequencyValue, sensitivityThreshold, 4096, 0, 255), 200, 255);
        if (direction == true) {
            for(int i = NUM_LEDS/2+1;i<NUM_LEDS-1;i++) {
                leds[i] = color;
            }
        } else {
            for(int i = NUM_LEDS-1;i<NUM_LEDS/2+1;i--) {
                leds[i] = color;
            }
        }
    }
}

void LEDAnimations::equalizerRight(int frequencyValue, int sensitivityThreshold, bool direction) {
    if (frequencyValue > sensitivityThreshold) {
        int numberToLight = map(frequencyValue, sensitivityThreshold, 3500, 0, NUM_LEDS/2);
        CRGB color = CHSV(map(frequencyValue, sensitivityThreshold, 4096, 0, 255), 200, 255);
        if (direction == true) {
            for(int i = NUM_LEDS/2-1;i<NUM_LEDS-1 - numberToLight;i--) {
                leds[i] = color;
            }
        } else {
            for(int i = 0;i<NUM_LEDS-1 + numberToLight;i++) {
                leds[i] = color;
            }
        }
    }
}

bool LEDAnimations::getMusicReactive() {
  return musicReactive;
}

void LEDAnimations::setMusicReactive(bool newMusicReactiveValue) {
  musicReactive = newMusicReactiveValue;
}


#endif
