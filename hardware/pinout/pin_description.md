# Descripción de Pinout

## Comunicación I2C

| Señal | GPIO ESP32 | Dispositivos |
|---|---|---|
| SDA | GPIO21 | OLED SH1106 |
| SCL | GPIO22 | OLED SH1106 |

Frecuencia I2C configurada:
- 400 kHz

Bus I2C utilizado:
- I2C_NUM_0

## OLED SH1106

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

## Sensor BME280

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
| Red | GPIO25 |
| Green | GPIO26 |
| Blue | GPIO127 |

Modo de control:
- PWM mediante LEDC.

Frecuencia PWM:
- 5 kHz

Resolución PWM:
- 8 bits

## Buzzer

| Señal | GPIO |
|---|---|
| Control | GPIO18 |

Modo:
- Salida digital.

Función:
- Alarma sonora del sistema.

## Botón

| Señal | GPIO |
|---|---|
| Entrada digital | GPIO19 |

Configuración:
- Activo bajo.
- Pull-up interno habilitado.

Función:
- Silenciar buzzer durante 30 segundos y reiniciar el sistema.

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
