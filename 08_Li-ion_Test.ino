#include "Ws_AMOLED.h"
#include "gpio_bsp.h"
#include "i2c_bsp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);
Ws_AMOLED amoled;

void setup()
{
  Serial.begin(115200);
  I2C_master_Init();
  esp32_gpio_init();
  amoled.beginAMOLED_241();
  amoled.setBrightness(100);
  spr.createSprite(600, 450);
  spr.setSwapBytes(1);
}
void loop()
{

}
