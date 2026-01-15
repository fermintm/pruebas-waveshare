#include "gpio_bsp.h"
#include "i2c_bsp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
void setup()
{
  Serial.begin(115200);
  I2C_master_Init();
  esp32_gpio_init();
}
void loop()
{
  
}
