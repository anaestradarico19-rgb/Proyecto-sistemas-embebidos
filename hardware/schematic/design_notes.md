# Notas de Diseño Electrónico

## Arquitectura General

El sistema está basado en un ESP32 como unidad principal de procesamiento.

El ESP32 se comunica mediante I2C con:

- OLED SH1106

## Sensor Principal

Se utiliza un BME280 para la adquisición de presión atmosférica en hPa.

## Interfaz Visual

La visualización del sistema se realiza mediante una pantalla OLED SSD1306.

## Sistema de Alertas

El sistema utiliza:

- LED RGB para indicación visual,
- buzzer para alarma sonora.

## Manejo del Buzzer

El buzzer se controla mediante transistor 2N2222 para proteger los GPIO del ESP32 de sobrecorriente.

## Comunicación

Protocolos implementados:
- I2C
- UART

## Consideraciones de Diseño

- Todos los dispositivos comparten tierra común.
- El sistema opera principalmente a 3.3V.
- Se priorizó modularidad y facilidad de integración.
