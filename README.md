# ESPRoomba de Ernesto

Firmware personalizado para Roomba 600/650 y ESP32-C3 Super Mini.

- `main`, `stable/legacy-2025.2.2` y `roomba-working-2025.2.2`: snapshot preservado, con el watchdog de Ernesto.
- `feat/esphome-2026`: refactorización experimental; **no instalada ni probada en hardware**.
- Se mantienen `irobot`, nombres/IDs existentes, Arduino, TX GPIO6, RX GPIO7 y 115200 baud.
- Los binarios de recuperación permanecen locales y excluidos de Git. No publicar firmware: puede contener credenciales compiladas.

## Documentación

- [Recuperación](docs/restore.md) y [manifiesto](recovery/manifest.json).
- [Hardware y UART](docs/hardware.md).
- [Detección y contabilidad](docs/activity.md).
- [Compatibilidad y compilaciones](docs/compatibility.md).
- [Pruebas y puertas de aprobación](docs/test-plan.md).
- [Inventario de Home Assistant](docs/home-assistant-audit.md).

## Compilación aislada, sin instalación

`python tools/stage_compile_check.py` crea una copia en `build/modern-check` con credenciales públicas ficticias; `--legacy` extrae el tag preservado en `build/legacy-check`. No lee `secrets.yaml`. No usar estos binarios para OTA.

Usar un entorno Python separado por versión y `python -m esphome compile <copia>/roomba.yaml`. Nunca usar `run`, `upload` o `rename` para una comprobación. En Windows, usar rutas de build y paquetes sin espacios (ver compatibilidad).

La configuración saneada conserva marcadores `!secret REDACTED` de la copia original: no son una configuración de producción lista para instalar. Antes de cualquier despliegue se deben reconciliar privadamente con las referencias exactas de la configuración activa, sin cambiar los valores de las claves.

## Créditos

Fork de [ShonP40/ESPRoomba](https://github.com/ShonP40/ESPRoomba); remoto `upstream` conservado. Implementación original y aportaciones de mannkind, davidecavestro, wburgers y philpownall. Licencia MIT del repositorio.
