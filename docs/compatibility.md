# Compatibilidad y validación (2026-09-22)

| Variante | ESPHome | Resultado | Hardware |
|---|---|---|---|
| Snapshot del tag, credenciales ficticias | 2025.2.2 | Compila con Arduino, OTA/factory de prueba generados | No instalado |
| Refactor, credenciales ficticias, toolchain PlatformIO | 2026.9.0 | Compila con Arduino; OTA/factory de prueba generados | No instalado |
| Refactor, toolchain predeterminado local | 2026.9.0 | Bloqueado por dependencias del entorno Windows: xtensa-esp-elf/dfu-util | No instalado |
| Refactor | 2026.3.x / 2026.6.x | No ejecutado; compatibilidad no afirmada | No probado |

Las pruebas C++ de host pasan con Zig 0.16.0/C++17. El runner es `python tools/run_tests.py`; también acepta `CXX` o g++/clang++. CI ejecuta estas regresiones sin acceder a Home Assistant ni a dispositivos.

Se mantiene el framework Arduino. La compilación 2026 requiere `api.custom_services: true` para conservar el servicio `command` registrado desde C++; se encontró por una aserción del compilador y se corrigió. El acceso a componentes desde botones/switches usa el singleton `${init}`. Dependencia externa fijada en la rama experimental a `3bb2da2112ad0aef2163e4bfc694cac6d33c0cf0`. El snapshot original no la fijaba: recompilarlo hoy no demuestra identidad binaria con el firmware instalado.

## Windows

Se comprobó que el backend PlatformIO legado falla con `CreateProcess: No such file or directory` al tener espacios en la ruta de paquetes. Compiló usando un directorio de build temporal corto y una junction de paquetes sin espacios. Para Arduino 2026, ESP-IDF/CMake rechaza explícitamente espacios en el directorio de framework; se copiaron las dependencias a una carpeta temporal sin espacios (una junction no evitaba su resolución a la ruta original).

`tools/stage_compile_check.py` usa `%TEMP%/roomba-check-2025` o `roomba-check-2026` para el build en Windows; el usuario/ruta temporal también debe carecer de espacios. `PLATFORMIO_PACKAGES_DIR` debe apuntar a una ruta real sin espacios para 2026. Estos cambios afectan solo al entorno local de compilación.

Comandos usados, después de preparar las copias:

```text
build/venv-legacy/Scripts/python.exe -m esphome compile build/legacy-check/roomba.yaml
build/venv-2026/Scripts/python.exe -m esphome --toolchain platformio compile build/modern-check/roomba.yaml
```

PlatformIO es una elección de herramientas de compilación, no un cambio de framework. No se ha iniciado una migración del código a ESP-IDF. La primera reconstrucción 2026 falló al enlazar el servicio API `command` porque el árbol temporal se había generado antes de habilitar los servicios personalizados. Tras una limpieza de ese árbol temporal, `user_services.cpp` se incluyó y la compilación finalizó correctamente.

Los binarios de pruebas usan credenciales ficticias conocidas y **no deben instalarse**. Los binarios originales de recovery se conservan intactos. Compila, validado con simulación y probado en hardware son tres resultados diferentes.

