# ESP32 Pinout — Manómetro Ambiental

| Componente | Pin del Componente | Pin ESP32 | Protocolo / Función | Voltaje |
|---|---|---|---|---|
| BME280 | SDA | GPIO21 | I2C Data | 3.3V |
| BME280 | SCL | GPIO22 | I2C Clock | 3.3V |
| BME280 | VCC | 3.3V | Alimentación | 3.3V |
| BME280 | GND | GND | Tierra común | 0V |
| OLED SSD1306 | SDA | GPIO21 | I2C Data | 3.3V |
| OLED SSD1306 | SCL | GPIO22 | I2C Clock | 3.3V |
| OLED SSD1306 | VCC | 3.3V | Alimentación | 3.3V |
| OLED SSD1306 | GND | GND | Tierra común | 0V |
| LED RGB | Red | GPIO25 | Canal Rojo | 3.3V |
| LED RGB | Green | GPIO26 | Canal Verde | 3.3V |
| LED RGB | Blue | GPIO27 | Canal Azul | 3.3V |
| LED RGB | GND | GND | Tierra común | 0V |
| Botón Push Button | Terminal 1 | GPIO4 | Entrada digital | 3.3V |
| Botón Push Button | Terminal 2 | GND | Tierra | 0V |
| Buzzer  | Signal | GPIO18 | PWM / Alarma | 3.3V |
| Buzzer  | GND | GND | Tierra común | 0V |
| LED Indicador | Ánodo | GPIO5 | Indicador estado | 3.3V |
| LED Indicador | Cátodo | GND | Tierra | 0V |
| ESP32 | VIN | Fuente externa | Alimentación principal | 5V (omputador)|
| ESP32 | GND | GND | Tierra común | 0V |

# Configuración de Comunicaciones

## I2C
- SDA → GPIO21
- SCL → GPIO22
- Frecuencia → 100 kHz
- Dirección BME280 
- Dirección OLED SSD1306 → 0x3C

# Funcionalidad de Hardware

## BME280
Sensor principal para medición de presión atmosférica y temperatura.

## OLED SSD1306
Visualización de:
- presión en hPa
- Temperatura

## LED RGB
Indicador visual de estados:
- Verde → Presion positiva.
- Azul → Presion neutra.
- Rojo → Presion negativa.

## Botón
Permite:
- Apaga alarma mientras se estabiliza el sistema de nuevo.

## Buzzer
Alarma sonora cuando la presión o temperatura excede umbrales configurados.


# Notas de Integración

- Todos los módulos comparten tierra común (GND).
- BMP280 y OLED comparten el mismo bus I2C.
- El ESP32 trabaja a lógica de 3.3V.
- Se usaron resistencia de 330Ω para cada canal del LED RGB.
- Se habilita el pull-up interno del botón.
