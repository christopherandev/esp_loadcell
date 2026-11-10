#include <stdio.h>
#include "esp_loadcell.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "main";

void app_main(void)
{
    esp_loadcell_handle_t esp_loadcell_handle;

    esp_loadcell_config_t esp_loadcell_config = {
        .dout = 19,      // GPIO_NUM_19
        .pd_sck = 18,    // GPIO_NUM_18
        .speed_io = 4,   // GPIO_NUM_4
        .gain0_io = 21,  // GPIO_NUM_21
        .gain1_io = 22,  // GPIO_NUM_22
        .pdwn_io = 25,   // GPIO_NUM_25
        .a0_io = 23,     // GPIO_NUM_23
        .speed = 0,      // 0 --> 10Hz, 1 --> 80Hz
        .type = ADS1232,
        .gain = 1        // Ganho inicial = 1
    };

    ESP_ERROR_CHECK(init_esp_loadcell(&esp_loadcell_config, &esp_loadcell_handle));

    // Ajusta a velocidade inicial para 10Hz
    esp_loadcell_set_speed(esp_loadcell_handle, 0);

    int32_t value = 0;
    int counter = 0;
    
    // Lista de ganhos para alternar durante o teste
    uint32_t gains_to_test[] = {1, 2, 64, 128};
    int gain_index = 0;

    ads1232_channel_t current_channel = ADS1232_CHANNEL_A;

    esp_err_t r;
    while (1)
    {
        r = esp_loadcell_read(esp_loadcell_handle, &value);
        if (r == ESP_OK)
        {
            counter++;

            // A cada 5 leituras válidas, exercita as novas funções da biblioteca
            if (counter >= 5)
            {
                counter = 0;

                // 1. Altera o ganho dinamicamente (1 -> 2 -> 64 -> 128)
                gain_index = (gain_index + 1) % 4;
                uint32_t new_gain = gains_to_test[gain_index];
                ESP_LOGI(TAG, "==> Teste: Alterando ganho para %lu", new_gain);
                esp_loadcell_set_gain(esp_loadcell_handle, new_gain);

                // 2. Alterna a entrada entre Canal A e Canal B
                current_channel = (current_channel == ADS1232_CHANNEL_A) ? ADS1232_CHANNEL_B : ADS1232_CHANNEL_A;
                ESP_LOGI(TAG, "==> Teste: Alternando para %s", (current_channel == ADS1232_CHANNEL_B) ? "Canal B" : "Canal A");
                esp_loadcell_set_channel(esp_loadcell_handle, current_channel);

                // 3. Simula entrada e saída do modo Power-Down
                ESP_LOGI(TAG, "==> Teste: Entrando em modo Power-Down (standby)...");
                esp_loadcell_power_down(esp_loadcell_handle, true);
                
                // Tentativa de leitura em Power-Down (deve retornar erro ESP_ERR_INVALID_STATE)
                esp_err_t err_pd = esp_loadcell_read(esp_loadcell_handle, &value);
                ESP_LOGW(TAG, "Status da leitura em Power-Down: %s (esperado ESP_ERR_INVALID_STATE)", esp_err_to_name(err_pd));

                vTaskDelay(pdMS_TO_TICKS(1000)); // Permanece 1s desativado

                ESP_LOGI(TAG, "==> Teste: Reativando o ADS1232 (Power-Up)...");
                esp_loadcell_power_down(esp_loadcell_handle, false);
                vTaskDelay(pdMS_TO_TICKS(100)); // Tempo de settling/estabilização do Front-End analógico
            }
        }
        else
        {
            ESP_LOGE(TAG, "Falha na leitura do sensor: %s", esp_err_to_name(r));
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}