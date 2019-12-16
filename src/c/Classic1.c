#include <pebble.h>


/*


Notes
-----

The "s_" is just a naming convention meaning the variable is a String. Nothing else.

If viewing in VS Code, ignore the pebble.h lint message - the compiler will resolve it automatically.


NEED TO UNDERSTAND: How to use/include a graphic layer, how to use/include a custom font


Watchface C Instructions
------------------------

snprintf() - Print text_buffer, with a size of "sizeof(text_buffer)", using this string/format, [with this numerical data]

strftime() - Look at https://gist.github.com/alokc83/5792799 for format specifiers

localtime(&now) - get the time and put it in object of type "time_t"

tick_timer_service_subscribe(SECOND_UNIT, handler_name)

battery_state_service_subscribe(handler_name);

connection_service_subscribe((ConnectionHandlers) {
  .pebble_app_connection_handler = handler_name
});


Event Handlers (Injected)
-------------------------

handle_second_tick(struct tm* tick_time, TimeUnits units_changed) - use tick_time hierarchy, TimeUnits (secs, mins)

handle_bluetooth(bool connected) - use the 'connected' value as needed



Text Layer Operations
---------------------

text_layer_set_text(which_layer, which_text_buffer)

text_layer_create(GRect(x1, y1, x2, y2)), return the TextLayer object

text_layer_set_text_color(which_layer, GColorName);

text_layer_set_background_color(which_layer, GColorName);

text_layer_set_font(which_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));

text_layer_set_text_alignment(which_layer, GTextAlignmentCenter);


Colors (GColorName)
-------------------

GColorBlack, GColorWhite, GColorClear


Alignments (GTextAlignmentValue)
--------------------------------

GTextAlignmentLeft, GTextAlignmentCenter, GTextAlignmentRight

GRect(x, y, width, height)


DO NOT TOUCH OR RENAME: init(), deinit(), or main()


*/


// Global objects

static Window *s_main_window;                   // Root object

static TextLayer *s_time_layer;                 // Layer objects - one per piece of data to show
static TextLayer *s_dayofweek_layer;                 // Layer objects - one per piece of data to show
static TextLayer *s_date_layer;                 // Layer objects - one per piece of data to show

//static TextLayer *s_battery_layer;
//static TextLayer *s_connection_layer;


// Battery level event handler - optional
/*
static void handle_battery(BatteryChargeState charge_state) {

  static char battery_text[] = "100% charged";

  if (charge_state.is_charging) {
    snprintf(battery_text, sizeof(battery_text), "charging");
  } else {
    snprintf(battery_text, sizeof(battery_text), "%d%% charged", charge_state.charge_percent);
  }
  text_layer_set_text(s_battery_layer, battery_text);

}
*/


//*************************************************************************
// Tick event handler - key
//*************************************************************************

static void handle_second_tick(struct tm* tick_time, TimeUnits units_changed) {

  // Needs to be static because it's used by the system later.
  static char s_time_text[] = "00:00:00";
  static char s_dayofweek_text[] = "--DAY/WEEK--";
  static char s_date_text[] = "00/00/00";

  strftime(s_time_text, sizeof(s_time_text), "%T", tick_time);
  strftime(s_dayofweek_text, sizeof(s_dayofweek_text), "%A", tick_time);
  //strftime(s_dayofweek_text, sizeof(s_time_text), "%s", tick_time);
  strftime(s_date_text, sizeof(s_date_text), "%D", tick_time);

  text_layer_set_text(s_time_layer, s_time_text);
  text_layer_set_text(s_dayofweek_layer, s_dayofweek_text);
  text_layer_set_text(s_date_layer, s_date_text);

}


// Bluetooth status handler - optional
/*
static void handle_bluetooth(bool connected) {

  text_layer_set_text(s_connection_layer, connected ? "online" : "offline");

}
*/


//*************************************************************************
// Main layout
//*************************************************************************

static void main_window_load(Window *window) {


  // Key objects

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);


  // Design any new layers here (time, date, other operations, etc.) 

  s_time_layer = text_layer_create(GRect(0, 28, bounds.size.w, 34));
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_LECO_32_BOLD_NUMBERS));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  s_dayofweek_layer = text_layer_create(GRect(0, 70, bounds.size.w, 34));
  text_layer_set_text_color(s_dayofweek_layer, GColorVividCerulean);
  text_layer_set_background_color(s_dayofweek_layer, GColorClear);
  text_layer_set_font(s_dayofweek_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text_alignment(s_dayofweek_layer, GTextAlignmentCenter);

  s_date_layer = text_layer_create(GRect(0, 95, bounds.size.w, 34));
  text_layer_set_text_color(s_date_layer, GColorVividCerulean);
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_LECO_32_BOLD_NUMBERS));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);


  /*
  s_connection_layer = text_layer_create(GRect(0, 90, bounds.size.w, 34));
  text_layer_set_text_color(s_connection_layer, GColorWhite);
  text_layer_set_background_color(s_connection_layer, GColorClear);
  text_layer_set_font(s_connection_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(s_connection_layer, GTextAlignmentCenter);
  handle_bluetooth(connection_service_peek_pebble_app_connection());

  s_battery_layer = text_layer_create(GRect(0, 120, bounds.size.w, 34));
  text_layer_set_text_color(s_battery_layer, GColorWhite);
  text_layer_set_background_color(s_battery_layer, GColorClear);
  text_layer_set_font(s_battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentCenter);
  text_layer_set_text(s_battery_layer, "100% charged");
  */

  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)

  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  handle_second_tick(current_time, SECOND_UNIT);                            // event handler


  //*************************************************************************
  // Call timer service - update every second (alter for minute, hour, etc.)
  //*************************************************************************

  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);


  // Call battery status service - call whenever
  /*
  battery_state_service_subscribe(handle_battery);
  */


  // Call Bluetooth connection service - call whenever
  /*
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = handle_bluetooth
  });
  */


  //*************************************************************************
  // Add the display layers here, one at a time
  //*************************************************************************

  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_dayofweek_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));

  //layer_add_child(window_layer, text_layer_get_layer(s_connection_layer));
  //layer_add_child(window_layer, text_layer_get_layer(s_battery_layer));


  // Check the battery here
  /*
  handle_battery(battery_state_service_peek());                         // event handler
  */

}


// Shutdown operations

static void main_window_unload(Window *window) {

  // Unsubscribe first

  tick_timer_service_unsubscribe();
  
  //battery_state_service_unsubscribe();
  //connection_service_unsubscribe();

  // Then dispense with it

  text_layer_destroy(s_time_layer);
  
  //text_layer_destroy(s_connection_layer);
  //text_layer_destroy(s_battery_layer);

}


// Basic operations - DO NOT ALTER ORDER

static void init() {

  s_main_window = window_create();
  
  window_set_background_color(s_main_window, GColorBlack);             // can change color only (Was:)
  
  window_set_window_handlers(s_main_window, (WindowHandlers) {            // defines event handlers for loading and unloading the window
    .load = main_window_load,
    .unload = main_window_unload,
  });
  
  window_stack_push(s_main_window, true);

}

static void deinit() {
  window_destroy(s_main_window);
}


// Main program - main loop

int main(void) {
  init();
  app_event_loop();
  deinit();
}