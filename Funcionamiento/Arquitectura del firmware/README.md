# Arquitectura de Firmware

El firmware del manómetro diferencial hospitalario fue desarrollado sobre ESP-IDF utilizando FreeRTOS.  
La arquitectura está organizada en tareas concurrentes encargadas de:

- Lectura de sensores
- Control de alarmas
- Actualización del display
- Comunicación UART
- Manejo del botón de silencio

La comunicación entre tareas se realiza mediante mutex y colas de FreeRTOS.

---

# Organización General

```text
                    ┌─────────────────────┐
                    │     sensor_task     │
                    │ Lectura BME280      │
                    └─────────┬───────────┘
                              │
                              │ Variables globales
                              ▼
                 ┌─────────────────────────┐
                 │ Variables compartidas   │
                 │ Presión, temp, humedad  │
                 └─────────┬───────────────┘
                           │
          ┌────────────────┴────────────────┐
          ▼                                 ▼
┌─────────────────────┐          ┌─────────────────────┐
│    control_task     │          │    display_task     │
│ LED RGB + buzzer    │          │ Pantalla OLED       │
└─────────┬───────────┘          └─────────────────────┘
          │
          ▼
┌─────────────────────┐
│ button_task         │
│ Silenciar buzzer    │
└─────────────────────┘

UART Console ───────► console_task
```

---

# Tareas FreeRTOS

## 1. sensor_task

### Función

Se encarga de:

- Inicializar los sensores BME280
- Leer presión, temperatura y humedad
- Validar datos
- Detectar desconexiones
- Reconectar sensores automáticamente

### Frecuencia

```c
vTaskDelay(pdMS_TO_TICKS(1000));
```

Actualización cada 1 segundo.

### Variables actualizadas

```c
in_p, in_t, in_h
out_p, out_t, out_h
in_ok
out_ok
```

### Sincronización

Protege las variables compartidas usando:

```c
xSemaphoreTake(data_mtx, portMAX_DELAY);
xSemaphoreGive(data_mtx);
```

También protege el bus I2C usando:

```c
i2c_mtx
```

---

## 2. control_task

### Función

Realiza:

- Cálculo de diferencia de presión
- Control del LED RGB
- Verificación de rangos
- Activación del buzzer

### Frecuencia

```c
vTaskDelay(pdMS_TO_TICKS(200));
```

Actualización cada 200 ms.

### Lógica del LED RGB

```text
Rojo  -> Presión interior > exterior
Azul  -> Presión exterior > interior
Verde -> Presiones iguales
OFF   -> Error de sensor
```

### Lógica de alarma

El buzzer se activa cuando:

```text
- Presión fuera de rango
- Temperatura fuera de rango
```

### Comunicación

Lee datos compartidos protegidos con:

```c
data_mtx
```

---

## 3. display_task

### Función

Actualiza la pantalla OLED mostrando:

- Presión interior
- Presión exterior
- Temperatura
- Humedad
- Diferencia de presión
- Estado de error

### Frecuencia

```c
vTaskDelay(pdMS_TO_TICKS(500));
```

Actualización cada 500 ms.

### Comunicación

Accede a datos compartidos usando:

```c
data_mtx
```

### Driver utilizado

```c
sh1106_init()
sh1106_update()
sh1106_draw_str()
```

---

## 4. button_task

### Función

Gestiona el botón físico para silenciar el buzzer.

### Funcionamiento

```text
Interrupción GPIO
        ↓
 ISR envía evento a cola
        ↓
 button_task procesa evento
        ↓
 Silencia buzzer por 30 segundos
```

### Comunicación ISR → tarea

Se utiliza la cola:

```c
btn_evt_q
```

Envío desde ISR:

```c
xQueueSendFromISR()
```

Recepción en tarea:

```c
xQueueReceive()
```

### Debounce

Se implementa debounce por software:

```c
vTaskDelay(pdMS_TO_TICKS(40));
```

---

## 5. console_task

### Función

Permite configurar el sistema mediante UART.

### Comandos disponibles

```text
set in  <Pmin> <Pmax> <Tmin> <Tmax>
set out <Pmin> <Pmax> <Tmin> <Tmax>
show
mute
help
```

### Ejemplo

```text
set in 825 865 18 26
```

### Comunicación

Modifica:

```c
r_in
r_out
```

Protegidos mediante:

```c
data_mtx
```

---

# Comunicación entre tareas

## Mutex

### i2c_mtx

Protege el bus I2C compartido entre:

- Sensores BME280
- Pantalla OLED

Uso:

```c
xSemaphoreTake(i2c_mtx, portMAX_DELAY);
xSemaphoreGive(i2c_mtx);
```

---

### data_mtx

Protege variables globales compartidas:

```c
in_p, in_t, in_h
out_p, out_t, out_h
r_in, r_out
in_ok, out_ok
```

Evita corrupción de datos por acceso concurrente.

---

# Colas FreeRTOS

## btn_evt_q

Cola utilizada para comunicar:

```text
ISR del botón → button_task
```

Uso:

```c
xQueueSendFromISR()
xQueueReceive()
```

Esto evita ejecutar lógica compleja dentro de la interrupción.

---

# Drivers Implementados

## Driver I2C

Maneja la comunicación con:

```text
- BME280 interior
- BME280 exterior
- OLED SH1106
```

Protegido mediante:

```c
i2c_mtx
```

---

## Driver OLED SH1106

Funciones principales:

```c
sh1106_init()
sh1106_update()
sh1106_draw_char()
sh1106_draw_str()
```

Implementa:

- Framebuffer
- Escritura de texto
- Actualización de pantalla

---

## Driver RGB

Control del LED RGB:

```c
rgb_set(r, g, b)
```

---

## Driver Buzzer

PWM generado usando LEDC del ESP32:

```c
buzzer_on()
buzzer_off()
```

Frecuencia:

```text
2 kHz
```

---

# Flujo General del Sistema

```text
BME280
   ↓
sensor_task
   ↓
Variables globales protegidas
   ↓
┌───────────────┬───────────────┐
↓                               ↓
control_task               display_task
↓                               ↓
LED RGB + buzzer           OLED SH1106
```

Flujos adicionales:

```text
Botón → ISR → Cola → button_task
```

```text
UART → console_task → actualización de rangos
```

---

# Características de Tiempo Real

```text
✔ Multitarea concurrente
✔ Sincronización con mutex
✔ Comunicación ISR-tarea con colas
✔ Protección del bus I2C
✔ Reconexión automática de sensores
✔ Debounce por software
✔ Alarmas no bloqueantes
✔ Actualización periódica independiente
```

---

# Prioridades de Tareas

| Tarea | Prioridad |
|---|---|
| button_task | 6 |
| sensor_task | 5 |
| control_task | 5 |
| display_task | 4 |
| console_task | 3 |

Esto garantiza:

```text
- Respuesta rápida del botón
- Lectura estable de sensores
- Control prioritario sobre interfaz gráfica
- Comunicación UART en segundo plano
```
