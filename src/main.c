// Codigo del proyecto completo 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/uart.h"
#include "esp_vfs_dev.h"
#include "esp_log.h"
#include "bme280.h"


// CONFIGURACIÓN — GPIO Y DIRECCIONES
#define I2C_PORT             I2C_NUM_0
#define I2C_SDA_GPIO         21
#define I2C_SCL_GPIO         22
#define I2C_FREQ_HZ          100000

#define BME_IN_ADDR          0x76   
#define BME_OUT_ADDR         0x77   
#define OLED_ADDR            0x3C

#define LED_R_GPIO           25
#define LED_G_GPIO           26
#define LED_B_GPIO           27
#define BUZZER_GPIO          18
#define BUTTON_GPIO          19

#define BUZ_LEDC_TIMER       LEDC_TIMER_0
#define BUZ_LEDC_CHANNEL     LEDC_CHANNEL_0
#define BUZ_LEDC_MODE        LEDC_LOW_SPEED_MODE
#define BUZ_FREQ_HZ          2000

#define DIFF_HYST_HPA        0.2f
#define UART_CONSOLE_NUM     UART_NUM_0
#define UART_BAUD            115200

static const char *TAG = "BARO";

//Estados compartidos
typedef struct { float p_min, p_max, t_min, t_max; } range_t;

// Rangos por defecto para medellin 
static range_t r_in  = { 820.0f, 870.0f, 18.0f, 28.0f }; 
static range_t r_out = { 820.0f, 870.0f, 10.0f, 32.0f };

static float in_p = 0, in_t = 0, in_h = 0;
static float out_p = 0, out_t = 0, out_h = 0;

// Estado de validez de las lecturas
static bool  in_ok = false, out_ok = false;

// Estado del sistema de alarma 
static volatile TickType_t buzzer_mute_until = 0;
static volatile bool alarm_active    = false;

static SemaphoreHandle_t i2c_mtx;
static SemaphoreHandle_t data_mtx;
static QueueHandle_t     btn_evt_q;


//INTERFAZ I2C PARA LIBRERÍA BOSCH BME280 

// Escritura I2C requerida por la librería Bosch 
BME280_INTF_RET_TYPE bme280_i2c_write(uint8_t reg, const uint8_t *data,
                                       uint32_t len, void *intf_ptr) {
    uint8_t addr = *(uint8_t *)intf_ptr;
    uint8_t buf[32];
    if (len + 1 > sizeof(buf)) return -1;
    buf[0] = reg;
    memcpy(buf + 1, data, len);
    xSemaphoreTake(i2c_mtx, portMAX_DELAY);
    esp_err_t r = i2c_master_write_to_device(I2C_PORT, addr, buf, len + 1,
                                              pdMS_TO_TICKS(1000));
    xSemaphoreGive(i2c_mtx);
    return (r == ESP_OK) ? BME280_INTF_RET_SUCCESS : -1;
}

// Lectura I2C requerida por la librería Bosch
BME280_INTF_RET_TYPE bme280_i2c_read(uint8_t reg, uint8_t *data,
                                      uint32_t len, void *intf_ptr) {
    uint8_t addr = *(uint8_t *)intf_ptr;
    xSemaphoreTake(i2c_mtx, portMAX_DELAY);
    esp_err_t r = i2c_master_write_read_device(I2C_PORT, addr, &reg, 1,
                                                data, len, pdMS_TO_TICKS(1000));
    xSemaphoreGive(i2c_mtx);
    return (r == ESP_OK) ? BME280_INTF_RET_SUCCESS : -1;
}

// Delay requerido por la librería Bosch
void bme280_delay_us(uint32_t period, void *intf_ptr) {
    (void)intf_ptr;
    vTaskDelay(pdMS_TO_TICKS(period / 1000 + 1));
}


// FONT 5×7 ASCII (0x20–0x7E)
// Tabla de caracteres para el display OLED, cada carácter es una matriz de 5 columnas x 7 filas (bits) 

