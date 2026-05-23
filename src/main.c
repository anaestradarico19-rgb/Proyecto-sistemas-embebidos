#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
 
// ====================== Configuración de pines ======================
#define I2C_MASTER_SCL_IO          22
#define I2C_MASTER_SDA_IO          21
#define I2C_MASTER_NUM             I2C_NUM_0
#define I2C_MASTER_FREQ_HZ         400000
#define OLED_ADDR                   0x3C
#define OLED_WIDTH                  128
#define OLED_HEIGHT                 64
#define OLED_PAGES                  8
 
#define BUZZER_GPIO                 25
#define RGB_RED_GPIO                26
#define RGB_GREEN_GPIO              27
#define RGB_BLUE_GPIO               14
 
// Botón (activo bajo, pull-up interno)
#define BUTTON_GPIO                 0
 
// Rangos para LED (presión normal)
#define PRESSURE_LED_MIN            950
#define PRESSURE_LED_MAX            1050
 
// Rangos para BUZZER (presión y temperatura)
#define PRESSURE_BUZZER_MIN         849
#define PRESSURE_BUZZER_MAX         851
#define TEMP_BUZZER_MIN             18.0
#define TEMP_BUZZER_MAX             23.0
 
#define TASK_PERIOD_MS              1000
#define MUTE_DURATION_MS            15000   // 15 segundos
 
static QueueHandle_t sensor_data_queue;
 
typedef struct {
    float pressure;
    float temperature;
} sensor_data_t;
 
static float sim_pressure = 1013.25;
static float sim_temperature = 25.0;
 
// Tiempo hasta el cual el buzzer debe permanecer silenciado (en ticks)
static TickType_t mute_until = 0;
 
static const char *TAG = "MANOMETER";
 
