#ifndef __MINIMALGPS_H__
#define __MINIMALGPS_H__

#include <SoftwareSerial.h>

// Debug NMEA stream to test without GPS
//#define MOCKSTREAM
#ifdef MOCKSTREAM
// A sample NMEA stream.
const char gpsStream[] =
  "$GPRMC,045103.000,A,3014.1984,N,09749.2872,W,0.67,161.46,030913,,,A*7C\r\n"
  "$GPGGA,045104.000,3014.1985,N,09749.2873,W,1,09,1.2,211.6,M,-22.5,M,,0000*62\r\n"
  "$GPRMC,045200.000,A,3014.3820,N,09748.9514,W,36.88,65.02,030913,,,A*77\r\n"
  "$GPGGA,045201.000,3014.3864,N,09748.9411,W,1,10,1.2,200.8,M,-22.5,M,,0000*6C\r\n"
  "$GPRMC,045251.000,A,3014.4275,N,09749.0626,W,0.51,217.94,030913,,,A*7D\r\n"
  "$GPGGA,045252.000,3014.4273,N,09749.0628,W,1,09,1.3,206.9,M,-22.5,M,,0000*6F\r\n";

typedef struct mockss {

    int istream;
    int streamlen;

    int available() {
        return 10;
    }

    mockss(int,int)
        : istream(0), streamlen(sizeof(gpsStream)) {}

    void begin(int baud) {}

    int read() {
        if (istream >= streamlen) {
            istream = 0;
        }

        return gpsStream[istream++];
    }

} mockss;

#define SoftwareSerial mockss

#endif

#define GPSBUFSIZE 16

class MinimalGPS {
private:
    SoftwareSerial _ss;
    uint32_t _refreshrate;
    uint32_t _lastrefresh;

    float _last_latitude;
    float _last_longitude;

    char    _gpsbuf[GPSBUFSIZE];
    uint8_t _igpsbuf;

    void refresh();

public:
    MinimalGPS (int rx, int tx, long baud, long refreshrate);

    float getLatitude();
    float getLongitude();
};

#undef GPSBUFSIZE

#endif // __MINIMALGPS_H__
