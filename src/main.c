#include <pebble.h>

#include "menu.h"
#include "settings.h"
#include "workout_layout.h"
#include "support.h"
#include "comms.h"


static void init(void) {
	menu_init();
	settings_init();
	comms_init();
	//workout_layout_init();
	
	//menu_show(numWorkouts, wKeyList);
}

static void deinit(void) {	
	menu_deinit();
	settings_deinit();
	comms_deinit();
//	workout_deinit();
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}
