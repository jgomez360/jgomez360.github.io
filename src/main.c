#include <pebble.h>

#include "menu.h"
#include "settings.h"
#include "workout_layout.h"
#include "support.h"
	
enum {
	NUM_OF_WORKOUTS = 0,	
	WORKOUT_KEY_LIST = 1
};

// ADD DELETE WORKOUT FUNCTION
uint32_t wIndex = 0;
uint32_t numWorkouts = 0;
uint32_t wKey = 100; // Starting Workout Key
uint32_t exerKey = 1; // Starting Exercise Key
uint32_t exerCounter = 0; // Exercise Counter

char *wName = NULL;
char eKeyList[100] = {'\0'};
char wKeyList[100] = {'\0'};
char fos[100] = {'\0'};

char *strtok1(char *s, const char *delim);
char *strtok3(char *s, const char *delim);

void out_sent_handler(DictionaryIterator *sent, void *context) {
	// outgoing message was delivered
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Message sent.");
}

void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
	 // outgoing message failed
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Failed to send message. Reason = %i", reason);
}

void in_received_handler(DictionaryIterator *iter, void *context) {
	// incoming message received
	Tuple *tuple = dict_read_first(iter);
  //APP_LOG(APP_LOG_LEVEL_ERROR, "Size of message: %li", (uint32_t)iter->end - (uint32_t)iter->dictionary);
		
	switch (tuple->key) {
	case 0:
		// Received Workout Name
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Tuple Key: %li Workout Name: %s", tuple->key, tuple->value->cstring);

		int lastKey = 0;
		char *result;
		
		wName = malloc(strlen(tuple->value->cstring) + 1);
		snprintf(wName, tuple->length, tuple->value->cstring);
	//	APP_LOG(APP_LOG_LEVEL_DEBUG, "Workout Name: %s", wName);

		strncpy(fos, wKeyList, 100);
		
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "wKeyList: %s", wKeyList);
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "FOS: %s", fos);
			
		if (fos[0] != '\0') {
			result = strtok(fos,","); 
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "Result: %s", result);
			lastKey = atoi(result);
			while (result != NULL) {
				result = strtok(NULL, ","); 
				if (result != NULL) {
					lastKey = atoi(result);
					//APP_LOG(APP_LOG_LEVEL_DEBUG, "PLUS %s", result);
					if (!persist_exists(lastKey)) { 
						//APP_LOG(APP_LOG_LEVEL_DEBUG, "Key %s does not exist!", result);
						wKey = lastKey;
						break; 
					} 
				} else {
					wKey = (lastKey + 100);
				}
			}
		} else {
			wKey = 100;
		}
		
		//if (strlen(result) != 0) {
		//	free(result);
		//}
		
		APP_LOG(APP_LOG_LEVEL_DEBUG, "wKeyList: %s", wKeyList);
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Next wKey: %d", (int)wKey);
		
		// Write workout name to persistent storage
		persist_write_string((uint32_t)(wKey), tuple->value->cstring);
		break;
	case 1: 
		// Received number of exercises in workout
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "Tuple Key: %lu Num of Exercises: %li", tuple->key, tuple->value->int32);
		// Write number of workouts to persistent storage
		persist_write_int((uint32_t)(wKey + 1), tuple->value->int32);
		break;	  	
	case 2:
	  // Received Exercise Name
	  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Tuple Key: %lu Exercise Name: %s", tuple->key, tuple->value->cstring);
		// Write exercise name to persistent storage
		persist_write_string((uint32_t)(wKey + (10 * exerKey)), tuple->value->cstring);

		// Add exercise key to Exercise Key List
		if (eKeyList[0] == '\0') {
			snprintf(eKeyList, (sizeof eKeyList) - strlen(eKeyList), "%d",(int)(wKey + (10 * exerKey)));
		} else {
			snprintf(eKeyList + strlen(eKeyList), (sizeof eKeyList) - strlen(eKeyList), ",%d", (int)(wKey + (10 * exerKey)));
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "E Key list: %s", eKeyList);
		}

		exerCounter++;
		break;
	case 3:
		// Received Exercise Rest Times
	  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Tuple Key: %lu Rest Time: %li", tuple->key, tuple->value->int32);
		// Write exercise rest times to persistent storage
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "Loading 2nd key %i", (int)(wKey + (10 * exerKey) + exerCounter));

		persist_write_int((uint32_t)(wKey + (10 * exerKey) + exerCounter), tuple->value->int32);
	  exerCounter++;
	  break;
	case 4:
		// Received Exercise Reps
	  APP_LOG(APP_LOG_LEVEL_DEBUG, "Tuple Key: %lu Reps: %s", tuple->key, tuple->value->cstring);
		// Write exercise reps to persistent storage
		persist_write_string((uint32_t)(wKey + (10 * exerKey) + exerCounter), tuple->value->cstring);
	  exerCounter++;
	  break;
	case 5:
		// Received Exercise Loads
	  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Tuple Key: %lu Loads: %s", tuple->key, tuple->value->cstring);
		// Write exercise loads to persistent storage
		persist_write_string((uint32_t)(wKey + (10 * exerKey) + exerCounter), tuple->value->cstring);

		// All information received for current exercise, update counters
		exerKey += 1; // Add to Exercise Key
		exerCounter = 0; // Reset exercise counter
		break;
	case 6:
	  // Done receiving all exercises, write data and clear buffer
	  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Tuple Key: %lu - %s", tuple->key, tuple->value->cstring);
	  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Done receiving workout: %s", workout->name);
		if (wKeyList[0] == '\0') {
			//wKeyList = itoa(100 * (numWorkouts + 1));
			//strcpy(wKeyList,  itoa(100 * (numWorkouts + 1)));	
			snprintf(wKeyList, (sizeof wKeyList) - strlen(wKeyList), "%d",(int)wKey);
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "wKeyList1: %s", wKeyList);
		} else {
			//wKeyList += itoa(100 * (numWorkouts + 1));
			//strcat(wKeyList, "," + (100 * (numWorkouts + 1)));
			snprintf(wKeyList + strlen(wKeyList), (sizeof wKeyList) - strlen(wKeyList), ",%d", (int)wKey);
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "wKeyList2: %s", wKeyList);
		}

		// Done receiving workout, update counters
		exerKey = 1; // Reset Exercise Key
		numWorkouts += 1; // Add to number of workouts

		// Write workout data to persistent storage
		persist_write_int(NUM_OF_WORKOUTS, numWorkouts);
		persist_write_string(WORKOUT_KEY_LIST, wKeyList);
		//persist_write_string(eKey, eKeyList);
	  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Exercise Key List: %s", eKeyList);

		// Add Workout Name to Menu List
		if (wName != NULL) {
			addWorkout(wName, wKeyList);
			free(wName);			
		}
		break;
	default:
  	APP_LOG(APP_LOG_LEVEL_WARNING, "Unknown key in dict: %lu", tuple->key);
  	break;
  }
}

