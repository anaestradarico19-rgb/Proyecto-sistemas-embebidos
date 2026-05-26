# Estrategia de Logging

## Descripción General

El sistema implementa un mecanismo de logging mediante comunicación UART para facilitar el monitoreo, depuración y validación del comportamiento del firmware en tiempo real.

Los logs permiten visualizar eventos relevantes del sistema, estados operativos, errores detectados y lecturas obtenidas de los sensores.

---

## Niveles de Logging

El sistema utiliza tres niveles principales de logging:

| Nivel | Descripción |
|---|---|
| INFO | Información normal del sistema |
| WARN | Advertencias o condiciones cercanas a falla |
| ERROR | Errores críticos detectados |

---

## Ejemplos de Logs

```text
[INFO] Presión: 845.91 hPa
[INFO] Temperatura: 28.31 C
[WARN] Presión cercana al umbral
[ERROR] Sensor desconectado
```

---

## Información Registrada

Los mensajes de logging incluyen:

- Lecturas de presión
- Temperatura y humedad
- Estado del sistema
- Activación de alarmas
- Errores de comunicación I2C
- Resultados de autodiagnóstico

---

## Comunicación UART

El sistema utiliza UART a 115200 bps para transmitir los logs hacia el monitor serial.

### Configuración UART

| Parámetro | Valor |
|---|---|
| Baudrate | 115200 |
| Data bits | 8 |
| Stop bits | 1 |
| Paridad | Ninguna |

---

## Objetivos del Sistema de Logging

- Facilitar la depuración del firmware
- Validar el funcionamiento del sistema
- Detectar errores de sensores
- Generar evidencia para testing
- Monitorear el comportamiento en tiempo real

---

## Integración con el Sistema

El sistema de logging trabaja de forma integrada con:

- Lectura de sensores BME280
- Comunicación I2C
- Actualización de la pantalla OLED
- Activación de alarmas mediante LEDs y buzzer
- Rutina de autodiagnóstico

---

## Manejo de Errores

Los eventos críticos son reportados mediante logs de nivel ERROR.

### Ejemplos

```text
[ERROR] Sensor no detectado
[ERROR] Timeout en comunicación I2C
[ERROR] Lectura inválida de presión
```

Las advertencias del sistema son reportadas mediante logs WARN cuando una variable se aproxima a condiciones críticas.

---

## Evidencia de Funcionamiento

La validación del sistema de logging se realizó mediante capturas UART obtenidas durante la ejecución del sistema embebido, evidenciando:

- Generación correcta de mensajes INFO/WARN/ERROR
- Monitoreo continuo de sensores
- Funcionamiento del autodiagnóstico
- Registro de eventos del sistema
