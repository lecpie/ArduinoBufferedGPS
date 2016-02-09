#include "BufferedGPS.h"

#include <Arduino.h>

#define GPSBUFSIZE 16

#define SELECTTYPE "$GPGGA"

#define    TYPETOKEN           0
#define    LATITUDETOKEN       2
#define    LATITUDESIGNTOKEN   3
#define    LONGITUDETOKEN      4
#define    LONGITUDESIGNTOKEN  5


MinimalGPS::MinimalGPS(int rx, int tx, long baud, long refreshrate)
    : _ss(rx,tx),
      _refreshrate(refreshrate),
      _lastrefresh(0),
      _last_latitude(0.0),
      _last_longitude(0.0)
{
    _ss.begin(baud);
}

float MinimalGPS::getLatitude()
{
    refresh();
    return _last_latitude;
}

float MinimalGPS::getLongitude()
{
    refresh();
    return _last_longitude;
}

void MinimalGPS::refresh()
{
    if (millis() - _lastrefresh < _refreshrate) return;

    uint8_t itok = 0,
            goodsentence = 0,
            quit = 0;
    char c;

    while (_ss.available() > 0 && !quit)
    {
      if (_igpsbuf >= GPSBUFSIZE - 1) {
        // Full buffer
        // Assume it's not normal to have an entry size that long and just reset the buffer
        _igpsbuf = 0;
        itok = 0;

        continue;
      }

      switch ((c = _ss.read())) {

        // New token char
        case '\n':
        case '\t':
        case ',':
          if (itok == TYPETOKEN) {
              _gpsbuf[_igpsbuf++] = '\0';
              goodsentence = !strcmp(SELECTTYPE, _gpsbuf);
          }

          // IF we are reading the NMEA sentence we wish to read
          if (goodsentence)
          {
              // Check interesting tokens
              switch (itok) {
              case LONGITUDETOKEN:

                  // Prepare to parse it with standard functions
                  _gpsbuf[_igpsbuf++] = '\0';

                  // First two characters left of the decimal are minutes, other degrees
                  // -> divide by 100 to get standard format
                  _last_longitude = strtod(_gpsbuf, NULL) / 100.0;

                  break;

              case LATITUDETOKEN:

                  // Same thing here

                  _gpsbuf[_igpsbuf++] = '\0';
                  _last_latitude = strtod(_gpsbuf, NULL) / 100.0;

                  break;

              case LATITUDESIGNTOKEN:
                  if (_igpsbuf && _gpsbuf[0] == 'S') {
                      _last_latitude = -_last_latitude;
                  }

                  break;
              case LONGITUDESIGNTOKEN:
                  if (_igpsbuf && _gpsbuf[0] == 'W') {
                      _last_longitude = -_last_longitude;
                      quit = 1;
                  }

                  break;
              }

          }

          // Get ready for next token
          ++itok;
          _igpsbuf = 0;

          break;

      default:
          // put char part of token in a buffer
          _gpsbuf[_igpsbuf++] = c;
      }

      if (c == '\n') {
          // New line, new NMEA sentence, new tokens
          itok = 0;
      }

    }

    _lastrefresh = millis();
}

#undef    TYPETOKEN
#undef    LATITUDETOKEN
#undef    LATITUDESIGNTOKEN
#undef    LONGITUDETOKEN
#undef    LONGITUDESIGNTOKEN
#undef    SELECTTYPE
#undef    GPSBUFSIZE
