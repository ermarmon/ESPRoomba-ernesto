# Prueba real de la Roomba, paso a paso

Esta guía sirve para la primera prueba del firmware de la rama `feat/esphome-2026`.

**No instales nada todavía.** La Roomba debe estar en la base, con una lectura de batería estable superior al 50 %, y debes poder llegar físicamente al ESP32-C3 y a un cable USB. Si la batería marca algo raro, como ahora, espera a que cargue y compruébalo de nuevo.

## Qué ya está preparado

- El firmware de prueba 2026 compila con Arduino.
- Se conserva el firmware de recuperación 2025.2.2 y sus hashes.
- La prueba incorporará `OI valid responses` y `OI invalid responses`: sirven para saber si entran paquetes UART nuevos.
- No se han cambiado automatizaciones, entidades existentes ni la Roomba.

## Antes de instalar

1. Pon la Roomba en la base y espera a que la batería supere el 50 %.
2. Abre Home Assistant y mira el dispositivo **iRobot**. Debes ver que está disponible y que sus sensores de tensión, corriente, batería y OI mode muestran valores.
3. Comprueba que tienes acceso físico al ESP32-C3 y a su USB. No conectes USB mientras el ESP siga alimentado desde la Roomba sin revisar primero el cableado.
4. Confirma que están los dos archivos de `recovery/` y que sus SHA-256 coinciden con `recovery/SHA256SUMS`.
5. Detente aquí y pide a Codex que haga la comprobación previa. No pulses Update por tu cuenta.

## La única acción que requiere tu autorización

Cuando decidas hacerlo, escribe exactamente:

> Autorizo una única OTA de prueba del firmware `feat/esphome-2026` en la Roomba irobot.

Codex preparará y comprobará el binario privado. No se usarán los binarios de `build/`, porque contienen credenciales ficticias y no sirven para el robot.

## Justo después de la OTA

Espera dos minutos con la Roomba en la base. En Home Assistant, busca estas entidades nuevas:

- `iRobot OI valid responses`
- `iRobot OI invalid responses`
- `iRobot confirmed cleaning`
- `iRobot confirmed cleaning time`

Resultado correcto:

- `OI valid responses` sube aproximadamente una vez cada diez segundos.
- `OI invalid responses` sigue en 0.
- Las entidades antiguas siguen presentes.
- OI mode, voltaje, corriente y batería cambian o se actualizan normalmente.
- `confirmed cleaning` está apagado y el tiempo confirmado no aumenta estando en la base.

Si no es así, para. No pulses Clean. Comunícaselo a Codex para recuperar el firmware anterior de forma controlada.

## Prueba corta de limpieza

Solo si el paso anterior fue correcto:

1. Pulsa **Clean** y observa la Roomba durante dos minutos.
2. Comprueba que `OI valid responses` sigue subiendo.
3. Comprueba que `confirmed cleaning` pasa a activo.
4. Comprueba que `confirmed cleaning time` aumenta.
5. Pulsa **Stop**.
6. Espera medio minuto. `confirmed cleaning` debe apagarse y el contador debe dejar de aumentar.
7. Pulsa **Dock** y comprueba que vuelve a la base.

No fuerces una descarga completa. La prueba de parada por batería se hará otro día, de forma natural y supervisada.

## Cuándo detenerse

Para inmediatamente si ocurre cualquiera de estas cosas:

- `OI valid responses` no aumenta durante 30 segundos.
- `OI invalid responses` aumenta repetidamente.
- Desaparecen entidades conocidas.
- Voltaje, batería o corriente se vuelven desconocidos o absurdos.
- La Roomba no responde a Stop o Dock.

No modifiques aún los contadores de mantenimiento ni las automatizaciones. Primero confirmaremos varias limpiezas correctas.
