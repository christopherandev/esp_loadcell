#include <stdio.h>
#include "esp_loadcell.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    esp_loadcell_handle_t esp_loadcell_handle;

    esp_loadcell_config_t esp_loadcell_config = {
        .dout = 19,//GPIO_NUM_19,
        .pd_sck = 18,//GPIO_NUM_18,
        .speed_io = 4,
        .gain0_io = 21,
        .gain1_io = 22,
        .pdwn_io = 25,
        .a0_io = 23,
        .speed = 0, // 1 --> 80Hz, 0 --> 10Hz
        .type = ADS1232,
        .gain = 1};

    init_esp_loadcell(&esp_loadcell_config, &esp_loadcell_handle);

    esp_loadcell_set_speed(esp_loadcell_handle,0);

    int32_t value = 0;
    int counter = 0;
    uint32_t gains_to_test[] = {1, 2, 64, 128};
    int gain_index = 0;

    esp_err_t r;
    while (1)
    {
        r = esp_loadcell_read(esp_loadcell_handle, &value);
        if(r == ESP_OK)
        {
            counter++;
            // A cada 5 leituras, altera o ganho dinamicamente para teste
            if (counter >= 5)
            {
                counter = 0;
                gain_index = (gain_index + 1) % 4;
                uint32_t new_gain = gains_to_test[gain_index];
                
                printf("Alterando ganho do ADS1232 para: %lu\n", new_gain);
                esp_loadcell_set_gain(esp_loadcell_handle, new_gain);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
