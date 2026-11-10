#ifndef _esp_loadcell_H_
#define _esp_loadcell_H_

#include <stdio.h>
#include "esp_err.h"
#include "esp_check.h"
#include "driver/gpio.h"

typedef struct esp_loadcell_t *esp_loadcell_handle_t;

typedef enum {
    ADS1232 = 0,
    HX711
} loadcell_adc_type_t;

typedef enum {
    ADS1232_CHANNEL_A = 0, // Canal A1/A2 diferencial (A0 = LOW)
    ADS1232_CHANNEL_B = 1  // Canal B1/B2 diferencial (A0 = HIGH)
} ads1232_channel_t;

typedef struct {
    gpio_num_t dout;
    gpio_num_t pd_sck;
    gpio_num_t speed_io;
    gpio_num_t gain0_io;
    gpio_num_t gain1_io;
    gpio_num_t pdwn_io;
    gpio_num_t a0_io;

    uint8_t speed;       // 1 --> 80Hz, 0 --> 10Hz
    loadcell_adc_type_t type;
    uint32_t gain;
} esp_loadcell_config_t;

/**
 * @brief Inicializa o driver da célula de carga (ADS1232 / HX711).
 */
esp_err_t init_esp_loadcell(esp_loadcell_config_t *config, esp_loadcell_handle_t *handle);

/**
 * @brief Le o valor bruto convertido pelo ADC (24 bits).
 */
esp_err_t esp_loadcell_read(esp_loadcell_handle_t handle, int32_t *data_read);

/**
 * @brief Ajusta a velocidade de leitura (1 = 80Hz, 0 = 10Hz).
 */
esp_err_t esp_loadcell_set_speed(esp_loadcell_handle_t handle, uint8_t speed);

/**
 * @brief Define o ganho do ADS1232 dinamicamente (1, 2, 64 ou 128).
 */
esp_err_t esp_loadcell_set_gain(esp_loadcell_handle_t handle, uint32_t gain);

/**
 * @brief Seleciona o canal de entrada do ADS1232 (ADS1232_CHANNEL_A ou ADS1232_CHANNEL_B).
 */
esp_err_t esp_loadcell_set_channel(esp_loadcell_handle_t handle, ads1232_channel_t channel);

/**
 * @brief Controle de Power-Down / Reset para o ADS1232.
 * @param enable true coloca o chip em standby (PDWN = LOW), false reativa (PDWN = HIGH).
 */
esp_err_t esp_loadcell_power_down(esp_loadcell_handle_t handle, bool enable);

#endif // _esp_loadcell_H_