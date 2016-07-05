#include "menu.h"
#include "settings.h"
#include "workout_layout.h"
#include "support.h"
#include <pebble.h>
#include "exercise_edit_layout.h"

const VibePattern timer_done_vibe = {
	.durations = (uint32_t []) {100, 500, 100, 500},
	.num_segments = 4
};

// Timer defaults to 1 minute at app startup
typedef struct {
	int min;
	int sec;
} CounterData;

CounterData init_val = { 1, 0 };		// Remember counter starting point
CounterData curr_val = { 1, 0 };		// Counter current value

bool timer_running = false;				// Is the timer currently running?
int seconds = 0;						// Number of seconds in countdown

int reps[20] = {0};
int loads[20] = {0};

int workout_key = 100;
char *workout_name;

int exer_key = 1;
int exer_counter = 0;
int numOfExer = 0;
int numOfSets = 0;
int setCounter = 0;

typedef struct _exerData { 
	char *exer_name;
	int rest_time;
	char *reps;
	char *loads;
} exerData;

exerData e;

static Window *s_main_window;
static Layer *s_layer;

static GFont s_res_bitham_42_medium_numbers;
static GFont s_res_gothic_24_bold;

static char *exercise_count_text = "Exercise 1 of 00";
static char *timer_text = "00:00";
static char *sets_text = "Set 1 of 0";
static char *load_text = "Load:235";
static char *reps_text = "Reps:12";
static char *exercise_name_text = "Workout Name and vegetables";

