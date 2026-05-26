# Justificación del Diseño Electrónico

## Arquitectura General

El sistema fue diseñado alrededor de un ESP32 debido a su bajo consumo energético, conectividad integrada y compatibilidad con comunicación I2C a 3.3 V.

El circuito integra:

- Dos sensores BME280 para medición de temperatura, presión y humedad.
- Una pantalla OLED SSD1306 para visualización de datos.
- Un LED RGB como sistema de indicación visual.
- Componentes de protección y estabilización para garantizar un funcionamiento confiable.

Todos los dispositivos operan a 3.3 V, lo que simplifica el diseño y evita el uso de conversores de nivel lógico.


# Justificación de Componentes

## Resistencias de 330 Ω

Las resistencias de 330 Ω fueron utilizadas para limitar la corriente del LED RGB y proteger tanto el LED como los pines GPIO del ESP32.

La corriente se determina mediante la ley de Ohm:

I = (VCC - Vf) / R

Considerando:

- VCC = 3.3 V
- Vf ≈ 2 V
- R = 330 Ω

La corriente aproximada es de 4 mA, valor seguro para el ESP32 y suficiente para obtener una iluminación visible sin sobrecargar el sistema.


## Resistencias de 1 kΩ

Las resistencias de 1 kΩ se utilizan como protección y limitación de corriente en señales de control, especialmente hacia la base del transistor.

Este valor permite:

- Activar correctamente el transistor.
- Limitar la corriente entregada por el GPIO del ESP32.
- Evitar consumo excesivo de corriente.
- Proteger el microcontrolador frente a sobrecorrientes.


## Uso del Transistor

El transistor fue implementado como etapa de conmutación para evitar que el ESP32 alimente directamente cargas que puedan demandar mayor corriente.

Su utilización permite:

- Proteger los pines GPIO del ESP32.
- Manejar corrientes superiores a las soportadas directamente por el microcontrolador.
- Mejorar la confiabilidad del sistema.
- Aislar eléctricamente las cargas controladas.

El transistor funciona como un interruptor electrónico controlado mediante señales digitales provenientes del ESP32.


## Capacitor de 1 nF

El capacitor de 1 nF se incorporó para reducir ruido eléctrico y estabilizar la alimentación de los sensores.

Sus principales funciones son:

- Filtrar perturbaciones de alta frecuencia.
- Reducir ruido en las señales.
- Mejorar la estabilidad de las mediciones del BME280.
- Mantener una alimentación más limpia para el sistema.


# Justificación del Uso de I2C

Se eligió el protocolo I2C debido a sus ventajas en sistemas embebidos:

- Permite conectar múltiples dispositivos utilizando únicamente dos líneas de comunicación.
- Reduce significativamente el cableado.
- Optimiza el uso de GPIO del ESP32.
- Facilita futuras expansiones del sistema.

Gracias a esto, los sensores BME280 y la pantalla OLED pueden compartir el mismo bus de comunicación.


# Conclusión del Diseño

El diseño electrónico fue desarrollado buscando:

- Bajo consumo energético.
- Compatibilidad completa a 3.3 V.
- Protección del microcontrolador.
- Estabilidad en la comunicación I2C.
- Facilidad de integración y expansión.
