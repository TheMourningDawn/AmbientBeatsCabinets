#ifndef SPECTRUMEQUALIZERCLIENT_H
#define SPECTRUMEQUALIZERCLIENT_H

#include "application.h"

class SpectrumEqualizerClient {
    public:
      UDP multicastUDP;
      int port;
      IPAddress remoteIP;

      int frequenciesLeftChannel[7];
      int frequenciesRightChannel[7];

      SpectrumEqualizerClient();
      void readAudioFrequencies();
};

#endif
