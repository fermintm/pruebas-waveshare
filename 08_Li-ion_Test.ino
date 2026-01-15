#include "Ws_AMOLED.h"

#include <TFT_eSPI.h>

#include "gpio_bsp.h"
#include "i2c_bsp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);
Ws_AMOLED amoled;

int bright[6]={40,60,100,126,180,220}; //brightness levels
int b=2; //chosen brightness level
int deb=0;  //touch debounce

void setup()
{
  Serial.begin(115200);
  
    amoled.beginAMOLED_241();
  amoled.setBrightness(200);
  spr.createSprite(600, 450);
  spr.setSwapBytes(1);
  
  I2C_master_Init();
  esp32_gpio_init();
}
void loop()
{
   static int16_t x, y;
  bool touched = amoled.getPoint(&x, &y);
   
  if(touched)
  {
    if(y>320 && y<450 && x>0 && x<180)
    if(deb==0)
      {deb=1;b++; if(b==6) b=0; amoled.setBrightness(bright[b]); }
  }else deb=0;


  spr.setTextDatum(0);
  spr.fillSprite(TFT_BLACK);
  spr.setCursor(20, 20);
  spr.setTextSize(5);
  spr.setTextColor(TFT_WHITE);
  spr.print("HOLA MIKEL");
  amoled.pushColors(0, 0, 600, 450, (uint16_t *)spr.getPointer());
}
