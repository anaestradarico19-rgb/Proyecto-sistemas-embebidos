# Test de verificación — Sensor BME280 (Arduino Framework)

Antes de implementar el driver definitivo en ESP-IDF, se realizó una 
prueba de verificación del sensor BME280 usando Arduino framework 
para confirmar el funcionamiento del hardware y las conexiones físicas.
Se dejaron estos documentos en otra rama (master) para que no se generara conflicto 
en la rama main con el archivo platformio.ini debido a que aca se trabajo en arduino
por lo que la configuración es diferente. 

## Resultado
Sensor BME280 detectado correctamente en dirección I2C 0x76  
Lecturas de presión, temperatura y humedad funcionando a 1 Hz  
Módulo de logging con niveles INFO/WARN/ERROR verificado  
Formato de mensajes con timestamp confirmado  

## Lecturas obtenidas
Presión: ~846 hPa (normal para Medellín a ~1500m de altitud)  
Temperatura: ~29°C  
Humedad: ~40%  

## Nota sobre los umbrales
Los umbrales de alerta (950/1050 hPa) están calibrados para nivel 
del mar. Para Medellín se ajustarán a 820/870 hPa en la 
implementación definitiva en ESP-IDF.