static const uint8_t font5x7[][5] = {
    {0x00,0x00,0x00,0x00,0x00},{0x00,0x00,0x5F,0x00,0x00},
    {0x00,0x07,0x00,0x07,0x00},{0x14,0x7F,0x14,0x7F,0x14},
    {0x24,0x2A,0x7F,0x2A,0x12},{0x23,0x13,0x08,0x64,0x62},
    {0x36,0x49,0x55,0x22,0x50},{0x00,0x05,0x03,0x00,0x00},
    {0x00,0x1C,0x22,0x41,0x00},{0x00,0x41,0x22,0x1C,0x00},
    {0x08,0x2A,0x1C,0x2A,0x08},{0x08,0x08,0x3E,0x08,0x08},
    {0x00,0x50,0x30,0x00,0x00},{0x08,0x08,0x08,0x08,0x08},
    {0x00,0x60,0x60,0x00,0x00},{0x20,0x10,0x08,0x04,0x02},
    {0x3E,0x51,0x49,0x45,0x3E},{0x00,0x42,0x7F,0x40,0x00},
    {0x42,0x61,0x51,0x49,0x46},{0x21,0x41,0x45,0x4B,0x31},
    {0x18,0x14,0x12,0x7F,0x10},{0x27,0x45,0x45,0x45,0x39},
    {0x3C,0x4A,0x49,0x49,0x30},{0x01,0x71,0x09,0x05,0x03},
    {0x36,0x49,0x49,0x49,0x36},{0x06,0x49,0x49,0x29,0x1E},
    {0x00,0x36,0x36,0x00,0x00},{0x00,0x56,0x36,0x00,0x00},
    {0x00,0x08,0x14,0x22,0x41},{0x14,0x14,0x14,0x14,0x14},
    {0x41,0x22,0x14,0x08,0x00},{0x02,0x01,0x51,0x09,0x06},
    {0x32,0x49,0x79,0x41,0x3E},{0x7E,0x11,0x11,0x11,0x7E},
    {0x7F,0x49,0x49,0x49,0x36},{0x3E,0x41,0x41,0x41,0x22},
    {0x7F,0x41,0x41,0x22,0x1C},{0x7F,0x49,0x49,0x49,0x41},
    {0x7F,0x09,0x09,0x01,0x01},{0x3E,0x41,0x41,0x51,0x32},
    {0x7F,0x08,0x08,0x08,0x7F},{0x00,0x41,0x7F,0x41,0x00},
    {0x20,0x40,0x41,0x3F,0x01},{0x7F,0x08,0x14,0x22,0x41},
    {0x7F,0x40,0x40,0x40,0x40},{0x7F,0x02,0x04,0x02,0x7F},
    {0x7F,0x04,0x08,0x10,0x7F},{0x3E,0x41,0x41,0x41,0x3E},
    {0x7F,0x09,0x09,0x09,0x06},{0x3E,0x41,0x51,0x21,0x5E},
    {0x7F,0x09,0x19,0x29,0x46},{0x46,0x49,0x49,0x49,0x31},
    {0x01,0x01,0x7F,0x01,0x01},{0x3F,0x40,0x40,0x40,0x3F},
    {0x1F,0x20,0x40,0x20,0x1F},{0x7F,0x20,0x18,0x20,0x7F},
    {0x63,0x14,0x08,0x14,0x63},{0x03,0x04,0x78,0x04,0x03},
    {0x61,0x51,0x49,0x45,0x43},{0x00,0x00,0x7F,0x41,0x41},
    {0x02,0x04,0x08,0x10,0x20},{0x41,0x41,0x7F,0x00,0x00},
    {0x04,0x02,0x01,0x02,0x04},{0x40,0x40,0x40,0x40,0x40},
    {0x00,0x01,0x02,0x04,0x00},{0x20,0x54,0x54,0x54,0x78},
    {0x7F,0x48,0x44,0x44,0x38},{0x38,0x44,0x44,0x44,0x20},
    {0x38,0x44,0x44,0x48,0x7F},{0x38,0x54,0x54,0x54,0x18},
    {0x08,0x7E,0x09,0x01,0x02},{0x08,0x14,0x54,0x54,0x3C},
    {0x7F,0x08,0x04,0x04,0x78},{0x00,0x44,0x7D,0x40,0x00},
    {0x20,0x40,0x44,0x3D,0x00},{0x00,0x7F,0x10,0x28,0x44},
    {0x00,0x41,0x7F,0x40,0x00},{0x7C,0x04,0x18,0x04,0x78},
    {0x7C,0x08,0x04,0x04,0x78},{0x38,0x44,0x44,0x44,0x38},
    {0x7C,0x14,0x14,0x14,0x08},{0x08,0x14,0x14,0x18,0x7C},
    {0x7C,0x08,0x04,0x04,0x08},{0x48,0x54,0x54,0x54,0x20},
    {0x04,0x3F,0x44,0x40,0x20},{0x3C,0x40,0x40,0x20,0x7C},
    {0x1C,0x20,0x40,0x20,0x1C},{0x3C,0x40,0x30,0x40,0x3C},
    {0x44,0x28,0x10,0x28,0x44},{0x0C,0x50,0x50,0x50,0x3C},
    {0x44,0x64,0x54,0x4C,0x44},{0x00,0x08,0x36,0x41,0x00},
    {0x00,0x00,0x7F,0x00,0x00},{0x00,0x41,0x36,0x08,0x00},
    {0x02,0x01,0x02,0x04,0x02},
};


