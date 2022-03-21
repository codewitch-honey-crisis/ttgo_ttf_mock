#define LCD_WIDTH 135
#define LCD_HEIGHT 240
#define LCD_HOST    VSPI
#define PIN_NUM_MISO -1
#define PIN_NUM_MOSI 19
#define PIN_NUM_CLK  18
#define PIN_NUM_CS   5

#define PIN_NUM_DC   16
#define PIN_NUM_RST  23
#define PIN_NUM_BCKL 4

#include <Arduino.h>
#include <gfx_cpp14.hpp>
#include <st7789.hpp>
#include "test.h"
using namespace arduino;
using namespace gfx;

using bus_type = tft_spi_ex<VSPI,PIN_NUM_CS,PIN_NUM_MOSI,PIN_NUM_MISO,PIN_NUM_CLK,SPI_MODE0,
true
#ifdef OPTIMIZE_DMA
,LCD_WIDTH*LCD_HEIGHT*2+8
#endif
>;

using lcd_type = st7789<LCD_WIDTH,LCD_HEIGHT,PIN_NUM_DC,PIN_NUM_RST,PIN_NUM_BCKL,bus_type,1>;

lcd_type lcd;
char text[256];
using lcd_color = color<typename lcd_type::pixel_type>;

open_font_cache fcache;
int counter;
float scale;
const open_font& test_font = test_ttf;
uint32_t counter_ts;
const int HZ = 25;
void setup() {
  Serial.begin(115200);
  counter = 0;
  counter_ts = 0;
  scale = test_ttf.scale(50);
  draw::filled_rectangle(lcd,(srect16)lcd.bounds(),lcd_color::white);
}

void loop() {
  using bmp_type = bitmap<typename lcd_type::pixel_type>;
  const uint32_t ms = millis();
    if(ms>counter_ts+(1000/HZ)) {
      ssize16 sz = test_font.measure_text((ssize16)lcd.dimensions(),{0,0},itoa(counter,text,10),scale,0,gfx_encoding::utf8,&fcache);
      void* bmp_data = malloc(bmp_type::sizeof_buffer((size16)sz));
      if(bmp_data!=nullptr) {
        bmp_type bmp((size16)sz,bmp_data);
        draw::filled_rectangle(bmp,(srect16)bmp.bounds(),lcd_color::white);
        draw::text(bmp,sz.bounds(),{0,0},text,test_font,scale, lcd_color::blue,lcd_color::white,false,false,0,gfx_encoding::utf8,nullptr,&fcache);
        srect16 sr = (srect16)bmp.bounds().center(lcd.bounds());
        draw::filled_rectangle(lcd,sr,lcd_color::white);
        draw::bitmap(lcd,sr,bmp,bmp.bounds());
        free(bmp_data);
      }
      ++counter;
      counter_ts = millis();
      
    }
}