void in_dropped_handler(AppMessageResult reason, void *context) {
	// Receiving workout failed, reset counters
	exerKey = 1; // Reset Exercise Key
	exerCounter = 0; // Reset exercise counter

	APP_LOG(APP_LOG_LEVEL_ERROR, "Dropped message! Reason given: %i", reason);
}

static void init(void) {
	menu_init();
	settings_init();
	//workout_layout_init();
	
	app_message_register_inbox_received(in_received_handler);
	app_message_register_inbox_dropped(in_dropped_handler);
	app_message_register_outbox_sent(out_sent_handler);
	app_message_register_outbox_failed(out_failed_handler);

	const uint32_t inbound_size = app_message_inbox_size_maximum();
	const uint32_t outbound_size = app_message_outbox_size_maximum();
	app_message_open(inbound_size, outbound_size);

	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Inbox Size: %lu", inbound_size);
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Outbox Size: %lu", outbound_size);	

	// Get Number of Workouts
	if (persist_exists(NUM_OF_WORKOUTS)) {
		numWorkouts = persist_read_int(NUM_OF_WORKOUTS);
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Number of Workouts: %li", numWorkouts);
	} else {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "No workout quantity found!");
		numWorkouts = 0;
	}
	
	// Get Workout Key List
	if (persist_exists(WORKOUT_KEY_LIST)) {
		int bufsize = persist_get_size(WORKOUT_KEY_LIST);			
		persist_read_string(WORKOUT_KEY_LIST, wKeyList, bufsize );	
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Workout Key List is %s", wKeyList);
	} else {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "No workout key list found!");
	}
	
	char *result;
	char *tempWorkoutName;
	int lastKey = 0, bufsize = 0;
	
	strncpy(fos, wKeyList, 100);
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "start wKeyList: %s", wKeyList);
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "start FOS: %s", fos);

	// Add each Workout Name to Menu List
	if (fos[0] != '\0') {
		result = strtok(fos,","); 
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "start Result: %s", result);

		lastKey = atoi(result);
		if (persist_exists(lastKey)) { 
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "start exists");
			bufsize = persist_get_size(lastKey);	
			tempWorkoutName = malloc( sizeof(char) * bufsize );
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "start twn: %s", tempWorkoutName);
			persist_read_string(lastKey, tempWorkoutName, bufsize);
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "start twn2: %s", tempWorkoutName);
			addWorkout(tempWorkoutName, wKeyList);
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "start wName: %s", tempWorkoutName);
			//free(tempWorkoutName);
		} 

		while (result != NULL) {
		//uint32_t i;
		//for ( i = 1; i < numWorkouts; i++ ) {
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "start plus1: %s", result);
			result = strtok(NULL, ","); 
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "start plus2: %s", result);
			//i++;
			if (result != NULL) {
				lastKey = atoi(result);
				if (persist_exists(lastKey)) { 
					bufsize = persist_get_size(lastKey);	
					tempWorkoutName = malloc( sizeof(char) * bufsize );
					persist_read_string(lastKey, tempWorkoutName, bufsize);
					addWorkout(tempWorkoutName, wKeyList);
					//free(tempWorkoutName);
					//break; 
				} 
			}
		}
	}
	
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "showing menu");
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "start menu wKeyList: %s", wKeyList);

	
	menu_show(numWorkouts, wKeyList);
}

