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
  uint8_t _add = 0;
  GPIO_SET(example_out_gpio_1,0);
  EXIO_Set_State(TCA_GPIO_5,0);
  delay(200);
  if(GPIO_GET(example_in_gpio_2)==0 && EXIO_Get_State(TCA_GPIO_6)==0)
  {
    _add++;
  }
  GPIO_SET(example_out_gpio_1,1);
  EXIO_Set_State(TCA_GPIO_5,1);
  delay(200);
  if(GPIO_GET(example_in_gpio_2)==1 && EXIO_Get_State(TCA_GPIO_6)==1)
  {
    _add++;
  }
  GPIO_SET(example_out_gpio_1,1);
  EXIO_Set_State(TCA_GPIO_5,0);
  delay(200);
  if(GPIO_GET(example_in_gpio_2)==1 && EXIO_Get_State(TCA_GPIO_6)==0)
  {
    _add++;
  }
  if(_add == 3)
  {
    printf("GPIO test passed.\n");
  }
  else
  {
    printf("GPIO test failed.\n");
  }
}