//  DRIVER SH1106 128×64
#define SH1106_W      128
#define SH1106_PAGES  8
#define SH1106_COL_OF 2

static uint8_t oled_fb[SH1106_W * SH1106_PAGES];

// Envia un comando al SH1106 a través de I2C (control byte 0x00)  
static void sh1106_cmd(uint8_t c) {
    uint8_t b[2] = { 0x00, c };
    xSemaphoreTake(i2c_mtx, portMAX_DELAY);
    i2c_master_write_to_device(I2C_PORT, OLED_ADDR, b, 2, pdMS_TO_TICKS(100));
    xSemaphoreGive(i2c_mtx);
}

// Inicializa el SH1106 
static void sh1106_init(void) {
    static const uint8_t seq[] = {
        0xAE, 0xD5, 0x80, 0xA8, 0x3F, 0xD3, 0x00, 0x40,
        0xAD, 0x8B, 0xA1, 0xC8, 0xDA, 0x12, 0x81, 0x80,
        0xD9, 0xF1, 0xDB, 0x40, 0xA4, 0xA6, 0xAF
    };
    for (size_t i = 0; i < sizeof(seq); i++) sh1106_cmd(seq[i]);
    ESP_LOGI(TAG, "OLED SH1106 inicializado (0x3C)");
}

static void sh1106_clear(void) { memset(oled_fb, 0, sizeof(oled_fb)); }

static void sh1106_update(void) {
    for (int page = 0; page < SH1106_PAGES; page++) {
        sh1106_cmd(0xB0 | page);
        sh1106_cmd(0x00 | (SH1106_COL_OF & 0x0F));
        sh1106_cmd(0x10 | ((SH1106_COL_OF >> 4) & 0x0F));
        uint8_t buf[1 + SH1106_W];
        buf[0] = 0x40;
        memcpy(&buf[1], &oled_fb[page * SH1106_W], SH1106_W);
        xSemaphoreTake(i2c_mtx, portMAX_DELAY);
        i2c_master_write_to_device(I2C_PORT, OLED_ADDR, buf, sizeof(buf),
                                    pdMS_TO_TICKS(100));
        xSemaphoreGive(i2c_mtx);
    }
}

