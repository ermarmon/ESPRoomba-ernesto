# Plan de pruebas

## Sin dispositivo

Compilar `tests/test_roomba.cpp` con C++17, incluyendo `tests/stubs`. Ejecuta el parser y la clase real RoombaComponent con UART simulada. Incluye: temperatura con signo, capacidad cero, carga incoherente, flags/rangos inválidos, todas las longitudes truncadas, bytes extra, fragmentación con retardos, deadline global de 200 ms, reintento único, preservación de sensores tras doble fallo, rollover de millis, parada con consumo residual, batería baja aún limpiando, motores constantes, ruedas sin cepillo, pérdida de comunicación y huecos en contabilidad.

Compilar firmware separado con ESPHome 2025.2.2 y 2026.9.0/Arduino; conservar logs y versiones. Nunca equiparar pruebas de stubs con pruebas de UART real.

## Puertas previas a OTA

1. Binarios de recuperación originales y SHA-256 verificados, USB disponible.
2. Configuración activa exacta y referencias de secretos recuperadas; explicar cualquier límite de reproducibilidad pendiente.
3. Compilación final con credenciales originales privada, sin cambios de framework, UART física, nombre ni claves; comparar nombres e IDs.
4. Presentar cambio final, compilación y riesgos al propietario. Solicitar autorización explícita. Si hay bloqueos pendientes, no proponer una OTA como lista.

## Prueba mínima en hardware, tras autorización

Con persona presente, batería suficiente y medios de recuperación disponibles:

- Registrar referencia legada: sensores y llegada de paquetes en reposo/base y limpieza. Guardar tiempos y consumos del cepillo antes/durante/después de una parada.
- Probar primero compatibilidad Arduino 2026 con el mínimo cambio de UART y transporte, separando ese resultado de los umbrales de actividad. Conservar una variante de comparación basada en el tag; no mezclar varios cambios de hardware.
- Tras instalar la variante aprobada: comprobar API **y** muestras OI nuevas durante al menos 5 minutos, no solo valores guardados. Si no hay muestras o aparecen fallos repetidos, parar la prueba y restaurar.
- Comprobar inicio normal/spot, parada por botón, retorno a base, pausa, carga y reinicio ESP. Supervisar al menos una parada natural por batería; no forzar descarga profunda.
- Al cesar el cepillo: contador deja de sumar en la siguiente muestra; actividad abandona Cleaning tras 20 segundos de evidencia continuada. Con UART sin respuestas: cómputo suspendido al fallo y responsive false/actividad Lost al caducar 30 segundos.
- Comprobar también obstáculos y cepillo atascado: corriente no equivale a movimiento; documentar falsos positivos antes de confiar mantenimiento a esta heurística.
- Comparar incremento de tiempo confirmado con duración observada, y validar estadísticas tras reinicio y pérdida de Wi-Fi.

No modificar mantenimientos de HA hasta validar estos resultados y obtener autorización para migrarlos. El firmware por sí solo no cambia sus fuentes actuales.
