#ifndef SPECTRUMEQUALIZERCLIENT_CPP
#define SPECTRUMEQUALIZERCLIENT_CPP

#include "SpectrumEqualizerClient.h"
#include "application.h"

UDP multicastUDP;
int port;

SpectrumEqualizerClient::SpectrumEqualizerClient() {
  port = 32555;
  IPAddress remoteIP(239,1,1,234);

  multicastUDP.begin(port);
  multicastUDP.joinMulticast(remoteIP);
}

void SpectrumEqualizerClient::readAudioFrequencies() {
    multicastUDP.parsePacket();
    for(int i=0;i<7;i++) {
      int value = multicastUDP.read() << 8 | multicastUDP.read();
      frequenciesLeftChannel[i] = value;
    }
}

#endif
