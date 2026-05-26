# Funcionamiento del Sistema Embebido

Este proyecto consiste en un sistema embebido para el monitoreo de presión atmosférica y temperatura en dos zonas diferentes de un entorno hospitalario: una zona interior y una zona exterior. El sistema utiliza dos sensores de presión y temperatura para realizar mediciones en tiempo real y comparar las condiciones ambientales entre ambos espacios.

## Indicadores de presión

El sistema compara continuamente la presión medida en el interior y en el exterior, activando diferentes LEDs según la condición detectada:

- **LED Rojo:** se enciende cuando la presión interior es igual a la presión exterior.
- **LED Azul:** se enciende cuando la presión interior es mayor que la presión exterior.
- **LED Verde:** se enciende cuando la presión interior es menor que la presión exterior.

Estos indicadores permiten visualizar rápidamente el estado de presión diferencial del ambiente.

## Configuración de rangos

El usuario puede configurar mediante comandos por consola los rangos permitidos de:

- Presión interior
- Presión exterior
- Temperatura interior
- Temperatura exterior

Para configurar los valores del ambiente interior se utiliza el comando:

set in presion_min presion_max temperatura_min temperatura_max

Ejemplo:
set in 700 800 16 26

Para configurar los valores del ambiente exterior se utiliza el comando:

set out presion_min presion_max temperatura_min temperatura_max

Ejemplo:
set out 600 900 20 25

Los valores deben ingresarse separados únicamente por espacios, sin comas ni otros caracteres adicionales.

## Sistema de alarma

Cuando alguna medición de presión o temperatura sale de los rangos configurados, el sistema activa una alarma sonora mediante un buzzer para alertar al usuario sobre una condición anormal.

Además, el sistema incorpora un botón físico que permite silenciar temporalmente la alarma. Al presionar el botón, el buzzer se desactiva durante aproximadamente un minuto, después del cual se reinicia el sistema. 
