# Descripción de Pinout

## Comunicación I2C

| Señal | GPIO ESP32 | Dispositivos |
|---|---|---|
| SDA | GPIO21 | OLED SSD1306 |
| SCL | GPIO22 | OLED SSD1306 |

Frecuencia I2C configurada:
- 400 kHz

Bus I2C utilizado:
- I2C_NUM_0

## OLED SSD1306

| Pin OLED | Conexión |
|---|---|
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO21 |
| SCL | GPIO22 |

Dirección I2C:
- 0x3C

Resolución:
- 128x64

## Sensor BMP280

| Pin BMP280 | Conexión |
|---|---|
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO21 |
| SCL | GPIO22 |

Estado actual:
- Integración física inicial realizada.
- Lectura del sensor aún en desarrollo.

## LED RGB

| Canal | GPIO |
|---|---|
| Red | GPIO26 |
| Green | GPIO27 |
| Blue | GPIO14 |

Modo de control:
- PWM mediante LEDC.

Frecuencia PWM:
- 5 kHz

Resolución PWM:
- 8 bits

## Buzzer

| Señal | GPIO |
|---|---|
| Control | GPIO25 |

Modo:
- Salida digital.

Función:
- Alarma sonora del sistema.

## Botón

| Señal | GPIO |
|---|---|
| Entrada digital | GPIO0 |

Configuración:
- Activo bajo.
- Pull-up interno habilitado.

Función:
- Silenciar buzzer durante 15 segundos.

## UART

| Señal | GPIO |
|---|---|
| TX | GPIO1 |
| RX | GPIO3 |

Configuración UART:
- 115200 bps

Funciones:
- Logging.
- Comandos seriales.
- Visualización de datos.

## Distribución General de GPIO

| GPIO | Función |
|---|---|
| GPIO21 | SDA I2C |
| GPIO22 | SCL I2C |
| GPIO25 | Buzzer |
| GPIO26 | LED RGB Red |
| GPIO27 | LED RGB Green |
| GPIO14 | LED RGB Blue |
| GPIO0 | Botón |
| GPIO1 | UART TX |
| GPIO3 | UART RX |