// ====================== Fuente 5x7 ======================
static const uint8_t font5x7[95][5] = {
    {0x00,0x00,0x00,0x00,0x00}, // espacio
    {0x00,0x00,0x5F,0x00,0x00}, // !
    {0x00,0x07,0x00,0x07,0x00}, // "
    {0x14,0x7F,0x14,0x7F,0x14}, // #
    {0x24,0x2A,0x7F,0x2A,0x12}, // $
    {0x23,0x13,0x08,0x64,0x62}, // %
    {0x36,0x49,0x55,0x22,0x50}, // &
    {0x00,0x05,0x03,0x00,0x00}, // '
    {0x00,0x1C,0x22,0x41,0x00}, // (
    {0x00,0x41,0x22,0x1C,0x00}, // )
    {0x14,0x08,0x3E,0x08,0x14}, // *
    {0x08,0x08,0x3E,0x08,0x08}, // +
    {0x00,0x50,0x30,0x00,0x00}, // ,
    {0x08,0x08,0x08,0x08,0x08}, // -
    {0x00,0x60,0x60,0x00,0x00}, // .
    {0x20,0x10,0x08,0x04,0x02}, // /
    {0x3E,0x51,0x49,0x45,0x3E}, // 0
    {0x00,0x42,0x7F,0x40,0x00}, // 1
    {0x42,0x61,0x51,0x49,0x46}, // 2
    {0x21,0x41,0x45,0x4B,0x31}, // 3
    {0x18,0x14,0x12,0x7F,0x10}, // 4
    {0x27,0x45,0x45,0x45,0x39}, // 5
    {0x3C,0x4A,0x49,0x49,0x30}, // 6
    {0x01,0x71,0x09,0x05,0x03}, // 7
    {0x36,0x49,0x49,0x49,0x36}, // 8
    {0x06,0x49,0x49,0x29,0x1E}, // 9
    {0x00,0x36,0x36,0x00,0x00}, // :
    {0x00,0x56,0x36,0x00,0x00}, // ;
    {0x08,0x14,0x22,0x41,0x00}, // <
    {0x14,0x14,0x14,0x14,0x14}, // =
    {0x00,0x41,0x22,0x14,0x08}, // >
    {0x02,0x01,0x51,0x09,0x06}, // ?
    {0x3E,0x41,0x5D,0x55,0x1E}, // @
    {0x7E,0x11,0x11,0x11,0x7E}, // A
    {0x7F,0x49,0x49,0x49,0x36}, // B
    {0x3E,0x41,0x41,0x41,0x22}, // C
    {0x7F,0x41,0x41,0x22,0x1C}, // D
    {0x7F,0x49,0x49,0x49,0x41}, // E
    {0x7F,0x09,0x09,0x09,0x01}, // F
    {0x3E,0x41,0x49,0x49,0x3A}, // G
    {0x7F,0x08,0x08,0x08,0x7F}, // H
    {0x00,0x41,0x7F,0x41,0x00}, // I
    {0x20,0x40,0x41,0x3F,0x01}, // J
    {0x7F,0x08,0x14,0x22,0x41}, // K
    {0x7F,0x40,0x40,0x40,0x60}, // L
    {0x7F,0x02,0x0C,0x02,0x7F}, // M
    {0x7F,0x04,0x08,0x10,0x7F}, // N
    {0x3E,0x41,0x41,0x41,0x3E}, // O
    {0x7F,0x09,0x09,0x09,0x06}, // P
    {0x3E,0x41,0x51,0x21,0x5E}, // Q
    {0x7F,0x09,0x19,0x29,0x46}, // R
    {0x46,0x49,0x49,0x49,0x31}, // S
    {0x01,0x01,0x7F,0x01,0x01}, // T
    {0x3F,0x40,0x40,0x40,0x3F}, // U
    {0x1F,0x20,0x40,0x20,0x1F}, // V
    {0x3F,0x40,0x38,0x40,0x3F}, // W
    {0x63,0x14,0x08,0x14,0x63}, // X
    {0x07,0x08,0x70,0x08,0x07}, // Y
    {0x61,0x51,0x49,0x45,0x43}, // Z
    {0x00,0x7F,0x41,0x41,0x00}, // [
    {0x02,0x04,0x08,0x10,0x20}, // backslash
    {0x00,0x41,0x41,0x7F,0x00}, // ]
    {0x04,0x02,0x01,0x02,0x04}, // ^
    {0x40,0x40,0x40,0x40,0x40}, // _
    {0x00,0x01,0x02,0x04,0x00}, // `
    {0x20,0x54,0x54,0x54,0x78}, // a
    {0x7F,0x48,0x44,0x44,0x38}, // b
    {0x38,0x44,0x44,0x44,0x20}, // c
    {0x38,0x44,0x44,0x48,0x7F}, // d
    {0x38,0x54,0x54,0x54,0x18}, // e
    {0x08,0x7E,0x09,0x01,0x02}, // f
    {0x0C,0x52,0x52,0x52,0x3E}, // g
    {0x7F,0x08,0x04,0x04,0x78}, // h
    {0x00,0x44,0x7D,0x40,0x00}, // i
    {0x20,0x40,0x44,0x3D,0x00}, // j
    {0x7F,0x10,0x28,0x44,0x00}, // k
    {0x00,0x41,0x7F,0x40,0x00}, // l
    {0x7C,0x04,0x18,0x04,0x78}, // m
    {0x7C,0x08,0x04,0x04,0x78}, // n
    {0x38,0x44,0x44,0x44,0x38}, // o
    {0x7C,0x14,0x14,0x14,0x08}, // p
    {0x08,0x14,0x14,0x18,0x7C}, // q
    {0x7C,0x08,0x04,0x04,0x08}, // r
    {0x48,0x54,0x54,0x54,0x20}, // s
    {0x04,0x3F,0x44,0x40,0x20}, // t
    {0x3C,0x40,0x40,0x20,0x7C}, // u
    {0x1C,0x20,0x40,0x20,0x1C}, // v
    {0x3C,0x40,0x30,0x40,0x3C}, // w
    {0x44,0x28,0x10,0x28,0x44}, // x
    {0x0C,0x50,0x50,0x50,0x3C}, // y
    {0x44,0x64,0x54,0x4C,0x44}, // z
    {0x00,0x08,0x36,0x41,0x00}, // {
    {0x00,0x00,0x7F,0x00,0x00}, // |
    {0x00,0x41,0x36,0x08,0x00}, // }
    {0x08,0x04,0x08,0x10,0x08}  // ~
};
 
// ====================== I2C y OLED ======================
static void i2c_master_init(void) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0));
}
 
static void oled_write_cmd(uint8_t cmd) {
    uint8_t buf[2] = {0x00, cmd};
    i2c_master_write_to_device(I2C_MASTER_NUM, OLED_ADDR, buf, 2, pdMS_TO_TICKS(100));
}
 
static void oled_write_data(uint8_t data) {
    uint8_t buf[2] = {0x40, data};
    i2c_master_write_to_device(I2C_MASTER_NUM, OLED_ADDR, buf, 2, pdMS_TO_TICKS(100));
}
 