static void update_layer_callback(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_frame(layer);
	 
	s_res_gothic_24_bold = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
	s_res_bitham_42_medium_numbers = fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS);
	
	// Set the line color
	graphics_context_set_stroke_color(ctx, GColorBlack);
	// Set the fill color
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_context_set_antialiased(ctx, false);
	
	// Draw black backgrounds
	graphics_fill_rect(ctx, GRect(0, 50, 144, 26), 0, GCornersAll);
	graphics_fill_rect(ctx, GRect(0, 100, 144, 26), 0, GCornersAll);
	
	// Draw white text
  graphics_context_set_text_color(ctx, GColorWhite);
  graphics_draw_text(ctx, exercise_count_text, s_res_gothic_24_bold, GRect(0, 46, 144, 26), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  graphics_draw_text(ctx, load_text, s_res_gothic_24_bold, GRect(68, 96, 77, 26), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  graphics_draw_text(ctx, reps_text, s_res_gothic_24_bold, GRect(0, 96, 71, 26), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  
	// Draw black text
	graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx, timer_text, s_res_bitham_42_medium_numbers, GRect(7, 120, 130, 50), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  graphics_draw_text(ctx, sets_text, s_res_gothic_24_bold, GRect(0, 72, 144, 26), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  graphics_draw_text(ctx, exercise_name_text, s_res_gothic_24_bold, GRect(0, 0, 144, 50), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);



}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  s_layer = layer_create(bounds);
  layer_set_update_proc(s_layer, update_layer_callback);
  layer_add_child(window_layer, s_layer);
}

static void window_unload(Window *window) {
  layer_destroy(s_layer);
}

static void initialise_ui(void) {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_main_window, true);
	
	/*s_window = window_create();
  #ifndef PBL_SDK_3
    window_set_fullscreen(s_window, true);
  #endif
  */
}


static void destroy_ui(void) {
  window_destroy(s_main_window);
} 

// Convert integers where 0 <= val <= 59 to two digit text
// For now, change to '!!' if val is out-of-bounds. Will look strange on the Pebble.
void itoa ( int min, int sec, char *txt ) {

	if (sizeof(txt) < 4) {
		return;
	}

	if ( ( min >= 0 ) && ( min <= 59 ) ) {
		txt[0] = (char) ('0' + (min / 10));
		txt[1] = (char) ('0' + (min % 10));
	} else {
		txt[0] = '!';
		txt[1] = '!';
	}
	
	txt[2] = ':';
	
	if ( ( sec >= 0 ) && ( sec <= 59 ) ) {
		txt[3] = (char) ('0' + (sec / 10));
		txt[4] = (char) ('0' + (sec % 10));
	} else {
		txt[3] = '!';
		txt[4] = '!';
	}
}

void set_timer (int seconds) {
	init_val.min = (seconds / 60);
	init_val.sec = (seconds % 60);

	curr_val.min = (seconds / 60);
	curr_val.sec = (seconds % 60);

	layer_mark_dirty(s_layer);
}

// Redisplay the timer minutes and seconds because a time tick has occurred while
// the timer is running, or the minutes or seconds have been edited
void redisplay_timer () {	
	static int last_min = -1;
	static int last_sec = -1;

	if (curr_val.min != last_min) {
		last_min = curr_val.min;
	}
	
	if (curr_val.sec != last_sec) {
		last_sec = curr_val.sec;
	}
	
	itoa(curr_val.min, curr_val.sec, timer_text);
		
	layer_mark_dirty(s_layer);
	
}

static void up_single_click_handler(ClickRecognizerRef recognizer, void* context) {
	exer_key += 1;

	if (persist_exists((uint32_t)(workout_key + (10 * exer_key)))) {
		int bufsize = persist_get_size((uint32_t)(workout_key + (10 * exer_key)));	
		//realloc( e.exer_name, sizeof(char) * bufsize );
		e.exer_name = malloc( sizeof(char) * bufsize );
		persist_read_string((uint32_t)(workout_key + (10 * exer_key)), e.exer_name, bufsize );	
		//text_layer_set_text(exercise_name_textlayer, e.exer_name);
		exercise_name_text = e.exer_name;
		exer_counter++;

		int needed = snprintf(NULL, 0, "Exercise %d of %d", exer_key, numOfExer);
		snprintf(exercise_count_text, (needed+1), "Exercise %d of %d", exer_key, numOfExer);
			
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "Exercise is %s", e.exer_name);

		//	APP_LOG(APP_LOG_LEVEL_DEBUG, "22nd key %i", (int)(workout_key + (10 * exer_key) + exer_counter));

		e.rest_time = persist_read_int((uint32_t)(workout_key + (10 * exer_key) + exer_counter));		
		set_timer(e.rest_time);
		redisplay_timer();
		exer_counter++;

		
		bufsize = persist_get_size((uint32_t)(workout_key + (10 * exer_key) + exer_counter));	
		//realloc( e.reps, sizeof(char) * bufsize );
		e.reps = malloc( sizeof(char) * bufsize );
		persist_read_string((uint32_t)(workout_key + (10 * exer_key) + exer_counter), e.reps, bufsize );	

		char *result;

		if (e.reps[0] != '\0') {
			numOfSets = 0;
			result = strtok(e.reps,","); 
			setCounter = 0;
			numOfSets += 1;
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "Rep1: %s", result);

			if (result[0] == '[') {
				result[0] = '0';
			}
			
			reps[0] = atoi(result);

			needed = snprintf(NULL, 0, "Reps:%d", reps[0]);
			snprintf(reps_text, (needed+1), "Reps:%d", reps[0]);

			//lastKey = atoi(result);
			int i = 0;
			while (result != NULL) {
				result = strtok(NULL, ","); 
				if (result != NULL) {
					i += 1;
					reps[i] = atoi(result);
					numOfSets += 1;
					//APP_LOG(APP_LOG_LEVEL_DEBUG, "PLUS %s", result);
				}
			}

			//text_layer_set_text(sets_textlayer, sets_count_text);
			needed = snprintf(NULL, 0, "Set %d of %d", (setCounter + 1), numOfSets);
			snprintf(sets_text, (needed+1), "Set %d of %d", (setCounter + 1), numOfSets);
		}

		exer_counter++;

		//APP_LOG(APP_LOG_LEVEL_DEBUG, "Reps %s", e.reps);


		bufsize = persist_get_size((uint32_t)(workout_key + (10 * exer_key) + exer_counter));	
		//realloc( e.loads, sizeof(char) * bufsize );
		e.loads = malloc( sizeof(char) * bufsize );
		persist_read_string((uint32_t)(workout_key + (10 * exer_key) + exer_counter), e.loads, bufsize );	

		if (e.loads[0] != '\0') {
			result = strtok(e.loads,","); 
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "Load1: %s", result);

			if (result[0] == '[') {
				result[0] = '0';
			}
			
			loads[0] = atoi(result);

			//text_layer_set_text(load_textlayer, loads_text);
			needed = snprintf(NULL, 0, "Load:%d", loads[0]);
			snprintf(load_text, (needed+1), "Load:%d", loads[0]);

			int i = 0;
			while (result != NULL) {
				result = strtok(NULL, ","); 
				if (result != NULL) {
					i += 1;
					loads[i] = atoi(result);
					//APP_LOG(APP_LOG_LEVEL_DEBUG, "PLUS %s", result);

				}
			}

		}

		exer_counter = 0; // Reset Exercise Counter
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "Loads %s", e.loads);

	} else {
		exer_key -= 1;
	}
	
	layer_mark_dirty(s_layer);

}

