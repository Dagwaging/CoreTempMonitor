#include <pebble.h>
#include "main_window.h"

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GBitmap *s_res_background;
static GFont s_res_gothic_28_bold;
static GFont s_res_gothic_14;
static BitmapLayer *s_background;
static TextLayer *s_temp;
static TextLayer *s_label_cpu;
static TextLayer *s_label_ram;
static TextLayer *s_label_core;
static TextLayer *s_label_temp;
static Layer *s_ticks;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  #ifndef PBL_SDK_3
    window_set_fullscreen(s_window, 0);
  #endif
  
  s_res_background = gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND);
  s_res_gothic_28_bold = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  s_res_gothic_14 = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  // s_background
  s_background = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_background, s_res_background);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_background);
  
  // s_temp
  s_temp = text_layer_create(GRect(0, 86, 144, 28));
  text_layer_set_background_color(s_temp, GColorClear);
  text_layer_set_text(s_temp, "...");
  text_layer_set_text_alignment(s_temp, GTextAlignmentCenter);
  text_layer_set_font(s_temp, s_res_gothic_28_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_temp);
  
  // s_label_cpu
  s_label_cpu = text_layer_create(GRect(9, 152, 33, 16));
  text_layer_set_background_color(s_label_cpu, GColorClear);
  text_layer_set_text(s_label_cpu, "CPU");
  text_layer_set_text_alignment(s_label_cpu, GTextAlignmentCenter);
  text_layer_set_font(s_label_cpu, s_res_gothic_14);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_label_cpu);
  
  // s_label_ram
  s_label_ram = text_layer_create(GRect(102, 152, 33, 16));
  text_layer_set_background_color(s_label_ram, GColorClear);
  text_layer_set_text(s_label_ram, "RAM");
  text_layer_set_text_alignment(s_label_ram, GTextAlignmentCenter);
  text_layer_set_font(s_label_ram, s_res_gothic_14);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_label_ram);
  
  // s_label_core
  s_label_core = text_layer_create(GRect(0, 114, 144, 14));
  text_layer_set_background_color(s_label_core, GColorClear);
  text_layer_set_text(s_label_core, "CORE");
  text_layer_set_text_alignment(s_label_core, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_label_core);
  
  // s_label_temp
  s_label_temp = text_layer_create(GRect(0, 125, 144, 14));
  text_layer_set_background_color(s_label_temp, GColorClear);
  text_layer_set_text(s_label_temp, "TEMP");
  text_layer_set_text_alignment(s_label_temp, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_label_temp);
  
  // s_ticks
  s_ticks = layer_create(GRect(0, 0, 144, 144));
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_ticks);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  bitmap_layer_destroy(s_background);
  text_layer_destroy(s_temp);
  text_layer_destroy(s_label_cpu);
  text_layer_destroy(s_label_ram);
  text_layer_destroy(s_label_core);
  text_layer_destroy(s_label_temp);
  layer_destroy(s_ticks);
  gbitmap_destroy(s_res_background);
}
// END AUTO-GENERATED UI CODE

typedef struct {
  RotBitmapLayer *layer;
  int16_t angle;
} Needle;

static int16_t get_angle(void *subject) {
  return ((Needle*)subject)->angle;
}

static void set_angle(void *subject, int16_t value) {
  rot_bitmap_layer_set_angle(((Needle*)subject)->layer, value);
  ((Needle*)subject)->angle = value;
}

static const PropertyAnimationImplementation angle_animation = {
  .base = {
    .update = (AnimationUpdateImplementation) property_animation_update_int16,
  },
  .accessors = {
    .setter = { .int16 = (const Int16Setter) set_angle, },
    .getter = { .int16 = (const Int16Getter) get_angle, },
  },
};

static void animation_stopped_handler(Animation* animation, bool finished, void *context) {
  property_animation_destroy((PropertyAnimation*)animation);
}

static void animate_angle(PropertyAnimation* animation, Needle* needle, int16_t angle, int duration) {
  animation = property_animation_create(&angle_animation, needle, NULL, NULL);
  animation->values.from.int16 = needle->angle;
  animation->values.to.int16 = angle;
  animation_set_handlers((Animation*) animation, (AnimationHandlers) {
    .stopped = animation_stopped_handler
  }, NULL);
  animation_set_duration((Animation*) animation, duration);
  animation_schedule((Animation*) animation);
}

static char temp_text[8];
static GBitmap *needleBitmap, *needleSmallBitmap;
static Needle needle_temp, needle_cpu, needle_ram;
static int temp_min, temp_max;
static PropertyAnimation *animation_temp, *animation_cpu, *animation_ram;