static void sh1106_draw_char(int x, int page, char c) {
    if (c < 0x20 || c > 0x7E) c = '?';
    if (x < 0 || x + 5 >= SH1106_W || page < 0 || page > 7) return;
    const uint8_t *g = font5x7[c - 0x20];
    for (int i = 0; i < 5; i++) oled_fb[page * SH1106_W + x + i] = g[i];
    oled_fb[page * SH1106_W + x + 5] = 0x00;
}

static void sh1106_draw_str(int x, int page, const char *s) {
    while (*s) { sh1106_draw_char(x, page, *s++); x += 6; }
}

// Led RGB (Catodo comun) conectado a GPIOs

// Inicializa los GPIOs del LED RGB como salidas digitales
static void rgb_init(void) {
    gpio_config_t io = {
        .pin_bit_mask = (1ULL<<LED_R_GPIO)|(1ULL<<LED_G_GPIO)|(1ULL<<LED_B_GPIO),
        .mode         = GPIO_MODE_OUTPUT,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE
    };
    gpio_config(&io);
}

 // Actualiza el LED RGB según la diferencia de presión.
 // Rojo: presiones iguales
 // Verde: afuera > adentro
 // Azul: adentro > afuera 
static void rgb_set(int r, int g, int b) {
    gpio_set_level(LED_R_GPIO, r ? 1 : 0);
    gpio_set_level(LED_G_GPIO, g ? 1 : 0);
    gpio_set_level(LED_B_GPIO, b ? 1 : 0);
}

// BUZZER (PWM 2 kHz via LEDC)
static void buzzer_init(void) {
    ledc_timer_config_t t = {
        .speed_mode      = BUZ_LEDC_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num       = BUZ_LEDC_TIMER,
        .freq_hz         = BUZ_FREQ_HZ,
        .clk_cfg         = LEDC_AUTO_CLK
    };
    ledc_timer_config(&t);
    ledc_channel_config_t ch = {
        .gpio_num   = BUZZER_GPIO,
        .speed_mode = BUZ_LEDC_MODE,
        .channel    = BUZ_LEDC_CHANNEL,
        .timer_sel  = BUZ_LEDC_TIMER,
        .duty       = 0,
        .hpoint     = 0
    };
    ledc_channel_config(&ch);
}

static void buzzer_on(void) {
    ledc_set_duty(BUZ_LEDC_MODE, BUZ_LEDC_CHANNEL, 512);
    ledc_update_duty(BUZ_LEDC_MODE, BUZ_LEDC_CHANNEL);
}

static void buzzer_off(void) {
    ledc_set_duty(BUZ_LEDC_MODE, BUZ_LEDC_CHANNEL, 0);
    ledc_update_duty(BUZ_LEDC_MODE, BUZ_LEDC_CHANNEL);
}

// BOTÓN — ISR + debounce
static void IRAM_ATTR btn_isr(void *arg) {
    (void)arg;
    uint32_t v = 1;
    xQueueSendFromISR(btn_evt_q, &v, NULL);
}

static void button_init(void) {
    gpio_config_t io = {
        .pin_bit_mask = 1ULL << BUTTON_GPIO,
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .intr_type    = GPIO_INTR_NEGEDGE
    };
    gpio_config(&io);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_GPIO, btn_isr, NULL);
}

static void button_task(void *arg) {
    (void)arg;
    uint32_t evt;
    while (1) {
        if (xQueueReceive(btn_evt_q, &evt, portMAX_DELAY)) {
            vTaskDelay(pdMS_TO_TICKS(40));
            if (gpio_get_level(BUTTON_GPIO) == 0) {
                if (alarm_active) {
                    buzzer_mute_until = xTaskGetTickCount() + pdMS_TO_TICKS(30000); 
                    buzzer_off();
                    buzzer_off();
                    ESP_LOGI(TAG, "Boton -> buzzer silenciado");
                }
                vTaskDelay(pdMS_TO_TICKS(200));
                xQueueReset(btn_evt_q);
            }
        }
    }
}

