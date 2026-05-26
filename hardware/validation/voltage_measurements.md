# Validación Eléctrica

## Validación de Alimentación

Se verificó la correcta alimentación del ESP32 y los periféricos usando una fuente externa de 5V (Computador).

## Validación del Sensor BME280

El sensor BME280 fue detectado correctamente mediante comunicación I2C.

Las lecturas de presión permanecieron estables durante las pruebas iniciales, y cambiaron cuando se esperaba en cuanto a nuestra forma de prueba.

## Validación de Comunicación I2C

Se verificó:

- transmisión correcta de datos,
- inicialización del sensor,
- ausencia de errores de comunicación.

## Validación del Buzzer

El buzzer responde correctamente a señales de activación generadas por el ESP32.


## Validación del LED RGB

Los canales RGB responden correctamente mediante control GPIO de acuerdo a las medidas acordadas mediante el firmware.

## Resultado General

El hardware opera correctamente bajo condiciones normales de prueba.
