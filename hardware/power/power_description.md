# Sistema de Alimentación

## Descripción General

El sistema embebido se alimenta mediante una fuente externa de 5V (Computador) conectada al ESP32.

El ESP32 regula internamente el voltaje a 3.3V para alimentar los dispositivos de lógica digital y sensores.

## Componentes Alimentados

Los siguientes componentes operan a 3.3V:

- BME280
- OLED SSD1306
- LED RGB
- Buzzer y demás.

## Tierra Común

Todos los módulos comparten una conexión GND común para garantizar estabilidad eléctrica y correcta referencia de señal.

## Consideraciones Eléctricas

- El BME280 opera únicamente a 3.3V.
- El sistema evita alimentar sensores con 5V para prevenir daños.
- El buzzer es controlado mediante transistor para evitar sobrecorriente en los GPIO del ESP32.

## Estado Actual

La alimentación del sistema es estable durante operación continua. Usado aproximadamente por 30 minutos - 1 hora que sería el tiempo suficiente para realizar todas las pruebas de requerimientos.
