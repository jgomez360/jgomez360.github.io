#include "pebble.h"
#include "menu.h"
#include "workout_layout.h"
#include "settings.h"
#include "support.h"
	
#define TITLE_SIZE 96 + 1
#define NUM_MENU_SECTIONS 1

enum {
	NUM_OF_WORKOUTS = 0,	
	WORKOUT_KEY_LIST = 1
};

int32_t NUM_FIRST_MENU_ITEMS = 0;
uint32_t wKey1 = 100; // Starting Workout Key
uint32_t exerKey1 = 0; // Starting Exercise Key
uint32_t numWorkouts1 = 0;

char fos1[100] = {'\0'};
char wKeyList1[100] = {'\0'};
char workoutNames[20][TITLE_SIZE];

static Window *window;

// This is a menu layer
// You have more control than with a simple menu layer
static MenuLayer *menu_layer;

// A callback is used to specify the amount of sections of menu items
// With this, you can dynamically add and remove sections
static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}

// Each section has a number of items;  we use a callback to specify this
// You can also dynamically add and remove items using this
static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0:
      return NUM_FIRST_MENU_ITEMS;
    default:
      return 0;
  }
}

// A callback is used to specify the height of the section header
static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  // This is a define provided in pebble.h that you may use for the default height
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

// Here we draw what each header is
static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  // Determine which section we're working with
  switch (section_index) {
    case 0:
      // Draw title text in the section header
      menu_cell_basic_header_draw(ctx, cell_layer, "Select a workout");
      break;
  }
}

// This is the menu item draw callback where you specify what each item should look like
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {	
	// Only one section to draw in
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Row Draw: %d - %s", cell_index->row, workoutNames[cell_index->row]);
	menu_cell_basic_draw(ctx, cell_layer, workoutNames[cell_index->row], NULL, NULL);
}

// Here we capture when a user selects a menu item
static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  // Use the row to specify which item will receive the select action

	strncpy(fos1, wKeyList1, 100);

	APP_LOG(APP_LOG_LEVEL_DEBUG, "wKeyList: %s", fos1);

	int i = 0;
	char *result;
	if (fos1[0] != '\0') {
		result = strtok(fos1,","); 
		while ((result != NULL) && (i != cell_index->row)) {
			result = strtok(NULL,","); 
			i += 1;
			APP_LOG(APP_LOG_LEVEL_DEBUG, "Menu Result: %s", result);
		} 
	
		if (result != NULL) {
			show_workout_layout(atoi(result));
				//window_load();
			//free(result);
		}
	}
	
	//workout_show((int)(cell_index->row + 1));
}

static void menu_select_long_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
	settings_show((int)(cell_index->row + 1));
}

// Need to be more efficient
void addWorkout(char *workoutName, char *workoutKeyList) {
	strncpy(wKeyList1, workoutKeyList, 100);
	strncpy(workoutNames[NUM_FIRST_MENU_ITEMS], workoutName, TITLE_SIZE - 1);
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "menu wName: %s", workoutNames[NUM_FIRST_MENU_ITEMS]);
	NUM_FIRST_MENU_ITEMS += 1;
	
	menu_layer_reload_data(menu_layer);	
}

// This initializes the menu upon window load
void window_load(Window *window) {


}

void window_unload(Window *window) {
  // Destroy the menu layer
  menu_layer_destroy(menu_layer);
}

void menu_init(void) {
	// Initialize char array  
	for (int i = 0; i < 20; i++)
		memset(workoutNames[i], '\0', TITLE_SIZE);

	window = window_create();
  // Setup the window handlers
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
  });
	
	#ifdef PBL_SDK_3
		static StatusBarLayer *s_status_bar;
	#endif
		
	// Now we prepare to initialize the menu layer
  // We need the bounds to specify the menu layer's viewport size
  // In this case, it'll be the same as the window's
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // Create the menu layer
  menu_layer = menu_layer_create(bounds);

	// Add it to the window for display
	layer_add_child(window_layer, menu_layer_get_layer(menu_layer));

	// Set all the callbacks for the menu layer
	menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
		.get_num_sections = menu_get_num_sections_callback,
	  .get_num_rows = menu_get_num_rows_callback,
	  .get_header_height = menu_get_header_height_callback,
	  .draw_header = menu_draw_header_callback,
	  .draw_row = menu_draw_row_callback,
	  .select_click = menu_select_callback,
	  .select_long_click =  menu_select_long_click_callback,
	});

	#ifdef PBL_SDK_3
  	// Set up the status bar last to ensure it is on top of other Layers
  	s_status_bar = status_bar_layer_create();
  	layer_add_child(window_layer, status_bar_layer_get_layer(s_status_bar));
	#endif
	
	// Bind the menu layer's click config provider to the window for interactivity
	menu_layer_set_click_config_onto_window(menu_layer, window);


}

void menu_show(int numWorkouts, char* workoutKeyList) {
	numWorkouts1 = numWorkouts;
	NUM_FIRST_MENU_ITEMS = numWorkouts1;
	strncpy(wKeyList1, workoutKeyList, 100);

	window_stack_push(window, true /* Animated */);
}

void menu_deinit(void) {
	window_destroy(window);
}
