#include "gps.h"
#include "console.h"
#include <pthread.h>
#include <libgpsmm.h>

// GLOBAL VARS
extern bool verbose;
extern bool gps_debug;
extern bool console_disp;				// print smartbeaconing params to console
extern int console_iface;				// console serial port fd

// LOCAL VARS
GpsStruct gps;
int gps_iface;						// gps serial port fd

void* gps_thread(void*) {
	gpsmm gps_rec("localhost", DEFAULT_GPSD_PORT);

    if (gps_rec.stream(WATCH_ENABLE | WATCH_JSON) == NULL) {
        fprintf(stderr, "GPSD is not running.\n");
        return 0;
    }

	if (verbose) printf("Waiting for GPS fix.\n");

    for (;;) {
		struct gps_data_t* newdata;

		if (!gps_rec.waiting(5000000)) {
			gps.valid = false;
			if (gps_debug) printf("GPS_DEBUG: GPSd timeout.\n");
			if (console_disp) console_print("\x1B[4;6H\x1B[KGPSd timeout.");
			continue;
		}
		
		if ((newdata = gps_rec.read()) == NULL) {	// can't get gps info
			gps.valid = false;
			fprintf(stderr, "GPS read error.\n");
		} else if (newdata->fix.mode > 1) {		// good fix
			gps.valid = true;
			if (newdata->set & LATLON_SET) {
				gps.lat = newdata->fix.latitude;
				gps.lon = newdata->fix.longitude;
			}
			if (newdata->set & SPEED_SET) gps.speed = newdata->fix.speed * 2.23694;	// gpsd reports in m/s, covert to mph.
			if (newdata->set & TRACK_SET) gps.hdg = newdata->fix.track;
			if (newdata->set & ALTITUDE_SET) gps.alt = newdata->fix.altitude;
			
			if (gps_debug) printf("GPS_DEBUG: Lat:%f Lon:%f Alt:%i MPH:%.2f Hdg:%i Mode:%iD\n", gps.lat, gps.lon, gps.alt, gps.speed, gps.hdg, newdata->fix.mode);
			if (console_disp) dprintf(console_iface, "\x1B[4;6H\x1B[KLat:%f Lon:%f Alt:%i MPH:%.2f Hdg:%i Mode:%iD\n", gps.lat, gps.lon, gps.alt, gps.speed, gps.hdg, newdata->fix.mode);
		} else {
			gps.valid = false;	// no fix
			if (gps_debug) printf("GPS_DEBUG: No fix.\n");
			if (console_disp) console_print("\x1B[4;6H\x1B[KNo Fix.");
		}
		
    }
}