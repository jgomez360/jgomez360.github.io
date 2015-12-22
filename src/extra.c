// This is the menu item draw callback where you specify what each item should look like
/*static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {	
	// Determine which section we're going to draw in
	int i = 0;
	char *result;
	
	strncpy(fos1, wKeyList1, 100);

	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Menu wKeyList: %s", wKeyList1);
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Menu fos: %s", fos1);

	if (fos1[0] != '\0') {
		result = strtok(fos1,","); 
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "Menu Result1: %s", result);

		while (i != cell_index->row) {
			result = strtok(NULL,","); 
			i += 1;
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "Menu Result2: %s", result);
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "wKeyList: %s", wKeyList);
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "FOS: %s", fos);
		} 

//		while ((i < cell_index->row) && (result != NULL)) {
//			result = strtok(NULL, ","); 
//			i += 1;
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "wKeyList: %s", wKeyList);
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "FOS: %s", fos);
//			APP_LOG(APP_LOG_LEVEL_DEBUG, "Second Result: %s", result);
//		}
		
		if (result != NULL) {
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "Menu wKey: %s", result);
			wKey1 = atoi(result);
		
			if (persist_exists(wKey1)) { 
				int bufsize = persist_get_size(wKey1);	
				wName = malloc( sizeof(char) * bufsize );

				persist_read_string(wKey1, wName, bufsize);
				menu_cell_basic_draw(ctx, cell_layer, wName, NULL, NULL);
			}
		} else {
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "wtf null?!");
		}
	} else {
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "wKeyList well then...: %s", wKeyList1);
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "Well then...: %s", fos1);
		
	}

//	if (persist_exists((uint32_t)(100 * (cell_index->row + 1)))) {
//		int bufsize = persist_get_size((uint32_t)(100 * (cell_index->row + 1)));	
//		char wName[bufsize];
//		persist_read_string((uint32_t)(100 * (cell_index->row + 1)), wName, bufsize);
//		menu_cell_basic_draw(ctx, cell_layer, wName, NULL, NULL);	
//	} else {
//		APP_LOG(APP_LOG_LEVEL_DEBUG, "Wtf where is it?!");
//	}

	  
	  // Use the row to specify which item we'll draw
     //switch (cell_index->row) {
 //       case 0:
          // This is a basic menu item with a title and subtitle
 //         menu_cell_basic_draw(ctx, cell_layer, "Example Workout", "Placeholder", NULL);
 //         break;

 //       case 1:
          // This is a basic menu icon with a cycling icon
          //menu_cell_basic_draw(ctx, cell_layer, "Icon Item", "Select to cycle", menu_icons[current_icon]);
 //         break;
 //     }
   //   break;
  //}

}*/



