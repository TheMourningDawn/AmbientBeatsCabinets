#include "application.h"
#include "LEDAnimations.h"
#include "SpectrumEqualizerClient.h"

#define BORDER_LED_PIN    D1

#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

SpectrumEqualizerClient *spectrum;
LEDAnimations *animations;

bool poweredOn = true;

UDP udpMulticast;
int udpPort = 47555;
IPAddress udpIP(239,1,1,232);

int hueValue = 100;

bool shouldResetDevice = false;

void setup() {
    if(shouldResetDevice == true) {
      System.reset();
    }
    // Serial.begin(11520);
    setupCloudModeFunctions();
    connectToRemote();

    spectrum = new SpectrumEqualizerClient();
    animations = new LEDAnimations(spectrum);

    FastLED.addLeds<LED_TYPE, BORDER_LED_PIN, COLOR_ORDER>(animations->leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  }

void loop() {
    readColorFromRemote();
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

    Particle.function("reset-device", resetDevice);
    Particle.function("enter-safe-mode", enterSafeMode);
    Particle.function("power-on", powerOn);
    Particle.function("power-off", powerOff);

    Particle.subscribe("NEXT_MODE", handleNextMode);
    Particle.subscribe("PREVIOUS_MODE", handlePreviousMode);
}

int resetDevice(String arg) {
   System.reset();

   return 1;
}

int enterSafeMode(String arg) {
  System.enterSafeMode();

  return 1;
}

int powerOn(String arg) {
    poweredOn = true;

    return 1;
}

int powerOff(String arg) {
  poweredOn = false;

  animations->clearAllLeds();
  FastLED.show();

  return 1;
}

int nextMode(String mode) {
    int currentPattern = animations->nextPattern();
    return currentPattern;
}

void handleNextMode(const char *eventName, const char *data) {
    nextMode("notSureWhyIHaveToDoItLikeThis");
}

int previousMode(String mode) {
    int currentPattern = animations->previousPattern();
    return currentPattern;
}

void handlePreviousMode(const char *eventName, const char *data) {
    previousMode("seriouslyWhy?");
}
