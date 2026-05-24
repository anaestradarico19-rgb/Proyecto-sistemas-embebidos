# Documento de Requisitos y Plan de Testing
## Manómetro Ambiental con ESP32
## Tomás Bedoya, Ana María Estrada, Juan Manuel Gómez y Manuela Ramírez


## 1. Requisitos Funcionales

Los requisitos funcionales describen el comportamiento mínimo que debe cumplir el sistema para medir, mostrar y usar la presión atmosférica como base del manómetro.

| Req ID | Descripción | Criterio de Aceptación |
|--------|-------------|------------------------|
| RF-01 | El sistema debe adquirir la presión atmosférica ambiental mediante un sensor físico (BMP280 o equivalente por I2C). | El sensor entrega lecturas estables de presión al ESP32. |
| RF-02 | La presión medida debe expresarse en unidades de hPa. | Las lecturas visibles en el sistema aparecen en hPa. |
| RF-03 | El sistema debe mostrar la lectura de presión en tiempo real por monitor serial. | Se observan valores actualizados de presión en el monitor serial. |
| RF-04 | El sistema debe realizar lecturas periódicas del sensor con una frecuencia mínima de 1 Hz. | Se registra al menos una lectura por segundo durante la operación normal. |
| RF-05 | El firmware debe validar que la lectura de presión esté dentro de un rango operativo razonable para presión atmosférica. | Las lecturas fuera de un rango lógico se identifican como error o advertencia. |
| RF-06 | El sistema debe controlar un actuador sencillo, como LED o buzzer, para indicar un estado básico de alarma o advertencia. | El actuador cambia de estado cuando la presión supera o baja de un umbral definido. |
| RF-07 | El sistema debe usar comunicación I2C para interactuar con el sensor. | La comunicación con el sensor se realiza correctamente por I2C. |
| RF-08 | El sistema debe enviar la información principal por UART para su visualización y verificación. | Los datos se leen correctamente en el monitor serial configurado. |

---

## 2. Requisitos No Funcionales

Los requisitos no funcionales definen restricciones básicas de desempeño, estabilidad y presentación del sistema, sin introducir exigencias innecesarias para el alcance del proyecto.

| Req ID | Categoría | Descripción | Criterio de Aceptación |
|--------|-----------|-------------|------------------------|
| RNF-01 | Rendimiento | El tiempo entre la lectura del sensor y su visualización no debe ser excesivo. | La actualización se observa en menos de 2 segundos en operación normal. |
| RNF-02 | Confiabilidad | El sistema debe funcionar de forma continua durante una sesión de prueba corta. | Opera al menos 30 minutos sin reinicios ni pérdidas evidentes de datos. |
| RNF-03 | Implementación física | El sistema debe estar ensamblado en una montura o protoboard solo para pruebas y presentado de forma ordenada. | La demostración muestra un montaje limpio y funcional. |
| RNF-04 | Documentación | El código fuente debe estar comentado de forma básica y organizado por archivos. | La revisión del proyecto permite identificar claramente las funciones principales. |

---

## 3. Plan de Testing

### 3.1 Estrategia General

La verificación del sistema se realizará mediante pruebas básicas de módulo, integración y demostración final sobre el hardware ensamblado.

- **Unit Testing:** verificación de lectura del sensor y formateo de datos.
- **Integration Testing:** verificación de comunicación I2C y salida UART.
- **System Testing:** verificación del funcionamiento completo con el ESP32 y el actuador.

Cada requisito debe tener al menos un caso de prueba asociado con evidencia simple, como captura de pantalla del monitor serial, foto del montaje o video corto de la demostración.

---

### 3.2 Matriz de Trazabilidad de Requisitos (SRTM)

| Req ID | Descripción | Test Case ID | Tipo de Prueba | Artefacto de Evidencia |
|--------|-------------|-------------|----------------|------------------------|
| RF-01 | Adquisición de presión por sensor | TC-001 | Integration | Captura monitor serial |
| RF-02 | Unidades de presión en hPa | TC-002 | Integration | Captura monitor serial |
| RF-03 | Visualización en tiempo real | TC-003 | System | Captura monitor serial |
| RF-04 | Lecturas periódicas del sensor | TC-004 | Integration | Captura monitor serial |
| RF-05 | Validación de rango | TC-005 | Integration | Captura monitor serial |
| RF-06 | Control básico de actuador | TC-006 | System | Video o foto de la demostración |
| RF-07 | Comunicación I2C | TC-007 | Integration | Captura monitor serial |
| RF-08 | Salida UART | TC-008 | Integration | Captura monitor serial |
| RNF-01 | Tiempo de visualización | TC-009 | System | Registro cronometrado |
| RNF-02 | Estabilidad de operación | TC-010 | System | Log o video de sesión |
| RNF-03 | Montaje físico | TC-011 | Inspection | Fotografías del dispositivo |
| RNF-04 | Documentación básica | TC-012 | Inspection | Repositorio del proyecto |

---

### 3.3 Casos de Prueba

#### TC-001 — Adquisición de presión por sensor

| Campo | Detalle |
|-------|---------|
| **Req. Trazabilidad** | RF-01 |
| **Objetivo** | Verificar que el sensor entrega lecturas válidas al ESP32. |
| **Precondiciones** | Hardware ensamblado. Sensor conectado por I2C. Firmware cargado. Monitor serial abierto a 115200 bps. |
| **Resultado Esperado** | El monitor serial muestra lecturas de presión continuas durante la operación normal. |
| **Tipo** | Integration |
| **Evidencia** | Captura de pantalla del monitor serial. |

