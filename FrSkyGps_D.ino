// U-Blox M8N on pins 8&9
// FrSky RX on pin 3

#include "config.h"

#include <NMEAGPS.h>
#include <GPSfix.h>
#include <GPSport.h>
#include <FrSkyTelemetry.h>
#ifdef DEBUG
#include <Streamers.h>
#endif

static NMEAGPS gps;
static gps_fix fix;
static FrSkyTelemetry telemetry;

void fillTelemetry(gps_fix& fix, FrSkyTelemetry& telemetry)
{
  telemetry.setGpsData(
    fix.latitude(), fix.longitude(),
    fix.altitude(),
    fix.speed_kph() / 3.6,  // km/h -> m/sec
    fix.heading(),
    fix.dateTime.full_year(), fix.dateTime.month, fix.dateTime.date,
    fix.dateTime.hours, fix.dateTime.minutes, fix.dateTime.seconds);
  float t1 = 0.0, t2 = 0.0;
  if (fix.valid.status)
    t1 = (int(fix.status)*1000.0) + (fix.satellites*1.0);
#ifdef GPS_FIX_HDOP
  if (fix.valid.hdop)
    t2 = fix.hdop;
#endif
  telemetry.setTemsData(t1, t2);
  telemetry.send();
}

void setup()
{
  gpsPort.begin( 9600 );
#ifdef DEBUG
  DEBUG_PORT.begin(9600);
  while(!DEBUG_PORT)
    ;
  trace_header(DEBUG_PORT);
  DEBUG_PORT.flush();
#endif
  telemetry.begin(FrSkyTelemetry::SOFT_SERIAL_PIN_3);
}

void loop()
{
  while (gps.available( gpsPort )) {
    fix = gps.read();
    fillTelemetry(fix, telemetry);
#ifdef DEBUG
    trace_all(DEBUG_PORT, gps, fix);
#endif
  }
}
