#include <pebble.h>
#include "storage.h"

#include "comms.h"
#include "menu.h"
#include "settings.h"
#include "workout_layout.h"
#include "support.h"

enum {
	NUM_OF_WORKOUTS = 0,	
	WORKOUT_KEY_LIST = 1
};


uint32_t wIndex2 = 0;
uint32_t numWorkouts2 = 0;
uint32_t wKey2 = 100; // Starting Workout Key
uint32_t exerKey2 = 1; // Starting Exercise Key
uint32_t exerCounter2 = 0; // Exercise Counter

char *wName2 = NULL;
char eKeyList2[100] = {'\0'};
char wKey2List[100] = {'\0'};
char fos2[100] = {'\0'};

int add_workout(char *workoutName) {
	// Received Workout Name

	int lastKey = 0;
	char *result;

	strncpy(fos2, wKey2List, 100);

	if (fos2[0] != '\0') {
		result = strtok(fos2,","); 
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "Result: %s", result);
		lastKey = atoi(result);
		while (result != NULL) {
			result = strtok(NULL, ","); 
			if (result != NULL) {
				lastKey = atoi(result);
				//APP_LOG(APP_LOG_LEVEL_DEBUG, "PLUS %s", result);
				if (!persist_exists(lastKey)) { 
					//APP_LOG(APP_LOG_LEVEL_DEBUG, "Key %s does not exist!", result);
					wKey2 = lastKey;
					break; 
				} 
			} else {
				wKey2 = (lastKey + 100);
			}
		}
	} else {
		wKey2 = 100;
	}

	APP_LOG(APP_LOG_LEVEL_DEBUG, "wKey2List: %s", wKey2List);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Next wKey2: %d", (int)wKey2);

	// Write workout name to persistent storage
	persist_write_string((uint32_t)(wKey2), workoutName);
	return 0;
}

int set_num_exercises(int wKey2, int numExer) {
	persist_write_int((uint32_t)(wKey2 + 1), numExer);
	return 0;
}

int add_exercise(int wKey2, char *exerName) {
	persist_write_string((uint32_t)(wKey2 + (10 * exerKey2)), exerName);
	// Add exercise key to Exercise Key List
	if (eKeyList2[0] == '\0') {
		snprintf(eKeyList2, (sizeof eKeyList2) - strlen(eKeyList2), "%d",(int)(wKey2 + (10 * exerKey2)));
	} else {
		snprintf(eKeyList2 + strlen(eKeyList2), (sizeof eKeyList2) - strlen(eKeyList2), ",%d", (int)(wKey2 + (10 * exerKey2)));
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "E Key list: %s", eKeyList2);
	}	
	exerCounter2++;
	return 0;
}

int set_rest_time(int wKey2, int eKey, int rTime) {
	persist_write_int((uint32_t)(wKey2 + (10 * exerKey2) + exerCounter2), rTime);
	exerCounter2++;
	return 0;
}

int set_reps(int wKey2, int eKey, char *reps) {
	// Received Exercise Reps
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Reps: %s", reps);
	// Write exercise reps to persistent storage
	persist_write_string((uint32_t)(wKey2 + (10 * exerKey2) + exerCounter2), reps);
	exerCounter2++;
	return 0;
}

int set_loads(int wKey2, int eKey, char *loads) {
	// Received Exercise Loads
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Tuple Key: %lu Loads: %s", tuple->key, tuple->value->cstring);
	// Write exercise loads to persistent storage
	persist_write_string((uint32_t)(wKey2 + (10 * exerKey2) + exerCounter2), loads);

	// All information received for current exercise, update counters
	exerKey2 += 1; // Add to Exercise Key
	exerCounter2 = 0; // Reset exercise counter
	return 0;
}

int get_num_workouts() {
	// Get Number of Workouts
	if (persist_exists(NUM_OF_WORKOUTS)) {
		return persist_read_int(NUM_OF_WORKOUTS);
	} else {
		return 0;
	}
}

char *get_wKey2_list() {
		// Get Workout Key List
	if (persist_exists(WORKOUT_KEY_LIST)) {
		int bufsize = persist_get_size(WORKOUT_KEY_LIST);			
		persist_read_string(WORKOUT_KEY_LIST, wKey2List, bufsize );	
		return wKey2List;
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Workout Key List is %s", wKey2List);
	} else {
		return 0;
	}
}

