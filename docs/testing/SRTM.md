# Software Requirements Traceability Matrix (SRTM)

| ID Requerimiento | Descripción | Caso de Prueba | Tipo de Prueba | Estado |
|---|---|---|---|---|
| RF-01 | Lectura de presión atmosférica | TC-001 | HIL/System | PASS |
| RF-02 | Validación de rango de presión | TC-002 | HIL/System | PASS |
| RF-03 | Detección de desconexión del sensor | TC-003 | HIL/System | Pendiente |
| RF-04 | Activación del actuador | TC-004 | HIL/System | PASS |
| RF-05 | Generación de mensajes INFO/WARN/ERROR | TC-005 | Integration | PASS |
| RF-06 | Comunicación I2C | TC-006 | Integration | PASS |
| RF-07 | Comunicación UART | TC-007 | Integration | PASS |
| RF-08 | Actualización de GUI en tiempo real | TC-008 | HIL/System | PASS |
| RF-09 | Configuración de umbrales desde GUI | TC-009 | HIL/System | PASS |
| RF-10 | Registro de eventos del sistema | TC-010 | HIL/System | PASS |
| RF-11 | Rutina de autodiagnóstico | TC-011 | Integration | PASS |
| RF-12 | Reconexión automática del sensor | TC-012 | HIL/System | Pendiente |
| RNF-01 | Tiempo de respuesta <= 2 segundos | TC-013 | HIL/System | PASS |
| RNF-02 | Operación continua por 2 horas | TC-014 | HIL/System | PASS |
| RNF-03 | Implementación física del sistema | TC-015 | Inspection | PASS |
| RNF-04 | Organización de documentación y repositorio | TC-016 | Inspection | En validación |