static void back_single_click_handler(ClickRecognizerRef recognizer, void* context) {
	if (exer_key > 1) {
		exer_key -= 1;
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "Workout %i , Exer %i", workout_key, exer_key);
		
		if (persist_exists((uint32_t)(workout_key + (10 * exer_key)))) {
			int bufsize = persist_get_size((uint32_t)(workout_key + (10 * exer_key)));	
			//realloc( e.exer_name, sizeof(char) * bufsize );
			e.exer_name = malloc( sizeof(char) * bufsize );
			persist_read_string((uint32_t)(workout_key + (10 * exer_key)), e.exer_name, bufsize );	
			//text_layer_set_text(exercise_name_textlayer, e.exer_name);
			exercise_name_text = e.exer_name;
			exer_counter++;

			int needed = snprintf(NULL, 0, "Exercise %d of %d", exer_key, numOfExer);
			snprintf(exercise_count_text, (needed+1), "Exercise %d of %d", exer_key, numOfExer);
			//snprintf(exercise_count_text, (sizeof exercise_count_text), "Exercise %d of %d", exer_key, numOfExer);
			
			e.rest_time = persist_read_int((uint32_t)(workout_key + (10 * exer_key) + exer_counter));		
			set_timer(e.rest_time);
			redisplay_timer();
			exer_counter++;

			bufsize = persist_get_size((uint32_t)(workout_key + (10 * exer_key) + exer_counter));	
			//realloc( e.reps, sizeof(char) * bufsize );
			e.reps = malloc( sizeof(char) * bufsize );
			persist_read_string((uint32_t)(workout_key + (10 * exer_key) + exer_counter), e.reps, bufsize );	
			
			char *result;

			if (e.reps[0] != '\0') {
				numOfSets = 0;
				result = strtok(e.reps,","); 
				setCounter = 0;
				numOfSets += 1;
				//APP_LOG(APP_LOG_LEVEL_DEBUG, "Rep1: %s", result);

				if (result[0] == '[') {
					result[0] = '0';
				}
				
				reps[0] = atoi(result);

				needed = snprintf(NULL, 0, "Reps:%d", reps[0]);
				snprintf(reps_text, (needed+1), "Reps:%d", reps[0]);

				//lastKey = atoi(result);
				int i = 0;
				while (result != NULL) {
					result = strtok(NULL, ","); 
					if (result != NULL) {
						i += 1;
						reps[i] = atoi(result);
						numOfSets += 1;
						//APP_LOG(APP_LOG_LEVEL_DEBUG, "PLUS %s", result);
					}
				}

				//text_layer_set_text(sets_textlayer, sets_count_text);
				needed = snprintf(NULL, 0, "Set %d of %d", (setCounter + 1), numOfSets);
				snprintf(sets_text, (needed+1), "Set %d of %d", (setCounter + 1), numOfSets);
			}

			exer_counter++;

			bufsize = persist_get_size((uint32_t)(workout_key + (10 * exer_key) + exer_counter));	
			e.loads = malloc( sizeof(char) * bufsize );
			persist_read_string((uint32_t)(workout_key + (10 * exer_key) + exer_counter), e.loads, bufsize );	

			if (e.loads[0] != '\0') {
				result = strtok(e.loads,","); 
				//APP_LOG(APP_LOG_LEVEL_DEBUG, "Load1: %s", result);

				if (result[0] == '[') {
					result[0] = '0';
				}
				
				loads[0] = atoi(result);

				//text_layer_set_text(load_textlayer, loads_text);
				needed = snprintf(NULL, 0, "Load:%d", loads[0]);
				snprintf(load_text, (needed+1), "Load:%d", loads[0]);
				
				int i = 0;
				while (result != NULL) {
					result = strtok(NULL, ","); 
					if (result != NULL) {
						i += 1;
						loads[i] = atoi(result);
						//APP_LOG(APP_LOG_LEVEL_DEBUG, "PLUS %s", result);

					}
				}
			}
			exer_counter = 0; // Reset Exercise Counter

			//APP_LOG(APP_LOG_LEVEL_DEBUG, "Reps are %s", e.reps);
		}
	}
		
	layer_mark_dirty(s_layer);
		
}