static void oled_init(void) {
    vTaskDelay(pdMS_TO_TICKS(50));
    oled_write_cmd(0xAE); oled_write_cmd(0xD5); oled_write_cmd(0x80);
    oled_write_cmd(0xA8); oled_write_cmd(0x3F); oled_write_cmd(0xD3); oled_write_cmd(0x00);
    oled_write_cmd(0x40); oled_write_cmd(0x8D); oled_write_cmd(0x14);
    oled_write_cmd(0x20); oled_write_cmd(0x00); oled_write_cmd(0xA1); oled_write_cmd(0xC8);
    oled_write_cmd(0xDA); oled_write_cmd(0x12); oled_write_cmd(0x81); oled_write_cmd(0xCF);
    oled_write_cmd(0xD9); oled_write_cmd(0xF1); oled_write_cmd(0xDB); oled_write_cmd(0x40);
    oled_write_cmd(0xA4); oled_write_cmd(0xA6); oled_write_cmd(0xAF);
}
 
static void oled_clear(void) {
    for (int page = 0; page < OLED_PAGES; page++) {
        oled_write_cmd(0xB0 + page);
        oled_write_cmd(0x00);
        oled_write_cmd(0x10);
        for (int i = 0; i < OLED_WIDTH; i++) oled_write_data(0x00);
    }
}
 
static void oled_draw_char(char c, uint8_t x, uint8_t page) {
    if (c < 32 || c > 126) c = '?';
    const uint8_t *glyph = font5x7[c - 32];
    if (x > OLED_WIDTH - 6) return;
    oled_write_cmd(0xB0 + page);
    oled_write_cmd(x & 0x0F);
    oled_write_cmd(0x10 | (x >> 4));
    for (int i = 0; i < 5; i++) oled_write_data(glyph[i]);
    oled_write_data(0x00);
}
 
static void oled_draw_string(const char *str, uint8_t x, uint8_t page) {
    while (*str && x < OLED_WIDTH - 5) {
        oled_draw_char(*str, x, page);
        x += 6; str++;
    }
}
 
static void oled_display(float pressure, float temp) {
    oled_clear();
    oled_draw_string("MANOMETER", 25, 0);
    char line[20];
    snprintf(line, sizeof(line), "PRES: %.1f hPa", pressure);
    oled_draw_string(line, 5, 2);
    snprintf(line, sizeof(line), "TEMP: %.1f C", temp);
    oled_draw_string(line, 5, 4);
}
 
// ====================== Buzzer y LED RGB ======================
static void buzzer_init(void) {
    gpio_config_t io = { .pin_bit_mask = (1ULL<<BUZZER_GPIO), .mode = GPIO_MODE_OUTPUT };
    gpio_config(&io);
    gpio_set_level(BUZZER_GPIO, 0);
}
static void buzzer_set(bool on) { gpio_set_level(BUZZER_GPIO, on); }
 
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_RESOLUTION LEDC_TIMER_8_BIT
#define LEDC_FREQ 5000
 
static void rgb_init(void) {
    ledc_timer_config_t timer = { .speed_mode = LEDC_MODE, .duty_resolution = LEDC_RESOLUTION, .timer_num = LEDC_TIMER, .freq_hz = LEDC_FREQ };
    ledc_timer_config(&timer);
    ledc_channel_config_t ch_r = { .gpio_num = RGB_RED_GPIO, .speed_mode = LEDC_MODE, .channel = LEDC_CHANNEL_0, .timer_sel = LEDC_TIMER, .duty = 0 };
    ledc_channel_config(&ch_r);
    ledc_channel_config_t ch_g = { .gpio_num = RGB_GREEN_GPIO, .speed_mode = LEDC_MODE, .channel = LEDC_CHANNEL_1, .timer_sel = LEDC_TIMER, .duty = 0 };
    ledc_channel_config(&ch_g);
    ledc_channel_config_t ch_b = { .gpio_num = RGB_BLUE_GPIO, .speed_mode = LEDC_MODE, .channel = LEDC_CHANNEL_2, .timer_sel = LEDC_TIMER, .duty = 0 };
    ledc_channel_config(&ch_b);
}
static void rgb_set(uint8_t r, uint8_t g, uint8_t b) {
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_0, r); ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_0);
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_1, g); ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1);
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_2, b); ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_2);
}
static void rgb_update_led(bool ok) { rgb_set(ok ? 0 : 255, ok ? 255 : 0, 0); }
 
// ====================== Botón (silencia 15 segundos) ======================
static void button_init(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUTTON_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
}
 
