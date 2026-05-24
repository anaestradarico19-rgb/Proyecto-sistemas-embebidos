# Diagnóstico Inicial BMP280

## Estado actual
El sensor BMP280 no está generando lecturas válidas durante las pruebas de integración con ESP32.

## Problemas observados
- Fallos de detección I2C
- Inicialización inconsistente
- Lecturas inválidas de presión

## Validaciones realizadas
- Verificación de alimentación 3.3V
- Validación de cableado SDA/SCL
- Escaneo I2C
- Verificación de direcciones 0x76 y 0x77

## Estado
En validación.

## Acción correctiva propuesta
Evaluar reemplazo mediante sensor BME280 compatible con la arquitectura actual.
