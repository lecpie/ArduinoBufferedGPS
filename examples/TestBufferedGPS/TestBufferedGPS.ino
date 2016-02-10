#include <SoftwareSerial.h>

#include "BufferedGPS.h"

// GPS SPECIFIC
#define RX         2
#define TX         1
#define GPSBAUD 4800

MinimalGPS gps(RX, TX, 50);

void setup()
{
    Serial.begin(115200);
    gps.begin(GPSBAUD);
    Serial.println("setup");
}

#define PRECISION 6
char strbuf[16];

void loop()
{
    Serial.print(gps.getLatitude(),    PRECISION);
    Serial.print(",");
    Serial.println(gps.getLongitude(), PRECISION);
}
