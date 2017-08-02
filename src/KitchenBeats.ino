#include "application.h"
#include "LEDAnimations.h"
#include "SpectrumEqualizerClient.h"

#define BORDER_LED_PIN    D1

#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

#define BRIGHTNESS 255

SpectrumEqualizerClient *spectrum;
LEDAnimations *animations;

UDP udpMulticast;
int udpPort = 47555;
IPAddress udpIP(239,1,1,232);

int hueValue = 100;

void setup() {
    /*Serial.begin(11520);*/
    setupCloudModeFunctions();
    // connectToRemote();

    spectrum = new SpectrumEqualizerClient();
    animations = new LEDAnimations(spectrum);

    FastLED.addLeds<LED_TYPE, BORDER_LED_PIN, COLOR_ORDER>(animations->borderLeds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  }

void loop() {
    // readColorFromRemote();
    animations->runCurrentAnimation();
    FastLED.show();
}

void connectToRemote() {
   udpMulticast.begin(udpPort);
   udpMulticast.joinMulticast(udpIP);
}

void readColorFromRemote() {
   if(udpMulticast.parsePacket() > 0) {
     hueValue = udpMulticast.read() << 8 | udpMulticast.read();
   }
   animations->currentHue = hueValue;
}

void setupCloudModeFunctions() {
    Particle.function("nextMode", nextMode);
    Particle.function("previousMode", previousMode);

    Particle.subscribe("NEXT_MODE", handleNextMode);
    Particle.subscribe("PREVIOUS_MODE", handlePreviousMode);
}

int nextMode(String mode) {
    int currentPattern = animations->nextPattern();
    char currentPatternString[5];
    sprintf(currentPatternString, "%i", currentPattern);
    Particle.publish("Current Pattern", currentPatternString);
    return 1;
}

void handleNextMode(const char *eventName, const char *data) {
    nextMode("notSureWhyIHaveToDoItLikeThis");
}

int previousMode(String mode) {
    int currentPattern = animations->previousPattern();
    char currentPatternString[5];
    sprintf(currentPatternString, "%i", currentPattern);
    Particle.publish("Current Pattern", currentPatternString);
    return 1;
}

void handlePreviousMode(const char *eventName, const char *data) {
    previousMode("seriouslyWhy?");
}