static void button_task(void *pvParameters) {
    uint32_t last_stable = 0;
    bool last_reading = true;      // true = no presionado
    const uint32_t debounce_ms = 50;
 
    while (1) {
        bool raw = gpio_get_level(BUTTON_GPIO);
        static bool last_raw = true;
        uint32_t now = xTaskGetTickCount();
 
        if (raw != last_raw) {
            last_stable = now;
            last_raw = raw;
        }
 
        if ((now - last_stable) >= pdMS_TO_TICKS(debounce_ms)) {
            bool pressed = (raw == 0);  // activo bajo
            if (pressed != last_reading) {
                last_reading = pressed;
                if (pressed) {
                    // Flanco de bajada: botón presionado → activar mute por 15 s
                    mute_until = xTaskGetTickCount() + pdMS_TO_TICKS(MUTE_DURATION_MS);
                    ESP_LOGI(TAG, "Buzzer silenciado por %d ms", MUTE_DURATION_MS);
                }
                // No hacemos nada al soltar el botón
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
 
// ====================== Tareas principales ======================
static void sensor_simulation_task(void *pvParameters) {
    sensor_data_t data;
    TickType_t last = xTaskGetTickCount();
    while (1) {
        data.pressure = sim_pressure;
        data.temperature = sim_temperature;
        bool pressure_ok = (data.pressure >= PRESSURE_LED_MIN && data.pressure <= PRESSURE_LED_MAX);
        bool buzz_p = (data.pressure < PRESSURE_BUZZER_MIN || data.pressure > PRESSURE_BUZZER_MAX);
        bool buzz_t = (data.temperature < TEMP_BUZZER_MIN || data.temperature > TEMP_BUZZER_MAX);
        bool buzz = buzz_p || buzz_t;
 
        // Determinar si el buzzer debe sonar según lógica normal y mute temporizado
        bool buzzer_enable = false;
        if (buzz) {
            TickType_t now = xTaskGetTickCount();
            if (now >= mute_until) {
                buzzer_enable = true;   // mute expirado y aún hay alarma
            } else {
                buzzer_enable = false;  // dentro del período de mute
            }
        } else {
            buzzer_enable = false;      // sin condición de alarma
        }
 
        rgb_update_led(pressure_ok);
        buzzer_set(buzzer_enable);
 
        xQueueSend(sensor_data_queue, &data, 0);
        printf("P=%.2f hPa  T=%.2f C\r\n", data.pressure, data.temperature);
        fflush(stdout);
 
        vTaskDelayUntil(&last, pdMS_TO_TICKS(TASK_PERIOD_MS));
    }
}
 
static void display_task(void *pvParameters) {
    sensor_data_t data;
    while (1) {
        if (xQueueReceive(sensor_data_queue, &data, portMAX_DELAY))
            oled_display(data.pressure, data.temperature);
    }
}
 
static void uart_command_task(void *pvParameters) {
    char line[32];
    printf("\r\nSistema listo. Comandos: set_pressure <hPa>   set_temp <°C>\r\n");
    while (1) {
        if (fgets(line, sizeof(line), stdin) != NULL) {
            line[strcspn(line, "\r\n")] = '\0';
            if (strlen(line) == 0) continue;
            float val;
            if (strncmp(line, "set_pressure ", 13) == 0 && sscanf(line + 13, "%f", &val) == 1) {
                sim_pressure = val;
                ESP_LOGI(TAG, "Presión manual = %.2f hPa", sim_pressure);
            } else if (strncmp(line, "set_temp ", 9) == 0 && sscanf(line + 9, "%f", &val) == 1) {
                sim_temperature = val;
                ESP_LOGI(TAG, "Temperatura manual = %.2f C", sim_temperature);
            } else if (strcmp(line, "help") == 0) {
                printf("Comandos:\r\n  set_pressure <hPa>\r\n  set_temp <°C>\r\n");
            } else if (strlen(line) > 0) {
                printf("Comando no reconocido. Use 'help'\r\n");
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
 
void app_main(void) {
    ESP_LOGI(TAG, "=== MANÓMETRO CON BOTÓN (MUTE 15s) ===");
    i2c_master_init();
    oled_init();
    buzzer_init();
    rgb_init();
    button_init();
 
    sensor_data_queue = xQueueCreate(5, sizeof(sensor_data_t));
 
    xTaskCreate(sensor_simulation_task, "sim", 4096, NULL, 5, NULL);
    xTaskCreate(display_task, "disp", 4096, NULL, 4, NULL);
    xTaskCreate(uart_command_task, "cmd", 4096, NULL, 3, NULL);
    xTaskCreate(button_task, "btn", 2048, NULL, 2, NULL);
 
    ESP_LOGI(TAG, "Sistema listo. Pulse el botón (GPIO0) para silenciar el buzzer 15 segundos.");
}