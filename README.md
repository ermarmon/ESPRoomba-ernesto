# ESPRoomba de Ernesto

Firmware personalizado para Roomba 600/650 y ESP32-C3 Super Mini.

- `main`, `stable/legacy-2025.2.2` y `roomba-working-2025.2.2`: snapshot preservado, con el watchdog de Ernesto.
- `feat/esphome-2026`: refactorizaciÃ³n experimental; **no instalada ni probada en hardware**.
- Se mantienen `irobot`, nombres/IDs existentes, Arduino, TX GPIO6, RX GPIO7 y 115200 baud.
- Los binarios de recuperaciÃ³n permanecen locales y excluidos de Git. No publicar firmware: puede contener credenciales compiladas.

## DocumentaciÃ³n

- [RecuperaciÃ³n](docs/restore.md) y [manifiesto](recovery/manifest.json).
- [Hardware y UART](docs/hardware.md).
- [DetecciÃ³n y contabilidad](docs/activity.md).
- [Compatibilidad y compilaciones](docs/compatibility.md).
- [Pruebas y puertas de aprobaciÃ³n](docs/test-plan.md).
- [Inventario de Home Assistant](docs/home-assistant-audit.md).

## CompilaciÃ³n aislada, sin instalaciÃ³n

`python tools/stage_compile_check.py` crea una copia en `build/modern-check` con credenciales pÃºblicas ficticias; `--legacy` extrae el tag preservado en `build/legacy-check`. No lee `secrets.yaml`. No usar estos binarios para OTA.

Usar un entorno Python separado por versiÃ³n y `python -m esphome compile <copia>/roomba.yaml`. Nunca usar `run`, `upload` o `rename` para una comprobaciÃ³n. En Windows, usar rutas de build y paquetes sin espacios (ver compatibilidad).

La configuraciÃ³n saneada conserva marcadores `!secret REDACTED` de la copia original: no son una configuraciÃ³n de producciÃ³n lista para instalar. Antes de cualquier despliegue se deben reconciliar privadamente con las referencias exactas de la configuraciÃ³n activa, sin cambiar los valores de las claves.

## CrÃ©ditos

Fork de [ShonP40/ESPRoomba](https://github.com/ShonP40/ESPRoomba); remoto `upstream` conservado. ImplementaciÃ³n original y aportaciones de mannkind, davidecavestro, wburgers y philpownall. Licencia MIT del repositorio.