static void deinit(void) {
	persist_write_int(NUM_OF_WORKOUTS, numWorkouts);
	persist_write_string(WORKOUT_KEY_LIST, wKeyList);
	
	menu_deinit();
	settings_deinit();
//	workout_deinit();
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}

char *
strtok1(s, delim)
	register char *s;
	register const char *delim;
{
	register char *spanp;
	register int c, sc;
	char *tok;
	static char *last;


	if (s == NULL && (s = last) == NULL)
		return (NULL);

	/*
	 * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
	 */
cont:
	c = *s++;
	for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
		if (c == sc)
			goto cont;
	}

	if (c == 0) {		/* no non-delimiter characters */
		last = NULL;
		return (NULL);
	}
	tok = s - 1;

	/*
	 * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
	 * Note that delim must have one NUL; we stop if we see that, too.
	 */
	for (;;) {
		c = *s++;
		spanp = (char *)delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;
				last = s;
				return (tok);
			}
		} while (sc != 0);
	}
	/* NOTREACHED */
}

char *
strtok3(s, delim)
	register char *s;
	register const char *delim;
{
	register char *spanp;
	register int c, sc;
	char *tok;
	static char *last;


	if (s == NULL && (s = last) == NULL)
		return (NULL);

	/*
	 * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
	 */
cont:
	c = *s++;
	for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
		if (c == sc)
			goto cont;
	}

	if (c == 0) {		/* no non-delimiter characters */
		last = NULL;
		return (NULL);
	}
	tok = s - 1;

	/*
	 * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
	 * Note that delim must have one NUL; we stop if we see that, too.
	 */
	for (;;) {
		c = *s++;
		spanp = (char *)delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;
				last = s;
				return (tok);
			}
		} while (sc != 0);
	}
	/* NOTREACHED */
}
