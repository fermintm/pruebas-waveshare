#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "sd_card_bsp.h"
#include "driver/sdmmc_host.h"

#define USER_SDMMC 1
#define USER_SPI   0

#define SD_Read_Mode USER_SPI

#if (SD_Read_Mode == USER_SPI)
#define SD_SPI SPI3_HOST
#define PIN_NUM_MISO  (gpio_num_t)6
#define PIN_NUM_MOSI  (gpio_num_t)5
#define PIN_NUM_CLK   (gpio_num_t)4
#define PIN_NUM_CS    (gpio_num_t)2
#else
#define SDMMC_CMD  (gpio_num_t)5
#define SDMMC_D0   (gpio_num_t)6
#define SDMMC_CLK  (gpio_num_t)4
#endif

#define EXAMPLE_MAX_CHAR_SIZE    64  // Maximum size for reading data
#define SDlist "/sd_card" // Directory, similar to a standard path

esp_err_t s_example_write_file(const char *path, char *data);
esp_err_t s_example_read_file(const char *path);

sdmmc_card_t *card = NULL;

void SD_card_Init(void)
{
#if (SD_Read_Mode == USER_SPI)
  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
    .format_if_mount_failed = false,
    .max_files = 5,  // Maximum number of open files
    .allocation_unit_size = 16 * 1024 // Allocation unit size (similar to sector size)
  };
  spi_bus_config_t bus_cfg = {
    .mosi_io_num = PIN_NUM_MOSI,
    .miso_io_num = PIN_NUM_MISO,
    .sclk_io_num = PIN_NUM_CLK,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .max_transfer_sz = 4000,   // Maximum transfer size   
  };
  ESP_ERROR_CHECK_WITHOUT_ABORT(spi_bus_initialize(SD_SPI, &bus_cfg, SDSPI_DEFAULT_DMA));

  //sdspi_dev_handle_t dev_handle;
  sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
  slot_config.gpio_cs = (gpio_num_t)PIN_NUM_CS;
  slot_config.host_id = SD_SPI;
  //sdspi_host_init_device(&slot_config,&dev_handle);
  sdmmc_host_t host = SDSPI_HOST_DEFAULT();
  host.slot = SD_SPI;
  
  ESP_ERROR_CHECK_WITHOUT_ABORT(esp_vfs_fat_sdspi_mount(SDlist, &host, &slot_config, &mount_config, &card)); // Mount the SD card
  if(card != NULL)
  {
    sdmmc_card_print_info(stdout, card); // Print card information
    printf("practical_size:%.2f\n",(float)(card->csd.capacity)/2048/1024);// Size in GB
  }
  else
  {
    printf("Failed to open the SD card.\n");
  }
#else
  esp_vfs_fat_sdmmc_mount_config_t mount_config = 
  {
    .format_if_mount_failed = true,     // Format SD card if mounting fails
    .max_files = 5,                     // Maximum number of open files
    .allocation_unit_size = 512,        // Allocation unit size (similar to sector size)
  };
  sdmmc_host_t host = SDMMC_HOST_DEFAULT();
  //host.max_freq_khz = SDMMC_FREQ_HIGHSPEED; // High-speed mode

  sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
  slot_config.width = 1;           // 1-wire mode
  slot_config.clk = SDMMC_CLK;
  slot_config.cmd = SDMMC_CMD;
  slot_config.d0 = SDMMC_D0;
  ESP_ERROR_CHECK_WITHOUT_ABORT(esp_vfs_fat_sdmmc_mount(SDlist, &host, &slot_config, &mount_config, &card));

  if(card != NULL)
  {
    sdmmc_card_print_info(stdout, card); // Print card information
    printf("practical_size:%.2f\n",(float)(card->csd.capacity) / 2048 / 1024); // Size in GB
  }
  else
  {
    printf("Failed to open the SD card.\n");
  }
#endif
}

/* Write data
path: file path
data: data to write
*/
esp_err_t s_example_write_file(const char *path, char *data)
{
  esp_err_t err;
  if(card == NULL)
  {
    return ESP_ERR_NOT_FOUND;
  }
  err = sdmmc_get_status(card); // Check if the SD card is available
  if(err != ESP_OK)
  {
    return err;
  }
  FILE *f = fopen(path, "w"); // Open file for writing
  if(f == NULL)
  {
    printf("path:Write Wrong path\n");
    return ESP_ERR_NOT_FOUND;
  }
  fprintf(f, data); // Write data
  fclose(f);
  return ESP_OK;
}
/*
Read data
path: file path
*/
esp_err_t s_example_read_file(const char *path)
{
  esp_err_t err;
  if(card == NULL)
  {
    return ESP_ERR_NOT_FOUND;
  }
  err = sdmmc_get_status(card); // Check if the SD card is available
  if(err != ESP_OK)
  {
    return err;
  }
  FILE *f = fopen(path, "r"); // Open file for reading
  if (f == NULL)
  {
    printf("path:Read Wrong path\n");
  }
  char line[EXAMPLE_MAX_CHAR_SIZE];
  fgets(line, sizeof(line), f); // Read data
  fclose(f);
  char *pos = strchr(line, '\n'); // Find '\n'
  if (pos) 
  {
    *pos = '\0';
  }
  return ESP_OK;
}
/*
Additional file operations:

struct stat st;
stat(file_foo, &st); // Retrieve file information. Returns 0 on success. `file_foo` is the file name as a string (with an extension).
unlink(file_foo); // Delete a file. Returns 0 on success.
rename(char*,char*); // Rename a file.
esp_vfs_fat_sdcard_format(); // Format the SD card.
esp_vfs_fat_sdcard_unmount(mount_point, card); // Unmount the VFS.

File opening modes:
FILE *fopen(const char *filename, const char *mode);
"w": Open file in write mode. Clears the file content if it exists; creates a new file if it doesn't.
"a": Open file in append mode. Creates a new file if it doesn't exist.
mkdir(dirname, mode); // Create a directory.

To read non-text files (like images):
"rb" mode is used to open a file in read-only and binary mode. This is suitable for binary files like images. Using "r" would open the file in text mode, which could corrupt binary data.

For binary file size:
  fseek(file, 0, SEEK_END); // Move the file pointer to the end.
  ftell(file); // Returns the current file pointer position, which is the file size in bytes.
*/