// OLD WORKOUT.C
/*
#include <pebble.h>

#include "menu.h"
#include "settings.h"
#include "workout.h"
	
Window* window;

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
char timer_text[] = "XX:XX";

int workout_key = 100;
char *workout_name;

int exer_key = 1;
int exer_counter = 0;
int numOfExer = 0;

typedef struct _exerData { 
	char *exer_name;
	int rest_time;
	char *reps;
	char *loads;
} exerData;

//typedef struct _exerData *ExerData;

exerData e;
//exerData exers[1];

TextLayer *text_exer_name_layer;
TextLayer *text_excercise_layer;
TextLayer *text_set_layer;
TextLayer *text_reps_layer;
// Layers to display minutes and seconds, a label for minutes and seconds and a time's up message
TextLayer *text_timer_layer;
TextLayer *text_min_layer;				// Layer for displaying minutes
TextLayer *text_sec_layer;				// Layer for displaying seconds
//TextLayer *text_label_layer;			// Layer for displaying 'm' and 's' labels
//TextLayer *text_times_up_layer;			// Layer for displaying "Time's Up" message

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
		
	text_layer_set_text(text_timer_layer, timer_text);
	layer_mark_dirty(text_layer_get_layer(text_timer_layer));
	
}

static void up_single_click_handler(ClickRecognizerRef recognizer, void* context) {
	exer_key += 1;
	
	if (persist_exists((uint32_t)((100 * workout_key) + (10 * exer_key)))) {
		int bufsize = persist_get_size((uint32_t)((100 * workout_key) + (10 * exer_key)));	
		e.exer_name = malloc( sizeof(char) * bufsize );
		persist_read_string((uint32_t)((100 * workout_key) + (10 * exer_key)), e.exer_name, bufsize );	
		text_layer_set_text(text_exer_name_layer, e.exer_name);
		exer_counter++;
		
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "Exercise is %s", e.exer_name);
		
		e.rest_time = persist_read_int((uint32_t)((100 * workout_key) + (10 * exer_key) + exer_counter));		
		set_timer(e.rest_time);
		redisplay_timer();
		exer_counter++;
		
		bufsize = persist_get_size((uint32_t)((100 * workout_key) + (10 * exer_key) + exer_counter));	
		e.reps = malloc( sizeof(char) * bufsize );
		persist_read_string((uint32_t)((100 * workout_key) + (10 * exer_key) + exer_counter), e.reps, bufsize );	
		exer_counter++;
		//text_layer_set_text(text_reps_layer, e.reps);
		
		bufsize = persist_get_size((uint32_t)((100 * workout_key) + (10 * exer_key) + exer_counter));	
		e.loads = malloc( sizeof(char) * bufsize );
		persist_read_string((uint32_t)((100 * workout_key) + (10 * exer_key) + exer_counter), e.loads, bufsize );	
		exer_counter = 0; // Reset Exercise Counter
		
		//char delims[] = ",";
		//char *result = NULL;
		//result = strtok( e.reps, delims );
		//text_layer_set_text(text_reps_layer, result);
		
	//	char * pch; 

		//char* str = "JAN,FEB,MAR,APR,MAY,JUN,JUL,AUG,SEP,OCT,NOV,DEC";
		//pch = strtok (e.reps,","); 
		//while (pch != NULL) 
		//{	
		//	APP_LOG(APP_LOG_LEVEL_DEBUG, pch);

//			printf("\n%s", pch); 
		//	pch = strtok (NULL, "|"); 
    //}
		
		layer_mark_dirty(text_layer_get_layer(text_reps_layer));
		
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "Reps are %s", e.reps);
	} else {
		exer_key -= 1;
	}
}

static void back_single_click_handler(ClickRecognizerRef recognizer, void* context) {
	if (exer_key > 1) {
		exer_key -= 1;
		
		if (persist_exists((uint32_t)((100 * workout_key) + (10 * exer_key)))) {
			int bufsize = persist_get_size((uint32_t)((100 * workout_key) + (10 * exer_key)));	
			e.exer_name = malloc( sizeof(char) * bufsize );
			persist_read_string((uint32_t)((100 * workout_key) + (10 * exer_key)), e.exer_name, bufsize );	
			text_layer_set_text(text_exer_name_layer, e.exer_name);
			exer_counter++;

			e.rest_time = persist_read_int((uint32_t)((100 * workout_key) + (10 * exer_key) + exer_counter));		
			set_timer(e.rest_time);
			redisplay_timer();
			exer_counter++;

			bufsize = persist_get_size((uint32_t)((100 * workout_key) + (10 * exer_key) + exer_counter));	
			e.reps = malloc( sizeof(char) * bufsize );
			persist_read_string((uint32_t)((100 * workout_key) + (10 * exer_key) + exer_counter), e.reps, bufsize );	
			layer_mark_dirty(text_layer_get_layer(text_reps_layer));
			exer_counter++;

			bufsize = persist_get_size((uint32_t)((100 * workout_key) + (10 * exer_key) + exer_counter));	
			e.loads = malloc( sizeof(char) * bufsize );
			persist_read_string((uint32_t)((100 * workout_key) + (10 * exer_key) + exer_counter), e.loads, bufsize );	
			exer_counter = 0; // Reset Exercise Counter

			//APP_LOG(APP_LOG_LEVEL_DEBUG, "Reps are %s", e.reps);
		}
	}
}

static void down_single_click_handler(ClickRecognizerRef recognizer, void* context) {
	if (!timer_running) {											// Start the timer
		seconds = (curr_val.min * 60) + curr_val.sec;
		if (seconds != 0) {
			seconds = (curr_val.min * 60) + curr_val.sec;
		}
		timer_running = (seconds != 0);
	} else {														// Pause the timer
		timer_running = false;
	}
	
	//if (!timer_running) {									 // Reset the timer to the start value
	//	curr_val.sec = init_val.sec;
	//	curr_val.min = init_val.min;

	//	redisplay_timer();
	//} else {												 // Timer is running, no action
	//}
}

static void select_single_click_handler(ClickRecognizerRef recognizer, void* context) {
	if (!timer_running) {
		//text_layer_set_background_color(text_times_up_layer, GColorBlack); // Clear "Time's Up"
		//text_layer_set_text_color(text_min_layer, GColorBlack);
		//text_layer_set_background_color(text_min_layer, GColorWhite);
		curr_val.sec = init_val.sec;
		curr_val.min = init_val.min;
		redisplay_timer();
	}
}

static void click_config_provider(void* context) {
	const uint16_t repeat_interval_ms = 300;
	
	window_single_click_subscribe(BUTTON_ID_UP, up_single_click_handler);
	window_single_click_subscribe(BUTTON_ID_BACK, back_single_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, down_single_click_handler);	
	window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler);
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

static void exercise_layer_draw(Layer *layer, GContext *ctx) {
	GRect bounds = layer_get_bounds(layer);	
	GRect rect_frame = GRect(0, 0, bounds.size.w - bounds.size.h / 2, bounds.size.h - 4);
	GRect text_frame = GRect(10, -3, bounds.size.w, bounds.size.h);
	
	// Draw a black filled rectangle with sharp corners
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_rect(ctx, rect_frame, 0, GCornerNone);
	graphics_context_set_text_color(ctx, GColorWhite);

	char *buf = malloc( 15 * sizeof(char) + sizeof(exer_key));
	buf = "EXERCISE ";
	buf[9] = (char)( '0' + ( (exer_key) / 10 ) );
	buf[10] = (char)( '0' + ( (exer_key) % 10 ) );
	buf[11] = '/';
	buf[12] = (char)( '0' + ( numOfExer / 10 ) );
	buf[13] = (char)( '0' + ( numOfExer % 10 ) );
	buf[14] = '\0';
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "BUFFER %i, %s", sizeof(buf), buf);
		
	graphics_draw_text(ctx, buf, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), text_frame, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
}

static void set_layer_draw(Layer *layer, GContext *ctx) {
	GRect bounds = layer_get_bounds(layer);	
	GRect text_frame = GRect(10, -3, bounds.size.w, bounds.size.h);

	// Draw a black filled rectangle with sharp corners
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_rect(ctx, bounds, 0, GCornerNone);

	graphics_context_set_text_color(ctx, GColorWhite);
	graphics_draw_text(ctx, "SET", fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), text_frame, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
}

static void reps_layer_draw(Layer *layer, GContext *ctx) {
	GRect bounds = layer_get_bounds(layer);	
	GRect text_frame = GRect(10, -3, bounds.size.w, bounds.size.h);

	// Draw a black filled rectangle with sharp corners
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_rect(ctx, bounds, 0, GCornerNone);

	graphics_context_set_text_color(ctx, GColorWhite);
	graphics_draw_text(ctx, e.reps, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), text_frame, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
}

static void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	window_set_background_color(window, GColorWhite);
		
	GFont timer_font = fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS);
	GFont exer_name_font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);

	text_exer_name_layer = text_layer_create(GRect(0, 0, 144, 52));
	text_layer_set_text_color(text_exer_name_layer, GColorBlack);
	text_layer_set_background_color(text_exer_name_layer, GColorWhite);
	text_layer_set_font(text_exer_name_layer, exer_name_font);
	text_layer_set_text_alignment(text_exer_name_layer, GTextAlignmentCenter);
	layer_add_child((Layer *)window_layer, (Layer *)text_exer_name_layer);
	text_layer_set_overflow_mode(text_exer_name_layer, GTextOverflowModeFill); 	
	text_layer_set_text(text_exer_name_layer, e.exer_name);
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Exercise %s", e.exer_name);
		
	text_excercise_layer = text_layer_create(GRect(0, 53, 130, 22));
	layer_add_child((Layer *)window_layer, (Layer *)text_excercise_layer);
	layer_set_update_proc((Layer *)text_excercise_layer, exercise_layer_draw);
	
	text_set_layer = text_layer_create(GRect(0, 74, 130, 18));
	layer_add_child((Layer *)window_layer, (Layer *)text_set_layer);
	layer_set_update_proc((Layer *)text_set_layer, set_layer_draw);
	
	text_reps_layer = text_layer_create(GRect(0, 95, 130, 20));

	layer_add_child((Layer *)window_layer, (Layer *)text_reps_layer);
	layer_set_update_proc((Layer *)text_reps_layer, reps_layer_draw);
	
	text_timer_layer = text_layer_create(GRect(10, 114, 124, 36));
	text_layer_set_text_color(text_timer_layer, GColorBlack);
	text_layer_set_background_color(text_timer_layer, GColorWhite);
	text_layer_set_font(text_timer_layer, timer_font);
	text_layer_set_text_alignment(text_timer_layer, GTextAlignmentCenter);
	layer_add_child((Layer *)window_layer, (Layer *)text_timer_layer);
	
	redisplay_timer();

	window_set_click_config_provider(window, click_config_provider);
	
	// Subscribe to timer ticks every second
	tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);

}

static void window_unload(Window *window) {
	text_layer_destroy(text_exer_name_layer);
	text_layer_destroy(text_excercise_layer);
	text_layer_destroy(text_set_layer);
	text_layer_destroy(text_reps_layer);
	text_layer_destroy(text_timer_layer);
}

void workout_init(void) {
	window = window_create();
	  
	window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
}

void workout_deinit(void) {
  window_destroy(window);
}

void workout_show(int workout_key) {
	exer_key = 1;
	if (persist_exists((uint32_t)((100 * workout_key) + 1))) {
		numOfExer = persist_read_int((uint32_t)((100 * workout_key) + 1));	

		//APP_LOG(APP_LOG_LEVEL_DEBUG, "Workout %i", workout_key);

		if (persist_exists((uint32_t)((100 * workout_key) + (10 * exer_key) + exer_counter))) {
			int bufsize = persist_get_size((uint32_t)((100 * workout_key) + (10 * exer_key) + exer_counter));	

			e.exer_name = malloc( sizeof(char) * bufsize );
			//exercises[i]->exer_name = malloc( sizeof(char) * bufsize );
			persist_read_string((uint32_t)((100 * workout_key) + (10 * exer_key) + exer_counter), e.exer_name, bufsize );		
			exer_counter++;

			//APP_LOG(APP_LOG_LEVEL_DEBUG, "Exercise is %s", e.exer_name);
			
			e.rest_time = persist_read_int((uint32_t)((100 * workout_key) + (10 * exer_key) + exer_counter));		
			set_timer(e.rest_time);
			exer_counter++;

			//APP_LOG(APP_LOG_LEVEL_DEBUG, "Rest Time %i", e.rest_time);

			bufsize = persist_get_size((uint32_t)((100 * workout_key) + (10 * exer_key) + exer_counter));	
			e.reps = malloc( sizeof(char) * bufsize );
			persist_read_string((uint32_t)((100 * workout_key) + (10 * exer_key) + exer_counter), e.reps, bufsize );		
			exer_counter++;

			bufsize = persist_get_size((uint32_t)((100 * workout_key) + (10 * exer_key) + exer_counter));	
			e.loads = malloc( sizeof(char) * bufsize );
			persist_read_string((uint32_t)((100 * workout_key) + (10 * exer_key) + exer_counter), e.loads, bufsize );		
			
			//exer_key = 1; // Reset Exercise Key
			exer_counter = 0; // Reset Exercise Counter
			
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "Reps are %s", e.reps);
		}
		//}

		//char *workout_name = malloc( sizeof(char) * bufsize );
		//workout_name[bufsize];
		//persist_read_string((uint32_t)workout_key, workout_name, bufsize );

		//APP_LOG(APP_LOG_LEVEL_DEBUG, "Workout %i name: %s", workout_key, workout_name);
		//text_layer_set_text(text_exer_name_layer, workout_name);

	} else {
		//text_layer_set_text(text_exer_name_layer, exercises[0]->exer_name);
	}

	window_stack_push(window, true); // Animated
}*/