// INICIALIZACIÓN BME280

// Inicializa un sensor BME280 con la librería Bosch.
static int8_t setup_bme(struct bme280_dev *d, uint8_t *addr_ptr) {
    d->intf     = BME280_I2C_INTF;
    d->intf_ptr = addr_ptr;
    d->read     = bme280_i2c_read;
    d->write    = bme280_i2c_write;
    d->delay_us = bme280_delay_us;

    int8_t r = bme280_init(d);
    if (r != BME280_OK) return r;

    struct bme280_settings s;
    s.osr_h        = BME280_OVERSAMPLING_1X;
    s.osr_p        = BME280_OVERSAMPLING_1X;
    s.osr_t        = BME280_OVERSAMPLING_1X;
    s.filter       = BME280_FILTER_COEFF_OFF;
    s.standby_time = BME280_STANDBY_TIME_1000_MS;
    uint8_t sel = BME280_SEL_OSR_PRESS | BME280_SEL_OSR_TEMP |
                  BME280_SEL_OSR_HUM   | BME280_SEL_FILTER   |
                  BME280_SEL_STANDBY;
    bme280_set_sensor_settings(sel, &s, d);
    return bme280_set_sensor_mode(BME280_POWERMODE_NORMAL, d);
}

// TAREA DE SENSORES 
// Lee ambos BME280 a 1 Hz

