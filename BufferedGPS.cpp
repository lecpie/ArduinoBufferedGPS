#include "BufferedGPS.h"

#include <Arduino.h>

#define GPSBUFSIZE 16

#define SELECTTYPE "$GPGGA"

#define    TYPETOKEN           0
#define    LATITUDETOKEN       2
#define    LATITUDESIGNTOKEN   3
#define    LONGITUDETOKEN      4
#define    LONGITUDESIGNTOKEN  5

#define DEFINEPATH BufferedGPS


DEFINEPATH::BufferedGPS(int rx, int tx, long refreshrate)
    : _ss(rx,tx),
      _refreshrate(refreshrate),
      _lastrefresh          (0),
      _last_latitude      (0.0),
      _last_longitude     (0.0) {}

void DEFINEPATH::begin(long baud)
{
    _ss.begin(baud);

    while (not _ss.available())
    {
        if (millis() > 5000)
            return;
    }

    forcerefresh();
}

float DEFINEPATH::getLatitude()
{
    refresh();
    return _last_latitude;
}

float DEFINEPATH::getLongitude()
{
    refresh();
    return _last_longitude;
}

static float parseNMEA(const char * term)
{
    float ret = 0.0;
    uint32_t leftOfDecimal = (uint32_t)atol(term);
    uint32_t multiplier = 10000000UL;
    uint32_t tenMillionthsOfMinutes = (leftOfDecimal % 100) * multiplier;

    ret = leftOfDecimal / 100;

    //uint16_t deg = (int16_t)(leftOfDecimal / 100);

    while (isdigit(*term))
      ++term;

    if (*term == '.')
      while (isdigit(*++term))
      {
        multiplier /= 10;
        tenMillionthsOfMinutes += (*term - '0') * multiplier;
      }

    return ret + ((5 * tenMillionthsOfMinutes + 1) / 3) / 1000000000.0;
}

void DEFINEPATH::forcerefresh()
{
    uint8_t itok         = 0,
            goodsentence = 0,
            quit         = 0;
    char c;

    while (not quit)
    {
        while (_ss.available() > 0)
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
                  goodsentence = not strcmp(SELECTTYPE, _gpsbuf);
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
                      _last_longitude = parseNMEA(_gpsbuf);

                      break;

                  case LATITUDETOKEN:

                      // Same thing here

                      _gpsbuf[_igpsbuf++] = '\0';
                      _last_latitude = parseNMEA(_gpsbuf);

                      break;

                  case LATITUDESIGNTOKEN:
                      if (_igpsbuf && _gpsbuf[0] == 'S') {
                          _last_latitude = -_last_latitude;
                      }

                      break;
                  case LONGITUDESIGNTOKEN:
                      if (_igpsbuf && _gpsbuf[0] == 'W') {
                          _last_longitude = -_last_longitude;
                      }
                      quit = 1;

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
    }

    _lastrefresh = millis();
}

void DEFINEPATH::refresh()
{
    if (millis() - _lastrefresh < _refreshrate) return;

    forcerefresh();
}

#undef DEFINEPATH
#undef TYPETOKEN
#undef LATITUDETOKEN
#undef LATITUDESIGNTOKEN
#undef LONGITUDETOKEN
#undef LONGITUDESIGNTOKEN
#undef SELECTTYPE
#undef GPSBUFSIZE