static void initialise_needles(void) {
  needleBitmap = gbitmap_create_with_resource(RESOURCE_ID_NEEDLE_BLACK);
  needleSmallBitmap = gbitmap_create_with_resource(RESOURCE_ID_NEEDLE_SMALL_BLACK);
  
  needle_temp.layer = rot_bitmap_layer_create(needleBitmap);
  needle_cpu.layer = rot_bitmap_layer_create(needleSmallBitmap);
  needle_ram.layer = rot_bitmap_layer_create(needleSmallBitmap);
  
  needle_temp.angle = -(TRIG_MAX_ANGLE * 4 / 12);
  needle_cpu.angle = 0;
  needle_ram.angle = TRIG_MAX_ANGLE / 2;
  
  rot_bitmap_set_src_ic(needle_temp.layer, GPoint(3, 62));
  rot_bitmap_set_src_ic(needle_cpu.layer, GPoint(2, 2));
  rot_bitmap_set_src_ic(needle_ram.layer, GPoint(2, 2));
  
  GRect frame = layer_get_frame((Layer *)needle_temp.layer);
  frame.origin.x = 72 - frame.size.w / 2;
  frame.origin.y = 72 - frame.size.h / 2;
  layer_set_frame((Layer *)needle_temp.layer, frame);
  
  frame = layer_get_frame((Layer *)needle_cpu.layer);
  frame.origin.x = 5 - frame.size.w / 2;
  frame.origin.y = 161 - frame.size.h / 2;
  layer_set_frame((Layer *)needle_cpu.layer, frame);
  
  frame = layer_get_frame((Layer *)needle_ram.layer);
  frame.origin.x = 139 - frame.size.w / 2;
  frame.origin.y = 161 - frame.size.h / 2;
  layer_set_frame((Layer *)needle_ram.layer, frame);
  
  rot_bitmap_set_compositing_mode(needle_temp.layer, GCompOpClear);
  rot_bitmap_set_compositing_mode(needle_cpu.layer, GCompOpClear);
  rot_bitmap_set_compositing_mode(needle_ram.layer, GCompOpClear);
  
  layer_add_child(window_get_root_layer(s_window), (Layer *)needle_temp.layer);
  layer_add_child(window_get_root_layer(s_window), (Layer *)needle_cpu.layer);
  layer_add_child(window_get_root_layer(s_window), (Layer *)needle_ram.layer);
}

static void destroy_needles(void) {
  rot_bitmap_layer_destroy(needle_temp.layer);
  rot_bitmap_layer_destroy(needle_cpu.layer);
  rot_bitmap_layer_destroy(needle_ram.layer);
  
  gbitmap_destroy(needleBitmap);
  gbitmap_destroy(needleSmallBitmap);
}

static void draw_ticks(Layer *layer, GContext *ctx) {
  graphics_context_set_stroke_color(ctx, GColorBlack);
  
  for(int i = temp_min; i < temp_max; i += 10) {
    int angle = -(TRIG_MAX_ANGLE * 7 / 12) + (TRIG_MAX_ANGLE * 8 / 12) * (i - temp_min) / (temp_max - temp_min);
    int cos = cos_lookup(angle);
    int sin = sin_lookup(angle);
    
    GPoint p0 = GPoint(72 + cos * 64 / TRIG_MAX_RATIO, 72 + sin * 64 / TRIG_MAX_RATIO);
    GPoint p1 = GPoint(72 + cos * 52 / TRIG_MAX_RATIO, 72 + sin * 52 / TRIG_MAX_RATIO);
      
    graphics_draw_line(ctx, p0, p1);
  }
}

static void handle_window_unload(Window* window) {
  destroy_ui();
  destroy_needles();
}

void show_main_window(void) {
  initialise_ui();
  initialise_needles();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  temp_min = 0;
  temp_max = 0;
  layer_set_update_proc(s_ticks, draw_ticks);
  window_set_fullscreen(s_window, 1);
  window_stack_push(s_window, true);
}

void set_range(int min, int max) {
  temp_min = min;
  temp_max = max;
}

void set_temp_text(char *text) {
  text_layer_set_text(s_temp, text);
}

void reset_temp(void) {
  int16_t angle = -(TRIG_MAX_ANGLE * 4 / 12);
  animate_angle(animation_temp, &needle_temp, angle, 500);
}

void set_temp(int _temp) {
  snprintf(temp_text, 8, "%d° C", _temp);
  set_temp_text(temp_text);
  
  int16_t angle = -(TRIG_MAX_ANGLE * 4 / 12) + (TRIG_MAX_ANGLE * 8 / 12) * (_temp - temp_min > 0 ? _temp - temp_min : 0) / (temp_max - temp_min);
  animate_angle(animation_temp, &needle_temp, angle, 500);
}

void reset_load(void) {
  animate_angle(animation_cpu, &needle_cpu, 0, 500);
}

void set_load(int _load) {
  int16_t angle = -TRIG_MAX_ANGLE / 4 * _load / 100;
  animate_angle(animation_cpu, &needle_cpu, angle, 500);
}

void reset_memory(void) {
  animate_angle(animation_ram, &needle_ram, TRIG_MAX_ANGLE / 2, 500);
}

void set_memory(int _memory) {
  int16_t angle = -(TRIG_MAX_ANGLE / 2) + TRIG_MAX_ANGLE / 4 * _memory / 100;
  animate_angle(animation_ram, &needle_ram, angle, 500);
}

void hide_main_window(void) {
  window_stack_remove(s_window, true);
}
