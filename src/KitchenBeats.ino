#include "application.h"
#include "LEDAnimations.h"
#include "SpectrumEqualizerClient.h"

#define BORDER_LED_PIN    D1

#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

SpectrumEqualizerClient *spectrum;
LEDAnimations *animations;

UDP udpMulticast;
int udpPort = 47555;
IPAddress udpIP(239,1,1,232);

bool poweredOn = true;

void setup() {
    // Serial.begin(11520);
    setupCloudModeFunctions();
    connectToRemote();

    spectrum = new SpectrumEqualizerClient();
    animations = new LEDAnimations(spectrum);

    FastLED.addLeds<LED_TYPE, BORDER_LED_PIN, COLOR_ORDER>(animations->leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  }

void loop() {
    if(poweredOn) {
      // readColorFromRemote();
      animations->runCurrentAnimation();
      FastLED.show();
    }
}

void connectToRemote() {
   udpMulticast.begin(udpPort);
   udpMulticast.joinMulticast(udpIP);
}

void readColorFromRemote() {
   if(udpMulticast.parsePacket() > 0) {
     animations->currentHue = udpMulticast.read() << 8 | udpMulticast.read();
   }
}

void setupCloudModeFunctions() {
    Particle.variable("currentHue", animations->currentHue);

    Particle.function("next-mode", nextMode);
    Particle.function("previous-mode", previousMode);
    Particle.function("reset-device", resetDevice);
    Particle.function("safe-mode", enterSafeMode);
    Particle.function("power-on", powerOn);
    Particle.function("power-off", powerOff);
    Particle.function("toggle-music", toggleMusicReactive);
    Particle.function("set-rgb", setColor);
    Particle.function("set-hue", setHue);


    Particle.subscribe("NEXT_MODE", handleNextMode);
    Particle.subscribe("PREVIOUS_MODE", handlePreviousMode);
    Particle.subscribe("RESET", handleReset);
}

int resetDevice(String arg) {
   System.reset();

   return 1;
}

void handleReset(const char *eventName, const char *data) {
  resetDevice("whatever");
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

int toggleMusicReactive(String arg) {
  animations->setMusicReactive(!animations->getMusicReactive());

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

int setHue(String hueString) {
  animations->currentHue = hueString.toInt();
}

//Expects rgb values to be in r,g,b format e.g. 140,200,90
int setColor(String rgbString) {
   char buffer[12];
   rgbString.toCharArray(buffer, 12);
   String r = "";
   String g = "";
   String b = "";

   int rgbItem = 0;
   for(int i=0;i<12;i++) {
     if(buffer[i] != ',') {
       if(rgbItem == 0) {
         r.concat(buffer[i]);
       } else if(rgbItem == 1) {
         g.concat(buffer[i]);
       } else if(rgbItem == 2) {
         b.concat(buffer[i]);
       }
     } else {
       rgbItem++;
     }
   }

   CRGB rgb = CRGB(r.toInt(),g.toInt(),b.toInt());
   CHSV hsv = rgb2hsv_approximate(rgb);

   animations->currentHue = hsv.hue;

   return hsv.hue;
}