#### TC-002 — Unidades de presión en hPa

| Campo | Detalle |
|-------|---------|
| **Req. Trazabilidad** | RF-02 |
| **Objetivo** | Verificar que las lecturas se presentan en hPa. |
| **Precondiciones** | TC-001 exitoso. |
| **Resultado Esperado** | Las lecturas visibles aparecen en hPa sin unidades mezcladas. |
| **Tipo** | Integration |
| **Evidencia** | Captura de pantalla del monitor serial. |

#### TC-003 — Visualización en tiempo real

| Campo | Detalle |
|-------|---------|
| **Req. Trazabilidad** | RF-03 |
| **Objetivo** | Verificar que los datos se actualizan en tiempo real en el monitor serial. |
| **Precondiciones** | Sistema operando normalmente. |
| **Resultado Esperado** | Cada nueva lectura se observa en secuencia sin congelamientos. |
| **Tipo** | System |
| **Evidencia** | Captura del monitor serial. |

#### TC-004 — Lecturas periódicas del sensor

| Campo | Detalle |
|-------|---------|
| **Req. Trazabilidad** | RF-04 |
| **Objetivo** | Verificar que el sistema toma lecturas al menos una vez por segundo. |
| **Precondiciones** | Sistema operativo y monitor serial activo. |
| **Resultado Esperado** | Se observan lecturas periódicas con una frecuencia mínima de 1 Hz. |
| **Tipo** | Integration |
| **Evidencia** | Captura del monitor serial. |

#### TC-005 — Validación de rango

| Campo | Detalle |
|-------|---------|
| **Req. Trazabilidad** | RF-05 |
| **Objetivo** | Verificar que el sistema identifique lecturas fuera de un rango lógico. |
| **Precondiciones** | Sistema operando normalmente. |
| **Resultado Esperado** | Lecturas anómalas se reportan como advertencia o error. |
| **Tipo** | Integration |
| **Evidencia** | Captura del log o monitor serial. |

#### TC-006 — Control básico de actuador

| Campo | Detalle |
|-------|---------|
| **Req. Trazabilidad** | RF-06 |
| **Objetivo** | Verificar que el LED o buzzer cambie de estado ante un umbral definido. |
| **Precondiciones** | Umbrales configurados. Sistema operando. |
| **Resultado Esperado** | El actuador responde al cambio de presión o simulación del umbral. |
| **Tipo** | System |
| **Evidencia** | Video corto o fotografía de la demostración. |

#### TC-007 — Comunicación I2C

| Campo | Detalle |
|-------|---------|
| **Req. Trazabilidad** | RF-07 |
| **Objetivo** | Verificar que la comunicación con el sensor se haga por I2C. |
| **Precondiciones** | Hardware ensamblado. Firmware cargado. |
| **Resultado Esperado** | El sensor responde correctamente y la lectura se mantiene estable. |
| **Tipo** | Integration |
| **Evidencia** | Captura del monitor serial. |

#### TC-008 — Salida UART

| Campo | Detalle |
|-------|---------|
| **Req. Trazabilidad** | RF-08 |
| **Objetivo** | Verificar que el sistema envíe correctamente los datos por UART. |
| **Precondiciones** | Monitor serial configurado a 115200 bps. |
| **Resultado Esperado** | Los mensajes se leen sin caracteres corruptos. |
| **Tipo** | Integration |
| **Evidencia** | Captura del monitor serial. |

#### TC-009 — Tiempo de visualización

| Campo | Detalle |
|-------|---------|
| **Req. Trazabilidad** | RNF-01 |
| **Objetivo** | Verificar que la actualización visual ocurra en menos de 2 segundos. |
| **Precondiciones** | Sistema operando y cronómetro disponible. |
| **Resultado Esperado** | La lectura aparece en pantalla en menos de 2 segundos. |
| **Tipo** | System |
| **Evidencia** | Registro cronometrado. |

#### TC-010 — Estabilidad de operación

| Campo | Detalle |
|-------|---------|
| **Req. Trazabilidad** | RNF-02 |
| **Objetivo** | Verificar que el sistema funcione de forma continua durante la prueba. |
| **Precondiciones** | Sistema completamente ensamblado y en operación normal. |
| **Resultado Esperado** | El sistema permanece estable durante al menos 30 minutos. |
| **Tipo** | System |
| **Evidencia** | Log o video de la sesión. |

#### TC-011 — Montaje físico

| Campo | Detalle |
|-------|---------|
| **Req. Trazabilidad** | RNF-03 |
| **Objetivo** | Verificar que el dispositivo esté ensamblado de forma ordenada. |
| **Precondiciones** | Dispositivo completamente ensamblado. |
| **Resultado Esperado** | Se observa un montaje funcional y presentado de forma limpia. |
| **Tipo** | Inspection |
| **Evidencia** | Fotografías del dispositivo. |

#### TC-012 — Documentación básica

| Campo | Detalle |
|-------|---------|
| **Req. Trazabilidad** | RNF-04 |
| **Objetivo** | Verificar que el código y el repositorio tengan organización mínima. |
| **Precondiciones** | Repositorio accesible. |
| **Resultado Esperado** | El proyecto tiene archivos organizados y comentarios básicos en el código. |
| **Tipo** | Inspection |
| **Evidencia** | URL del repositorio. |