# Restauración de irobot

**Documento de procedimiento; ninguna instalación ha sido ejecutada. Hace falta autorización explícita antes de OTA o USB.**

## Material que debe conservarse

- Dos binarios originales locales `recovery/irobot-2025.2.2-recovery.ota.bin` y `recovery/irobot-2025.2.2-recovery.factory.bin`.
- `recovery/manifest.json` y `SHA256SUMS`.
- Snapshot `roomba-working-2025.2.2`, commit `4379489eef02a96e1807f0615d8f1370615ff1dc` (YAML saneado y Roomba.h).
- YAML activo completo `esp-roomba.yaml` y archivos locales exactos, con sus referencias de secretos conservadas privadamente en Home Assistant. El repo saneado NO reemplaza esa copia exacta: repite `!secret REDACTED` para valores de distinta naturaleza.
- ESPHome Builder 2025.2.2, framework Arduino y acceso físico USB recuperable.

Comprobar SHA-256 con `Get-FileHash -Algorithm SHA256 <archivo>` y comparar con el manifiesto antes de cualquier instalación. Los hashes verifican integridad del archivo conservado, no su equivalencia con la flash que está ejecutando ahora el ESP.

## Restaurar configuración y recompilar

Crear una carpeta de recuperación separada; extraer allí el tag con `git archive` o un checkout separado. Nunca resetear el árbol de trabajo ni sobrescribir la configuración activa para hacerlo. Recuperar privadamente las referencias de secretos del YAML original; mantener las claves API, OTA y Wi-Fi actuales. No imprimir ni exportar secretos.

Antes de llamar a esa compilación "exacta" hay que recuperar el commit de la dependencia `robertklep/esphome-custom-component` de la caché/registro original y cualquier diferencia entre YAML activo y snapshot. El snapshot original no fijaba ese commit. Compilar con Builder 2025.2.2; registrar log, versiones y hashes de los nuevos binarios sin publicarlos. `tools/stage_compile_check.py --legacy` solo prueba compilación con credenciales ficticias y NO genera un sustituto instalable de recuperación.

## OTA de recuperación, solo tras aprobación

Con API/Wi-Fi disponibles y alimentación estable, seleccionar el binario **OTA original** verificado y usar el cargador ESPHome con las credenciales existentes. No usar factory para OTA ni los binarios ficticios. Mantener Builder 2025.2.2 disponible; no actualizarlo durante la prueba. Registrar el resultado y verificar versión reportada antes de dar por restaurado.

## Si OTA falla: USB

Desconectar la conexión/alimentación procedente de la Roomba antes de alimentar por USB; evitar retroalimentación del buck y bus UART. Confirmar el esquema real y conexión del ESP32-C3 antes de manipular. Identificar su puerto serie; si no entra en bootloader, seguir la secuencia BOOT/RESET de esa placa. Verificar con esptool que el chip es ESP32-C3.

Tras aprobación, usar el binario **factory combinado** con el cargador USB ESPHome/esptool y offset 0x0 de ESP32-C3. Confirmar primero que el archivo es una imagen combinada válida para esa placa. No ejecutar borrado total como primer paso. No conectar una imagen OTA sola en offset 0x0. Reconectar a la Roomba solo después de retirar USB y comprobar cableado/alimentación.

## Comprobaciones posteriores

Confirmar API cifrada con claves originales y entidades existentes, firmware 2025.2.2, OI mode coherente, tensión/corriente, temperatura, capacidad/carga/batería, fuentes de carga y respuesta a botones. Con autorización de operación y supervisión, probar start/stop/dock; confirmar recepción UART en reposo, limpieza y al agotarse la batería. Una API accesible sin sensores OI NO es recuperación completa.

El contador ESP experimental y sus nuevos sensores no forman parte del firmware legado. Sus estadísticas posteriores no deben interpretarse como si siguieran midiendo al restaurar el legado.