static void down_single_click_handler(ClickRecognizerRef recognizer, void* context) {
	if (!timer_running) {											// Start the timer
		if ((curr_val.sec == init_val.sec) && (curr_val.min = init_val.min)) {
			setCounter += 1;

			if (setCounter >= numOfSets) {
				setCounter = 0;
			}

			int needed = snprintf(NULL, 0, "Set %d of %d", (setCounter + 1), numOfSets);
			snprintf(sets_text, (needed+1), "Set %d of %d", (setCounter + 1), numOfSets);
			
			needed = snprintf(NULL, 0, "Reps:%d", reps[setCounter]);
			snprintf(reps_text, (needed+1), "Reps:%d", reps[setCounter]);
			
			needed = snprintf(NULL, 0, "Load:%d", loads[setCounter]);
			snprintf(load_text, (needed+1), "Load:%d", loads[setCounter]);	

			layer_mark_dirty(s_layer);
		}

		seconds = (curr_val.min * 60) + curr_val.sec;
		if (seconds != 0) {
			seconds = (curr_val.min * 60) + curr_val.sec;
		}
		timer_running = (seconds != 0);
	} else {														// Pause the timer
		timer_running = false;
	}
		
	layer_mark_dirty(s_layer);
		
}

static void select_single_click_handler(ClickRecognizerRef recognizer, void* context) {
	setCounter += 1;
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Set %d of %d", setCounter, numOfSets);
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Reps %d Load %d", reps[setCounter], loads[setCounter]);
			
	if (setCounter >= numOfSets) {
		setCounter = 0;
	}
	
	int needed = snprintf(NULL, 0, "Set %d of %d", (setCounter + 1), numOfSets);
	snprintf(sets_text, (needed+1), "Set %d of %d", (setCounter + 1), numOfSets);
	
	needed = snprintf(NULL, 0, "Reps:%d", reps[setCounter]);
	snprintf(reps_text, (needed+1), "Reps:%d", reps[setCounter]);
	
	needed = snprintf(NULL, 0, "Load:%d", loads[setCounter]);
	snprintf(load_text, (needed+1), "Load:%d", loads[setCounter]);	

	layer_mark_dirty(s_layer);
}

// Used to stop and reset timer to initial value
static void down_long_click_handler(ClickRecognizerRef recognizer, void* context) {
	if (!timer_running) {
		curr_val.sec = init_val.sec;
		curr_val.min = init_val.min;
		redisplay_timer();
	} else {
		timer_running = false;

		curr_val.sec = init_val.sec;
		curr_val.min = init_val.min;
		redisplay_timer();
		
	}
}

static void select_long_click_handler(ClickRecognizerRef recognizer, void* context) {

	exercise_edit_initialise_ui(100,1,1);
	//settings_init();
	//settings_show(1);
}

static void click_config_provider(void* context) {
	const uint16_t repeat_interval_ms = 300;
	const uint16_t long_click_interval_ms = 500;
	
	window_single_click_subscribe(BUTTON_ID_UP, up_single_click_handler);
	window_single_click_subscribe(BUTTON_ID_BACK, back_single_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, down_single_click_handler);	
	window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler);
	
	window_long_click_subscribe(BUTTON_ID_DOWN, long_click_interval_ms, down_long_click_handler, NULL);
	window_long_click_subscribe(BUTTON_ID_SELECT, long_click_interval_ms, select_long_click_handler, NULL);
}

// Decrement the timer, return true if we hit zero
bool decrement_timer () {
	// Decrement the timer, if at zero then return true, otherwise return false
	// Don't do anything if we are sent a negative number, and set the seconds to zero to stop the timer
	if (seconds > 0 ) {
		seconds--;
		curr_val.min = seconds / 60;
		curr_val.sec = seconds % 60;
		redisplay_timer();
	} else {
		seconds = 0;
	}
	return (seconds == 0);
}

