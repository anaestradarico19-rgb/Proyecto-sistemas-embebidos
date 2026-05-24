// Test para verificar el sensor

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME280.h>

//Configuración del sensor
#define BME280_I2C_ADDR   0x76   // Dirección I2C 
#define SAMPLE_RATE_MS    1000   // Lectura cada 1 segundo (1 Hz)
#define PRESSURE_MIN_HPA  300.0f
#define PRESSURE_MAX_HPA  1100.0f

Adafruit_BME280 bme;


void log_info(const char *msg) {
    Serial.printf("[%08lums] [INFO ] %s\n", millis(), msg);
}


void log_warn(const char *msg) {
    Serial.printf("[%08lums] [WARN ] %s\n", millis(), msg);
}


void log_error(const char *code, const char *detail) {
    Serial.printf("[%08lums] [ERROR] %s — %s\n", millis(), code, detail);
}


void setup() {
    // Inicializar UART a 115200 bps, 8N1
    Serial.begin(115200);
    delay(1000);

    Serial.println("================================================");
    Serial.println("  Manometro Ambiental — Test BME280");
    Serial.println("================================================");

    log_info("Sistema iniciando...");
    log_info("Inicializando bus I2C (SDA=21, SCL=22, 400kHz)...");

    // Inicializar I2C con los pines correctos del ESP32
    Wire.begin(21, 22);

    // Intentar conectar con el BME280
    log_info("Buscando BME280 en direccion 0x76...");

    if (!bme.begin(BME280_I2C_ADDR, &Wire)) {
        log_error("ERR_SENSOR_DISCONNECT", "BME280 no detectado en 0x76");
        log_warn("Verificar conexiones: VCC=3.3V, GND, SDA=GPIO21, SCL=GPIO22");
        log_warn("Intentando con direccion alternativa 0x77...");

        // Algunos módulos usan 0x77 (SDO a VCC)
        if (!bme.begin(0x77, &Wire)) {
            log_error("ERR_SENSOR_DISCONNECT", "BME280 tampoco encontrado en 0x77");
            Serial.println("\n[SISTEMA DETENIDO] Revisar hardware y reiniciar.\n");
            while (1) delay(1000);  // Detener sistema
        } else {
            log_info("BME280 encontrado en direccion 0x77 (SDO conectado a VCC)");
        }
    } else {
        log_info("BME280 inicializado correctamente en 0x76");
    }

    // Configurar el sensor
    bme.setSampling(
        Adafruit_BME280::MODE_NORMAL,
        Adafruit_BME280::SAMPLING_X1,   // Temperatura oversampling x1
        Adafruit_BME280::SAMPLING_X1,   // Presión oversampling x1
        Adafruit_BME280::SAMPLING_X1,   // Humedad oversampling x1
        Adafruit_BME280::FILTER_OFF,
        Adafruit_BME280::STANDBY_MS_1000
    );

    log_info("=== AUTODIAGNOSTICO COMPLETO — Iniciando lectura ===");
    Serial.println("------------------------------------------------");
}


void loop() {
    // Leer datos del sensor
    float pressure    = bme.readPressure() / 100.0F;  // Convertir Pa → hPa
    float temperature = bme.readTemperature();
    float humidity    = bme.readHumidity();

    // Validar que el sensor sigue respondiendo
    if (isnan(pressure) || isnan(temperature) || isnan(humidity)) {
        log_error("ERR_SENSOR_TIMEOUT", "Sin respuesta del BME280");
        delay(SAMPLE_RATE_MS);
        return;
    }

    // Validar rango de presión (RF-02)
    if (pressure < PRESSURE_MIN_HPA || pressure > PRESSURE_MAX_HPA) {
        char msg[64];
        snprintf(msg, sizeof(msg), "Presion fuera de rango: %.2f hPa", pressure);
        log_error("ERR_SENSOR_OUT_OF_RANGE", msg);
        delay(SAMPLE_RATE_MS);
        return;
    }

    // Imprimir lectura exitosa
    char msg[96];
    snprintf(msg, sizeof(msg),
             "P=%.2f hPa | T=%.2f C | H=%.2f %%",
             pressure, temperature, humidity);
    log_info(msg);

    // Lógica de umbrales para el actuador
    if (pressure < 950.0f) {
        log_warn("ALERTA: Presion baja detectada");
    } else if (pressure > 1050.0f) {
        log_warn("ALERTA: Presion alta detectada");
    }

    delay(SAMPLE_RATE_MS);
}
