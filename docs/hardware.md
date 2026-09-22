# Hardware y UART

Confirmado por el propietario: ESP32-C3 Super Mini, Roomba 600/650, Arduino, TX GPIO6, RX GPIO7, 115200 baud. BRC GPIO10 e intervalo de consulta 10 segundos en el YAML preservado. LED GPIO8: conservar el cableado actual; es pin de arranque y el validador avisa.

La consulta OI `Query List` (149) solicita los paquetes 21,22,23,25,26,24,35,55,54,56,57,13,34,18, en ese orden. Sus longitudes suman 22 bytes. No hay cabecera ni checksum en esta respuesta. La especificación OI del fabricante se conserva en el PDF del repositorio. No confundirla con la respuesta de `Stream`, que tiene otro formato.

Configuración experimental ya iniciada en la copia de Ernesto: `rx_full_threshold: 22`, `rx_timeout: 10`, debug UART desactivado. Según [ESPHome UART](https://esphome.io/components/uart/), el primero determina cuántos bytes hacen disponible la recepción; el segundo usa tiempos de byte, **no milisegundos**. A 115200 baud/8N1, diez tiempos de byte son aproximadamente 0,87 ms. Son decisiones de prueba, no una reparación demostrada del fallo anterior con ESPHome 2026.

La lectura del componente dispone de 200 ms para toda la respuesta, con resta unsigned compatible con rollover de millis. Una respuesta incompleta o incoherente no actualiza sensores crudos. Se espera silencio RX de 10 ms, como máximo 200 ms, antes de un único reintento. Ruido continuo cancela ese reintento. Si ambos fallan, el siguiente polling vuelve a intentarlo. Tiempo máximo aproximado de recepción y drenaje: 800 ms por polling fallido; las secuencias BRC heredadas añaden sus propios delays.

Se rechazan longitudes incorrectas, bytes extra ya disponibles, carga 0..5/OI 0..3 fuera de rango, capacidad cero, carga superior a capacidad, flags imposibles y rangos físicos incoherentes. Límites de diseño amplios: 8..22 V, capacidad 100..20000 mAh, batería -20..85 °C, corriente total ±10 A y motores ±5 A. No son límites oficiales del protocolo. Valores excepcionales legítimos podrían ser rechazados; verificar con la batería instalada.

Sin framing ni checksum, un desplazamiento que produzca valores plausibles todavía puede pasar. Bytes tardíos posteriores al silencio también son una limitación. No afirmar integridad garantizada; registrar fallos y contrastar en hardware.

El framework permanece Arduino. Que un compilador 2026 use herramientas ESP-IDF debajo de Arduino no equivale a migrar el proyecto a `framework.type: esp-idf`.

## Cambio de backend confirmado en las versiones instaladas

El código de `components/uart/__init__.py` de ESPHome 2025.2.2 selecciona `ESP32ArduinoUARTComponent` para ESP32/Arduino. En 2026.9.0 selecciona `IDFUARTComponent` para todo ESP32. El backend usa `uart_driver_install`, `uart_set_rx_full_threshold` y `uart_set_rx_timeout`. Conservar Arduino no evita ese cambio interno. Esto es un hecho comprobado en el código instalado; que sea la causa del fallo anterior sigue siendo una hipótesis sin reproducción en hardware.

Los contadores nuevos `OI valid responses` y `OI invalid responses` permiten distinguir recepción de paquetes nuevos de valores antiguos que parecen plausibles. El contador válido debe seguir aumentando aunque voltaje u OI mode no cambien. El contador inválido incluye cada intento fallido, también si el reintento siguiente se recupera. Reinician al arrancar.
