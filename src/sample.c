#include <pebble.h>
#include "sample.h"

#define KEY_TEMP 0
#define KEY_COND 1

static Window *s_window;
static TextLayer *weatfer_or_time_text,*month_text,*day_text,*week_text;
static Layer *s_canvas;
static GFont system_font,custom_font;
static char weather_layer_buffer[32];
static int mycolor[10]={0x3C3D3D,0xA9744C,0xF0435A,0xFFA500,0xFFFF55,
                        0x9DC838,0x0055AA,0xAA55FF,0xAAAAAA,0xFFFFFF};
static int leg_color = 0x555555,cap_color = 0xFFA0AA;
static int totall_time = 0,sw_time = 0,hour = 0,minute = 0;
static int x_flag = 0,count = 0, body_color;


static void update_proc(Layer *layer, GContext *ctx) {
  //resister wire
  graphics_context_set_fill_color(ctx, GColorFromHEX(mycolor[8]));
  graphics_fill_rect(ctx, GRect(0, 30, 144, 4), 0, 0);
  
  //resister body
  body_color = (clock_is_24h_style() == true) ? 0x66AAFF:0xAAAA00;
  graphics_context_set_fill_color(ctx, GColorFromHEX(body_color));
  graphics_fill_circle(ctx, GPoint(36,32),24);
  graphics_fill_circle(ctx, GPoint(108,32),24);
  graphics_fill_rect(ctx, GRect(36, 8, 72, 50), 0, 0);
  
  //resister color bar
  int i = 1,j = 0,c = 0;
  for(c=0;c<4;c++)
  {
    sw_time = totall_time / i;
    if(sw_time == 0){
      graphics_context_set_fill_color(ctx, GColorFromHEX(mycolor[0]));
      graphics_fill_rect(ctx, GRect(96 - j, 8, 15, 50), 0, 0);
    }else{
      sw_time = sw_time % 10;
      graphics_context_set_fill_color(ctx, GColorFromHEX(mycolor[sw_time]));
      graphics_fill_rect(ctx, GRect(96 - j, 8, 15, 50), 0, 0);
    }
    i *= 10;
    j += 20;
  }
 
  //LED of display week
  graphics_context_set_fill_color(ctx, GColorCadetBlue);
  graphics_fill_rect(ctx, GRect(5, 151, 39, 7), 0, 0);
  graphics_fill_rect(ctx, GRect(7, 139, 35, 20), 0, 0);
  graphics_fill_circle(ctx, GPoint(24,139),17);
  graphics_context_set_fill_color(ctx, GColorFromHEX(leg_color));
  graphics_fill_rect(ctx, GRect(12, 150, 4, 28), 0,0);
  graphics_fill_rect(ctx, GRect(16, 150, 4, 6), 0,0);
  graphics_fill_rect(ctx, GRect(32, 150, 4, 28), 0,0);
  graphics_fill_rect(ctx, GRect(28, 150, 4, 6), 0,0);
  
  //Capaciter of display month
  graphics_context_set_fill_color(ctx, GColorFromHEX(leg_color));
  graphics_fill_rect(ctx, GRect(60, 140, 4, 28), 0,0);
  graphics_fill_rect(ctx, GRect(80, 140, 4, 28), 0,0);
  graphics_context_set_fill_color(ctx, GColorFromHEX(cap_color));
  graphics_fill_rect(ctx, GRect(57, 128, 10, 30), 4,GCornersAll);
  graphics_fill_rect(ctx, GRect(77, 128, 10, 30), 4,GCornersAll);
  graphics_fill_circle(ctx, GPoint(72,139),18);
 
  //Capaciter of display date
  graphics_context_set_fill_color(ctx, GColorFromHEX(leg_color));
  graphics_fill_rect(ctx, GRect(108, 140, 4, 28), 0,0);
  graphics_fill_rect(ctx, GRect(128, 140, 4, 28), 0,0);
  graphics_context_set_fill_color(ctx, GColorFromHEX(cap_color));
  graphics_fill_rect(ctx, GRect(105, 128, 10, 30), 4,GCornersAll);
  graphics_fill_rect(ctx, GRect(125, 128, 10, 30), 4,GCornersAll);
  graphics_fill_circle(ctx, GPoint(120,139),18);
  
  //IC of temparature and weather
  int g = 0,a = 0;
  for(g=0;g<8;g++){
    graphics_context_set_fill_color(ctx, GColorFromHEX(mycolor[8]));
    graphics_fill_rect(ctx, GRect(25 + a, 64, 4, 52), 0,0);
    a += 13;
  }
  
  graphics_context_set_fill_color(ctx, GColorDarkGray);
  graphics_fill_rect(ctx, GRect(12, 70, 120, 40), 0,0);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, GPoint(12,90),3);
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, GPoint(18,100),2);
}

