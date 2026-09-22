# Estado, comunicación y horas de limpieza

El watchdog fue añadido por Ernesto al YAML; se conserva su intención, nombres e IDs. No formaba parte del upstream.

## Fallos identificados

1. Comparaba `cleaning` con `Cleaning`: las ramas de inactividad no se ejecutaban.
2. Reiniciaba el timeout leyendo una cadena OI almacenada, aunque no llegaran paquetes nuevos.
3. Un cambio de batería/voltaje se tomaba por movimiento. Un valor motor constante podía parecer inactividad.
4. El componente llamaba `Cleaning` a cualquier consumo inferior a -300 mA. El consumo total no prueba que el cepillo siga funcionando.
5. Temperatura OI interpretada sin signo; respuesta sin validación y timeout reiniciado por byte.

## Contrato nuevo

`RoombaProtocol.h` separa decodificación y seguimiento de estado del transporte UART. Solo una respuesta completa y validada renueva la marca de comunicación. `responsive` exige una muestra de menos de 30 segundos, y es falso al arrancar sin datos. OI `off` sigue siendo un valor de protocolo válido, aunque no implique que se esté limpiando.

La evidencia de limpieza exige: sin fuente de carga, OI distinto de off, estado de carga 0 o 4, descarga superior a 300 mA y corriente absoluta del cepillo principal de al menos 100 mA. Son heurísticas pendientes de calibración. Corriente no demuestra rotación: un cepillo bloqueado puede consumir; no se garantiza detección de atasco mecánico.

Batería baja por sí sola nunca termina la limpieza. Si desaparece la evidencia del cepillo, `confirmed cleaning` pasa a falso en la primera muestra válida. El estado compatible `Cleaning` tolera 20 segundos de ausencia continuada antes de pasar a `Lost`. Se reutiliza `Lost` para que los consumidores existentes no se queden en `Limpiando` por desconocer un estado nuevo. No se introduce un estado `Idle` sin migrar antes sus consumidores.

`Docked` requiere una fuente de carga; ya no se deduce de un consumo cercano a cero fuera de la base. `Charging` usa fuente presente y estado de carga 1..3. Un fallo de carga pasa a `Lost`.

Al fallar una consulta se suspende el cómputo inmediatamente. Se preservan los sensores crudos; después de 30 segundos sin muestras válidas, la actividad derivada caduca a `Lost`. Esto no afirma que el robot esté físicamente parado: indica que no hay información fiable. `responsive` diferencia ese caso de una pausa conocida.

## Contabilidad y mantenimiento

Nuevas entidades ESPHome: `irobot confirmed cleaning` y `irobot confirmed cleaning time` (segundos, `total_increasing`). No se reutiliza `vacuum state`: ese sensor existente refleja órdenes manuales de motor, no observación física.

El contador acumula solo intervalos entre dos muestras consecutivas válidas con evidencia de limpieza, separados como máximo 15 segundos. No integra durante huecos de comunicación, ni inventa duración desde la última orden. Reinicia a cero al reiniciar ESP32: Home Assistant puede acumular sus incrementos mediante estadísticas `total_increasing`. No es un contador persistente independiente de HA. Puede perder tiempo al inicio/final de intervalos y durante desconexiones; tampoco recupera muestras que HA nunca recibió.

El polling de 10 segundos y el antirrebote del estado no permiten exactitud al segundo. El nuevo contador es conservador y distinto del tiempo durante el que el estado muestra `Cleaning`.

**No se han migrado los contadores, mantenimientos ni automatizaciones de Home Assistant**: están fuera de la autorización actual. Tras probar hardware, inventariar sus fuentes y sustituir la dependencia del estado atascado por incrementos del contador validado. Usar helpers nativos apropiados para acumulados/períodos, conservar los baselines de mantenimiento y comprobar reinicios antes del cambio. El historial anterior inflado no se puede corregir automáticamente sin evidencia.

La puntuación de salud ahora cae por debajo de 50 ante pérdida sostenida de comunicación (más de 60 s), aunque la actividad ya haya caducado a Lost. Así `needs attention` puede activarse tras su filtro de 60 s; antes la penalización máxima de comunicación era solo 40 y nunca bastaba por sí sola. Se conservan los filtros de aviso para evitar parpadeos.