static void sensor_task(void *arg) {
    (void)arg;
    struct bme280_dev dev_in, dev_out;
    uint8_t addr_in = BME_IN_ADDR, addr_out = BME_OUT_ADDR;

    bool init_in_ok  = (setup_bme(&dev_in,  &addr_in)  == BME280_OK);
    bool init_out_ok = (setup_bme(&dev_out, &addr_out) == BME280_OK);

    // Log solo de inicialización no datos continuos
    ESP_LOGI(TAG, "BME280 ADENTRO (0x%02X): %s", BME_IN_ADDR,
             init_in_ok ? "OK" : "ERR_SENSOR_DISCONNECT");
    ESP_LOGI(TAG, "BME280 AFUERA  (0x%02X): %s", BME_OUT_ADDR,
             init_out_ok ? "OK" : "ERR_SENSOR_DISCONNECT");

    int retry_cnt = 0;
    struct bme280_data d;

    while (1) {
        // Reconexión automática cada 5 ciclos 
        if (!init_in_ok && (retry_cnt % 5) == 0) {
            if (setup_bme(&dev_in, &addr_in) == BME280_OK) {
                init_in_ok = true;
                ESP_LOGI(TAG, "BME280 ADENTRO reconectado");
            }
        }
        if (!init_out_ok && (retry_cnt % 5) == 0) {
            if (setup_bme(&dev_out, &addr_out) == BME280_OK) {
                init_out_ok = true;
                ESP_LOGI(TAG, "BME280 AFUERA reconectado");
            }
        }
        retry_cnt++;

        // Leer sensor ADENTRO
        if (init_in_ok && bme280_get_sensor_data(BME280_ALL, &d, &dev_in) == BME280_OK) {
            float p = d.pressure / 100.0f;
            xSemaphoreTake(data_mtx, portMAX_DELAY);
            in_p = p; in_t = d.temperature; in_h = d.humidity;
            in_ok = (p >= 300.0f && p <= 1100.0f);
            xSemaphoreGive(data_mtx);
        } else {
            xSemaphoreTake(data_mtx, portMAX_DELAY);
            in_ok = false;
            init_in_ok = false;
            xSemaphoreGive(data_mtx);
            ESP_LOGE(TAG, "ERR_SENSOR_TIMEOUT — BME280 ADENTRO");
        }

        // Leer sensor AFUERA
        if (init_out_ok && bme280_get_sensor_data(BME280_ALL, &d, &dev_out) == BME280_OK) {
            float p = d.pressure / 100.0f;
            xSemaphoreTake(data_mtx, portMAX_DELAY);
            out_p = p; out_t = d.temperature; out_h = d.humidity;
            out_ok = (p >= 300.0f && p <= 1100.0f);
            xSemaphoreGive(data_mtx);
        } else {
            xSemaphoreTake(data_mtx, portMAX_DELAY);
            out_ok = false;
            init_out_ok = false;
            xSemaphoreGive(data_mtx);
            ESP_LOGE(TAG, "ERR_SENSOR_TIMEOUT — BME280 AFUERA");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// Actualiza el led RGB y el buzzer según las lecturas de presión y los rangos configurados. Refresca cada 200 ms.
static void control_task(void *arg) {
    (void)arg;
    while (1) {
        xSemaphoreTake(data_mtx, portMAX_DELAY);
        bool  oi = in_ok,  oo = out_ok;
        float pi = in_p,   po = out_p;
        float ti = in_t,   to = out_t;
        range_t ri = r_in, ro = r_out;
        xSemaphoreGive(data_mtx);

        // LED RGB según diferencia de presión
        if (!oi || !oo) {
            rgb_set(0, 0, 0);   
        } else {
            float diff = pi - po;
            if      (diff >  DIFF_HYST_HPA) rgb_set(1, 0, 0);  
            else if (diff < -DIFF_HYST_HPA) rgb_set(0, 0, 1);  
            else                            rgb_set(0, 1, 0);  
        }

        // Buzzer: activa si algún sensor está fuera del rango configurado
        bool out_of_range = false;
        if (oi) {
            out_of_range |= (pi < ri.p_min || pi > ri.p_max);
            out_of_range |= (ti < ri.t_min || ti > ri.t_max);
        }
        if (oo) {
            out_of_range |= (po < ro.p_min || po > ro.p_max);
            out_of_range |= (to < ro.t_min || to > ro.t_max);
        }

        if (out_of_range) {
    alarm_active = true;
    if (xTaskGetTickCount() >= buzzer_mute_until) {
        buzzer_on();
    } else {
        buzzer_off();
    }
} else {
    alarm_active = false;
    buzzer_off();
}

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

//Tarea del display
// Muestra la información de presión, temperatura y humedad de ambos sensores, así como la diferencia de presión. Refresca cada 500 ms.
static void display_task(void *arg) {
    (void)arg;
    char line[24];
    while (1) {
        xSemaphoreTake(data_mtx, portMAX_DELAY);
        bool  oi = in_ok,  oo = out_ok;
        float pi = in_p,   po = out_p;
        float ti = in_t,   to = out_t;
        float hi = in_h,   ho = out_h;
        xSemaphoreGive(data_mtx);

        sh1106_clear();
        sh1106_draw_str(0, 0, "BARO DIFERENCIAL");

        // Indicador de diferencia
        if (oi && oo) {
            float diff = pi - po;
            if      (diff >  DIFF_HYST_HPA) snprintf(line, sizeof(line), "IN>OUT %+.2f hPa", diff);
            else if (diff < -DIFF_HYST_HPA) snprintf(line, sizeof(line), "IN<OUT %+.2f hPa", diff);
            else                            snprintf(line, sizeof(line), "IN=OUT %+.2f hPa", diff);
        } else {
            snprintf(line, sizeof(line), "SENSOR ERROR");
        }
        sh1106_draw_str(0, 1, line);

        // Datos ADENTRO
        sh1106_draw_str(0, 3, "ADENTRO:");
        if (oi) {
            snprintf(line, sizeof(line), "%6.2fhPa", pi);
            sh1106_draw_str(54, 3, line);
            snprintf(line, sizeof(line), "T:%.1fC H:%.1f%%", ti, hi);
            sh1106_draw_str(0, 4, line);
        } else {
            sh1106_draw_str(54, 3, "--ERR--");
        }

        // Datos AFUERA
        sh1106_draw_str(0, 6, "AFUERA:");
        if (oo) {
            snprintf(line, sizeof(line), "%6.2fhPa", po);
            sh1106_draw_str(54, 6, line);
            snprintf(line, sizeof(line), "T:%.1fC H:%.1f%%", to, ho);
            sh1106_draw_str(0, 7, line);
        } else {
            sh1106_draw_str(54, 6, "--ERR--");
        }

        sh1106_update();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// Tarea de consola UART para configurar rangos y mostrar información. Lee comandos de la consola y actualiza los rangos o muestra la configuración actual. Comandos:
// - set in <Pmin> <Pmax> <Tmin> <Tmax>: Configura el rango para el sensor ADENTRO
// - set out <Pmin> <Pmax> <Tmin> <Tmax>: Configura el rango para el sensor AFUERA
static void print_help(void) {
    printf("\n=== Manometro Diferencial — Comandos ===\n");
    printf("  set in  <Pmin> <Pmax> <Tmin> <Tmax>   Rango ADENTRO\n");
    printf("  set out <Pmin> <Pmax> <Tmin> <Tmax>   Rango AFUERA\n");
    printf("  show                                  Muestra rangos actuales\n");
    printf("  mute                                  Silencia el buzzer\n");
    printf("  help                                  Muestra esta ayuda\n");
    printf("Ejemplo: set in 825 865 18 26\n");
    printf("=========================================\n\n");
}

static void console_task(void *arg) {
    (void)arg;
    char line[128];
    print_help();
    while (1) {
        printf("> ");
        fflush(stdout);
        if (fgets(line, sizeof(line), stdin) == NULL) {
            vTaskDelay(pdMS_TO_TICKS(50));
            continue;
        }
        size_t l = strlen(line);
        while (l > 0 && (line[l-1] == '\n' || line[l-1] == '\r')) line[--l] = 0;
        if (l == 0) continue;

        if (strncmp(line, "set in", 6) == 0) {
            float pmin, pmax, tmin, tmax;
            if (sscanf(line + 6, "%f %f %f %f", &pmin, &pmax, &tmin, &tmax) == 4
                && pmin < pmax && tmin < tmax) {
                xSemaphoreTake(data_mtx, portMAX_DELAY);
                r_in.p_min = pmin; r_in.p_max = pmax;
                r_in.t_min = tmin; r_in.t_max = tmax;
                xSemaphoreGive(data_mtx);
                printf("OK ADENTRO: P[%.2f, %.2f] hPa  T[%.2f, %.2f] C\n",
                       pmin, pmax, tmin, tmax);
            } else {
                printf("Uso: set in <Pmin> <Pmax> <Tmin> <Tmax>\n");
            }
        } else if (strncmp(line, "set out", 7) == 0) {
            float pmin, pmax, tmin, tmax;
            if (sscanf(line + 7, "%f %f %f %f", &pmin, &pmax, &tmin, &tmax) == 4
                && pmin < pmax && tmin < tmax) {
                xSemaphoreTake(data_mtx, portMAX_DELAY);
                r_out.p_min = pmin; r_out.p_max = pmax;
                r_out.t_min = tmin; r_out.t_max = tmax;
                xSemaphoreGive(data_mtx);
                printf("OK AFUERA: P[%.2f, %.2f] hPa  T[%.2f, %.2f] C\n",
                       pmin, pmax, tmin, tmax);
            } else {
                printf("Uso: set out <Pmin> <Pmax> <Tmin> <Tmax>\n");
            }
        } else if (strcmp(line, "show") == 0) {
            xSemaphoreTake(data_mtx, portMAX_DELAY);
            range_t a = r_in, b = r_out;
            xSemaphoreGive(data_mtx);
            printf("ADENTRO: P[%.2f, %.2f] hPa  T[%.2f, %.2f] C\n",
                   a.p_min, a.p_max, a.t_min, a.t_max);
            printf("AFUERA:  P[%.2f, %.2f] hPa  T[%.2f, %.2f] C\n",
                   b.p_min, b.p_max, b.t_min, b.t_max);
        } else if (strcmp(line, "mute") == 0) {
            buzzer_mute_until = xTaskGetTickCount() + pdMS_TO_TICKS(30000);
            buzzer_off();
            buzzer_off();
            printf("Buzzer silenciado.\n");
        } else if (strcmp(line, "help") == 0) {
            print_help();
        } else {
            printf("Comando desconocido. Escriba 'help'.\n");
        }
    }
}

static void console_uart_init(void) {
    setvbuf(stdin,  NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    esp_vfs_dev_uart_set_rx_line_endings(ESP_LINE_ENDINGS_CR);
    esp_vfs_dev_uart_set_tx_line_endings(ESP_LINE_ENDINGS_CRLF);
    esp_vfs_dev_uart_use_driver(UART_CONSOLE_NUM);
    const uart_config_t cfg = {
        .baud_rate  = UART_BAUD,
        .data_bits  = UART_DATA_8_BITS,
        .parity     = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    uart_driver_install(UART_CONSOLE_NUM, 256, 0, 0, NULL, 0);
    uart_param_config(UART_CONSOLE_NUM, &cfg);
    uart_set_line_inverse(UART_CONSOLE_NUM, UART_SIGNAL_INV_DISABLE);
}

// Main
void app_main(void) {
    ESP_LOGI(TAG, "=== MANOMETRO DIFERENCIAL HOSPITALARIO v2.0 ===");
    ESP_LOGI(TAG, "=== AUTODIAGNOSTICO INICIANDO ===");

    i2c_mtx   = xSemaphoreCreateMutex();
    data_mtx  = xSemaphoreCreateMutex();
    btn_evt_q = xQueueCreate(8, sizeof(uint32_t));

    // Inicializar I2C 
    i2c_config_t conf = {
        .mode             = I2C_MODE_MASTER,
        .sda_io_num       = I2C_SDA_GPIO,
        .scl_io_num       = I2C_SCL_GPIO,
        .sda_pullup_en    = GPIO_PULLUP_ENABLE,
        .scl_pullup_en    = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_FREQ_HZ,
    };
    i2c_param_config(I2C_PORT, &conf);
    i2c_driver_install(I2C_PORT, I2C_MODE_MASTER, 0, 0, 0);
    ESP_LOGI(TAG, "I2C OK (SDA=%d SCL=%d %dHz)", I2C_SDA_GPIO, I2C_SCL_GPIO, I2C_FREQ_HZ);

    // Periféricos 
    rgb_init();    rgb_set(0, 0, 0);
    buzzer_init(); buzzer_off();
    button_init();

    // OLED
    sh1106_init();
    sh1106_clear();
    sh1106_draw_str(0, 0, "BARO DIFERENCIAL");
    sh1106_draw_str(0, 2, "Iniciando...");
    sh1106_update();

    // Consola UART
    console_uart_init();

    ESP_LOGI(TAG, "=== AUTODIAGNOSTICO COMPLETO ===");

    // Tareas FreeRTOS
    xTaskCreate(sensor_task,  "sensor",  4096, NULL, 5, NULL);
    xTaskCreate(display_task, "display", 4096, NULL, 4, NULL);
    xTaskCreate(control_task, "control", 4096, NULL, 5, NULL);
    xTaskCreate(button_task,  "button",  2048, NULL, 6, NULL);
    xTaskCreate(console_task, "console", 4096, NULL, 3, NULL);

    ESP_LOGI(TAG, "Sistema listo. GPIO%d = silenciar buzzer", BUTTON_GPIO);
}