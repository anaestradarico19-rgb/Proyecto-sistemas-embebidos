# Manómetro ambiental
## Technical lead: Ana María Estrada
## Firmware engineer: 
## Hardware integration engineer:
## Verification and testing engineer: Tomás Bedoya

### Introducción y descripción del problema:
En entornos hospitalarios, el monitoreo de condiciones ambientales es un factor crítico para garantizar la seguridad de pacientes y personal médico. La presión atmosférica es una variable que influye directamente en el funcionamiento de equipos médicos, la administración de gases medicinales y el control de ambientes especiales como quirófanos y salas de aislamiento, donde mantener diferenciales de presión controlados es indispensable para prevenir la contaminación cruzada y asegurar condiciones estériles. Sin embargo, a pesar de su relevancia clínica, esta variable frecuentemente no recibe el monitoreo continuo y especializado que su importancia demanda.
Muchos centros de salud carecen de instrumentos embebidos dedicados, accesibles y de bajo costo que permitan medir, registrar y visualizar la presión ambiental en tiempo real. Esta ausencia de herramientas adecuadas, representa un riesgo potencial para la seguridad clínica, dado que variaciones no detectadas en la presión del entorno pueden comprometer las condiciones controladas que espacios críticos como quirófanos y salas de aislamiento requieren.
Ante esta problemática, este proyecto propone el desarrollo de un manómetro ambiental embebido basado en el microcontrolador ESP32, diseñado específicamente para su uso en ambientes hospitalarios. El sistema es capaz de adquirir la presión atmosférica del entorno, validar los datos obtenidos, generar alertas ante condiciones anómalas y presentar la información al usuario mediante una interfaz gráfica clara e intuitiva. 

### Objetivo general:
Diseñar e implementar un manómetro ambiental embebido basado en el microcontrolador ESP32 que permita medir, procesar y visualizar en tiempo real la presión atmosférica en entornos hospitalarios, con el fin de facilitar el monitoreo de las condiciones ambientales y contribuir al control seguro de espacios clínicos.
Objetivos específicos:
1). Desarrollar el sistema de adquisición de datos mediante la integración de un sensor de presión con el microcontrolador ESP32 para capturar y procesar la presión atmosférica del entorno.
2). Implementar un sistema de validación y generación de alertas que permita identificar condiciones de presión anómalas y notificar al usuario cuando los valores se encuentren fuera de los rangos establecidos.
3). Diseñar una interfaz gráfica de visualización que permita presentar de forma clara e intuitiva los valores de presión en tiempo real para facilitar su interpretación por parte del usuario.

### Alcance del proyecto:
El presente proyecto contempla el diseño y desarrollo de un prototipo funcional de manómetro ambiental embebido basado en el microcontrolador ESP32, orientado al monitoreo de la presión atmosférica en entornos hospitalarios. El sistema estará compuesto por un sensor de presión integrado al microcontrolador, encargado de adquirir datos del entorno, procesarlos y mostrarlos en tiempo real mediante una interfaz gráfica diseñada para facilitar su interpretación por parte del usuario.
El dispositivo permitirá la medición continua de la presión atmosférica, la validación de los datos obtenidos y la generación de alertas cuando los valores de presión se encuentren fuera de los rangos previamente establecidos, simulando condiciones que podrían presentarse en ambientes hospitalarios como quirófanos o salas de aislamiento. Asimismo, el sistema ofrecerá una visualización clara de la información mediante una interfaz que permita observar los valores actuales de presión y el estado del sistema.
El alcance del proyecto se limita al diseño, programación e integración de los componentes electrónicos y de software necesarios para el funcionamiento del prototipo, incluyendo la lectura del sensor, el procesamiento de datos, la lógica de alertas y la visualización de la información. No se contempla la certificación médica del dispositivo ni su implementación directa en instalaciones hospitalarias reales, sino la demostración de la viabilidad técnica de un sistema embebido de bajo costo para el monitoreo ambiental.