static void updateTime(){
  //get time
  time_t s_times = time(NULL);
  struct tm *t_time = localtime(&s_times);
  
  static char time_buffer[] = "00:00";
  static char month_buffer[] = "xx";
  static char day_buffer[] = "xx";
  static char week_buffer[] = "xxx";
  
  if(clock_is_24h_style() == true){
    strftime(time_buffer,  sizeof("00:00"), "%H:%M", t_time);
  }else{
    strftime(time_buffer, sizeof("00:00"), "%I:%M", t_time);
  }
  strftime(month_buffer, sizeof("xx"), "%m", t_time);
  strftime(day_buffer, sizeof("xx"), "%d", t_time);
  
  if(clock_is_24h_style()){
    hour = (t_time->tm_hour)*100;
  }else{
    if(t_time->tm_hour>=12){
      hour = ((t_time->tm_hour)-12)*100;
    }else{
      hour = (t_time->tm_hour)*100;
    }
  }
  
  minute = t_time->tm_min;
  strftime(week_buffer, sizeof("xxx"), "%a", t_time);
  totall_time = hour + minute;
  
 //text setting
  if(x_flag){
    text_layer_set_font(weatfer_or_time_text, custom_font);
    text_layer_set_text(weatfer_or_time_text, time_buffer);
  }else{
    text_layer_set_font(weatfer_or_time_text, system_font);
    text_layer_set_text(weatfer_or_time_text, weather_layer_buffer);
  }
  text_layer_set_text(month_text, month_buffer);
  text_layer_set_text(day_text, day_buffer);
  text_layer_set_text(week_text, week_buffer);
}

static void tick_handler(struct tm *time, TimeUnits unit){
  updateTime();
}

void accel_handler(AccelData *data, uint32_t num_samples){
  /*APP_LOG(APP_LOG_LEVEL_INFO, "accel");*/
  if(data->y > 700){
    x_flag = 1;
    count = 0;
    APP_LOG(APP_LOG_LEVEL_INFO, "Time display");
  }else if(count%10 == 0){
    x_flag = 0;
  }
  count++;
  updateTime();
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
   //store incoming information
  static char temperature_buffer[8];
  static char conditions_buffer[32];
  
  //read first item
  Tuple *t = dict_read_first(iterator);

  //for all items
  while(t != NULL) {
    // which key was received?
    switch(t->key) {
    case KEY_TEMP:
      snprintf(temperature_buffer, sizeof(temperature_buffer), "%dC", (int)t->value->int32);
      break;
    case KEY_COND:
      snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", t->value->cstring);
      break;
    default:
      APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }

    //look for next item
    t = dict_read_next(iterator);
  }
  
  //assemble full string and display
  snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s  %s", temperature_buffer, conditions_buffer);
}
static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void main_window_load(Window *win){
  s_canvas = layer_create(GRect(0, 0, 144, 168));
  layer_set_update_proc(s_canvas, update_proc);
  layer_add_child(window_get_root_layer(win),s_canvas);
  accel_data_service_subscribe(1, accel_handler);
  
  accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  //set font
  system_font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  custom_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_TTF_24));
  
  
  //set weatfer_or_time_text
  weatfer_or_time_text = text_layer_create(GRect(0, 72, 144, 56));
  text_layer_set_background_color(weatfer_or_time_text, GColorClear);
  text_layer_set_text_color(weatfer_or_time_text, GColorWhite);
  text_layer_set_text_alignment(weatfer_or_time_text, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(win), text_layer_get_layer(weatfer_or_time_text));
  
  //set month_text
  month_text = text_layer_create(GRect(58, 122, 30, 30));
  text_layer_set_background_color(month_text, GColorClear);
  text_layer_set_text(month_text, "00:00");
  text_layer_set_font(month_text, system_font);
  text_layer_set_text_alignment(month_text, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(win), text_layer_get_layer(month_text));
  
  //set day_text
  day_text = text_layer_create(GRect(106, 122, 30, 30));
  text_layer_set_background_color(day_text, GColorClear);
  text_layer_set_text(day_text, "00:00");
  text_layer_set_font(day_text, system_font);
  text_layer_set_text_alignment(day_text, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(win), text_layer_get_layer(day_text));
  
  //set week_text
  week_text = text_layer_create(GRect(0, 121, 50, 30));
  text_layer_set_background_color(week_text, GColorClear);
  text_layer_set_text(week_text, "week");
  text_layer_set_font(week_text, system_font);
  text_layer_set_text_alignment(week_text, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(win), text_layer_get_layer(week_text));
}

static void main_window_unload(Window *win){
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Unloading window");
  fonts_unload_custom_font(custom_font);
  text_layer_destroy(weatfer_or_time_text);
  text_layer_destroy(month_text);
  text_layer_destroy(day_text);
  text_layer_destroy(week_text);
  layer_destroy(s_canvas);
  accel_data_service_unsubscribe();
  tick_timer_service_unsubscribe();
}

static void init(){
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Init");
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
    
  s_window = window_create();
  
  window_set_window_handlers(s_window, (WindowHandlers){
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(s_window,true);
}

static void deinit(){
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Deinit");
  window_destroy(s_window);
}

int main(void){
  init();
  app_event_loop();
  deinit();
}