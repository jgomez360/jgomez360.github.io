#include "exercise_edit_layout.h"
#include <pebble.h>

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY

static Window *exercise_edit_s_main_window;
static Layer *exercise_edit_s_layer;

static GFont s_res_bitham_42_medium_numbers;
static GFont s_res_gothic_24_bold;

static int workoutKey = 0;
static int exerciseKey = 0;
static int setNumber = 0;
static char *value_text = "0";
static char *desc_text = "Reps";

int reps1[20] = {0};
int loads1[20] = {0};

int workout_key1 = 100;
char *workout_name;

int exer_key1 = 1;
int exer_counter1 = 0;
int numOfExer1 = 0;
int numOfSets1 = 0;
int setCounter1 = 0;

typedef struct _exerData { 
	char *exer_name;
	int rest_time;
	char *reps1;
	char *loads1;
} exerData;

exerData e;

static void exercise_edit_update_layer_callback(Layer *layer, GContext *ctx) {
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
  graphics_draw_text(ctx, value_text, s_res_gothic_24_bold, GRect(0, 46, 144, 26), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  
	// Draw black text
	graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx, desc_text, s_res_bitham_42_medium_numbers, GRect(7, 120, 130, 50), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  


}

static void exercise_edit_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  exercise_edit_s_layer = layer_create(bounds);
  layer_set_update_proc(exercise_edit_s_layer, exercise_edit_update_layer_callback);
  layer_add_child(window_layer, exercise_edit_s_layer);
}

static void exercise_edit_window_unload(Window *window) {
  layer_destroy(exercise_edit_s_layer);
}

void exercise_edit_initialise_ui(int wKey, int eKey, int setNum) {
  exercise_edit_s_main_window = window_create();
  window_set_window_handlers(exercise_edit_s_main_window, (WindowHandlers) {
    .load = exercise_edit_window_load,
    .unload = exercise_edit_window_unload,
  });
	
	workoutKey = wKey;
	exerciseKey = eKey;
	setNumber = setNum;
	
	
	if (persist_exists((uint32_t)(workoutKey + 1))) {
		numOfExer1 = persist_read_int((uint32_t)(workoutKey + 1));	

		//int needed = snprintf(NULL, 0, "Exercise 1 of %d", numOfExer1);
		//snprintf(exercise_count_text, (needed+1), "Exercise 1 of %d", numOfExer1);
		
		
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "Workout %i", workout_key1);

		if (persist_exists((uint32_t)(workout_key1 + (10 * exerciseKey) + exer_counter1))) {
			int bufsize = persist_get_size((uint32_t)(workout_key1 + (10 * exerciseKey) + exer_counter1));	

			e.exer_name = malloc( sizeof(char) * bufsize );
			//exercises[i]->exer_name = malloc( sizeof(char) * bufsize );
			persist_read_string((uint32_t)(workout_key1 + (10 * exer_key1) + exer_counter1), e.exer_name, bufsize );		
			//text_layer_set_text(exercise_name_textlayer, e.exer_name);
//			exercise_name_text = e.exer_name;
			exer_counter1++;

			APP_LOG(APP_LOG_LEVEL_DEBUG, "Exercise is %s", e.exer_name);

			//APP_LOG(APP_LOG_LEVEL_DEBUG, "12nd key %i", (int)(workout_key1 + (10 * exer_key1) + exer_counter1));
			e.rest_time = persist_read_int((uint32_t)(workout_key1 + (10 * exer_key1) + exer_counter1));		
	//		set_timer(e.rest_time);
	//		redisplay_timer();
			exer_counter1++;

			APP_LOG(APP_LOG_LEVEL_DEBUG, "Rest Time %i", e.rest_time);

			bufsize = persist_get_size((uint32_t)(workout_key1 + (10 * exer_key1) + exer_counter1));	
			e.reps1 = malloc( sizeof(char) * bufsize );
			persist_read_string((uint32_t)(workout_key1 + (10 * exer_key1) + exer_counter1), e.reps1, bufsize );		
			
			char *result;
			
			if (e.reps1[0] != '\0') {
				result = strtok(e.reps1,","); 
				numOfSets1 = 0;
				setCounter1 = 0;
				numOfSets1 += 1;
				APP_LOG(APP_LOG_LEVEL_DEBUG, "Rep1: %s", result);
				
				if (result[0] == '[') {
					result[0] = '0';
				}
				
				reps1[0] = atoi(result);
				
				//text_layer_set_text(reps1_textlayer, reps1_text);
				int needed = snprintf(NULL, 0, "Reps:%d", reps1[0]);
				snprintf(desc_text, (needed+1), "Reps:%d", reps1[0]);

				//lastKey = atoi(result);
				int i = 0;
				while (result != NULL) {
					result = strtok(NULL, ","); 
					if (result != NULL) {
						i += 1;
						reps1[i] = atoi(result);
						numOfSets1 += 1;
						APP_LOG(APP_LOG_LEVEL_DEBUG, "PLUS %s", result);
					}
				}

				//text_layer_set_text(sets_textlayer, sets_count_text);
				if (setNumber <= numOfSets1) {
					
					needed = snprintf(NULL, 0, "%d", reps1[setNumber]);
					snprintf(value_text, (needed+1), "%d", reps1[setNumber]);
					
				}
			}

			exer_counter1++;

		//	APP_LOG(APP_LOG_LEVEL_DEBUG, "Reps are %s", e.reps1);

			bufsize = persist_get_size((uint32_t)(workout_key1 + (10 * exer_key1) + exer_counter1));	
			e.loads1 = malloc( sizeof(char) * bufsize );
			persist_read_string((uint32_t)(workout_key1 + (10 * exer_key1) + exer_counter1), e.loads1, bufsize );		

			if (e.loads1[0] != '\0') {
				result = strtok(e.loads1,","); 
				APP_LOG(APP_LOG_LEVEL_DEBUG, "Load1: %s", result);

				if (result[0] == '[') {
					result[0] = '0';
				}
				
				loads1[0] = atoi(result);
				
				//text_layer_set_text(load_textlayer, loads1_text);
				//int needed = snprintf(NULL, 0, "Load:%d", loads1[0]);
				//snprintf(value_text, (needed+1), "Load:%d", loads1[0]);
				
				int i = 0;
				while (result != NULL) {
					result = strtok(NULL, ","); 
					if (result != NULL) {
						i += 1;
						loads1[i] = atoi(result);
						APP_LOG(APP_LOG_LEVEL_DEBUG, "PLUS %s", result);
					}
				}
			}

			exer_counter1 = 0; // Reset Exercise Counter
			
			//layer_mark_dirty(exercise_edit_s_layer);
			//window_stack_push(s_window, true);
		}
	}
	
  window_stack_push(exercise_edit_s_main_window, true);
	
	/*s_window = window_create();
  #ifndef PBL_SDK_3
    window_set_fullscreen(s_window, true);
  #endif
  */
}


void exercise_edit_destroy_ui(void) {
  window_destroy(exercise_edit_s_main_window);
}
