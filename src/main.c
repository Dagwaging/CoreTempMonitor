#include <pebble.h>
#include "main_window.h"
  
#define TEMP 0
#define ALERT 1
#define MIN 2
#define MAX 3
#define LOAD 4
#define MEMORY 5

void received_callback(DictionaryIterator *iter, void *context) {
  Tuple *tempValue = dict_find(iter, TEMP);
  Tuple *alertValue = dict_find(iter, ALERT);
  
  if(!tempValue) {
    reset_temp();
    reset_load();
    reset_memory();
    
    if(alertValue) {
      set_temp_text("NO IP SET");
    }
    else {
      set_temp_text("...");
    }
    
    return;
  }
  
  if(alertValue->value->int8) {
    vibes_short_pulse();
  }
  
  Tuple *minValue = dict_find(iter, MIN);
  Tuple *maxValue = dict_find(iter, MAX);
  
  set_range(minValue->value->int32, maxValue->value->int32);
  set_temp(tempValue->value->int32);
  
  Tuple *loadValue = dict_find(iter, LOAD);
  set_load(loadValue->value->int32);
  
  Tuple *memoryValue = dict_find(iter, MEMORY);
  set_memory(memoryValue->value->int32);
}

void handle_init(void) {
  show_main_window();
  reset_temp();
  reset_load();
  reset_memory();
  
  app_message_register_inbox_received(received_callback);
  AppMessageResult result = app_message_open(dict_calc_buffer_size(6, sizeof(int), sizeof(int), sizeof(int), sizeof(int), sizeof(int), sizeof(int)), 0);
}

void handle_deinit(void) {
  hide_main_window();
  app_message_deregister_callbacks();
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
