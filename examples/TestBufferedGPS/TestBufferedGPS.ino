#include <SoftwareSerial.h>

#include "BufferedGPS.h"

// GPS SPECIFIC
#define RX         2
#define TX         1
#define GPSBAUD 4800

MinimalGPS gps(TX, RX, GPSBAUD, 500);

void setup()
{
    Serial.begin(115200);
    Serial.println("setup");
}

#define FLOATPRECISION 4
char strbuf[16];

void loop()
{
    Serial.print(gps.getLatitude(),    FLOATPRECISION);
    Serial.print(",");
    Serial.println(gps.getLongitude(), FLOATPRECISION);
}