// Decrement the timer every second. When we hit zero we notify the user, both visually
// and with a vibration pattern
// TODO: Can this handler be disabled if the timer isn't running???
static void handle_second_tick(struct tm *t, TimeUnits units_changed) {
	// Get out of here quick if the timer isn't running
	if (timer_running) {
		if (decrement_timer()) {
			// Time is up, reset to initial time
			// Queue up the vibration notification
			curr_val.min = init_val.min;
			curr_val.sec = init_val.sec;
			redisplay_timer();

			timer_running = false;
			vibes_enqueue_custom_pattern(timer_done_vibe);			 
		}
	}
}

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_workout_layout(int w_key) {
  initialise_ui();

	window_set_click_config_provider(s_main_window, click_config_provider);
	
	// Subscribe to timer ticks every second
	tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
	
	workout_key = w_key;
	exer_key = 1;

	APP_LOG(APP_LOG_LEVEL_DEBUG, "Workout %i , Exer %i", workout_key, exer_key);

	if (persist_exists((uint32_t)(workout_key + 1))) {
		numOfExer = persist_read_int((uint32_t)(workout_key + 1));	

		int needed = snprintf(NULL, 0, "Exercise 1 of %d", numOfExer);
		snprintf(exercise_count_text, (needed+1), "Exercise 1 of %d", numOfExer);
		
		
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Workout %i", workout_key);

		if (persist_exists((uint32_t)(workout_key + (10 * exer_key) + exer_counter))) {
			int bufsize = persist_get_size((uint32_t)(workout_key + (10 * exer_key) + exer_counter));	

			e.exer_name = malloc( sizeof(char) * bufsize );
			//exercises[i]->exer_name = malloc( sizeof(char) * bufsize );
			persist_read_string((uint32_t)(workout_key + (10 * exer_key) + exer_counter), e.exer_name, bufsize );		
			//text_layer_set_text(exercise_name_textlayer, e.exer_name);
			exercise_name_text = e.exer_name;
			exer_counter++;

			//APP_LOG(APP_LOG_LEVEL_DEBUG, "Exercise is %s", e.exer_name);

			//APP_LOG(APP_LOG_LEVEL_DEBUG, "12nd key %i", (int)(workout_key + (10 * exer_key) + exer_counter));
			e.rest_time = persist_read_int((uint32_t)(workout_key + (10 * exer_key) + exer_counter));		
			set_timer(e.rest_time);
			redisplay_timer();
			exer_counter++;

			APP_LOG(APP_LOG_LEVEL_DEBUG, "Rest Time %i", e.rest_time);

			bufsize = persist_get_size((uint32_t)(workout_key + (10 * exer_key) + exer_counter));	
			e.reps = malloc( sizeof(char) * bufsize );
			persist_read_string((uint32_t)(workout_key + (10 * exer_key) + exer_counter), e.reps, bufsize );		
			
			char *result;
			
			if (e.reps[0] != '\0') {
				result = strtok(e.reps,","); 
				numOfSets = 0;
				setCounter = 0;
				numOfSets += 1;
				//APP_LOG(APP_LOG_LEVEL_DEBUG, "Rep1: %s", result);
				
				if (result[0] == '[') {
					result[0] = '0';
				}
				
				reps[0] = atoi(result);
				
				//text_layer_set_text(reps_textlayer, reps_text);
				needed = snprintf(NULL, 0, "Reps:%d", reps[0]);
				snprintf(reps_text, (needed+1), "Reps:%d", reps[0]);

				//lastKey = atoi(result);
				int i = 0;
				while (result != NULL) {
					result = strtok(NULL, ","); 
					if (result != NULL) {
						i += 1;
						reps[i] = atoi(result);
						numOfSets += 1;
						//APP_LOG(APP_LOG_LEVEL_DEBUG, "PLUS %s", result);
					}
				}

				//text_layer_set_text(sets_textlayer, sets_count_text);
				needed = snprintf(NULL, 0, "Set %d of %d", (setCounter + 1), numOfSets);
				snprintf(sets_text, (needed+1), "Set %d of %d", (setCounter + 1), numOfSets);
			}

			exer_counter++;

			APP_LOG(APP_LOG_LEVEL_DEBUG, "Reps are %s", e.reps);

			bufsize = persist_get_size((uint32_t)(workout_key + (10 * exer_key) + exer_counter));	
			e.loads = malloc( sizeof(char) * bufsize );
			persist_read_string((uint32_t)(workout_key + (10 * exer_key) + exer_counter), e.loads, bufsize );		

			if (e.loads[0] != '\0') {
				result = strtok(e.loads,","); 
				APP_LOG(APP_LOG_LEVEL_DEBUG, "Load1: %s", result);

				if (result[0] == '[') {
					result[0] = '0';
				}
				
				loads[0] = atoi(result);
				
				//text_layer_set_text(load_textlayer, loads_text);
				needed = snprintf(NULL, 0, "Load:%d", loads[0]);
				snprintf(load_text, (needed+1), "Load:%d", loads[0]);
				
				int i = 0;
				while (result != NULL) {
					result = strtok(NULL, ","); 
					if (result != NULL) {
						i += 1;
						loads[i] = atoi(result);
						APP_LOG(APP_LOG_LEVEL_DEBUG, "PLUS %s", result);
					}
				}

			}

			exer_counter = 0; // Reset Exercise Counter
			
			layer_mark_dirty(s_layer);
			//window_stack_push(s_window, true);
		}
	}
	
}
 
void hide_workout_layout(void) {
  window_stack_remove(s_main_window, true);
}
