# Auditoría de Home Assistant (lectura, 2026-09-22)

MCP confirmó firmware instalado 2025.2.2, actualización automática desactivada, Builder normal 2026.9.0 y legado 2025.2.2, ambos iniciados. `irobot`, IP 192.168.1.62, MAC 94:A9:90:96:0B:3C. En la lectura inicial: actividad Cleaning, OI passive, fuente de carga off, responsive on. No se considera fiable el timeout antiguo, por el defecto de caché identificado.

Se encontró `automation.actualizar_estado_roomba`: tras 15 segundos de cambio de actividad, traduce Cleaning → Limpiando, Lost → Perdida y Docked → En la base en `input_select.estado_roomba`; no contempla Charging. Se preservan estos estados y se documenta esa carencia, sin modificar la automatización.

Otros consumidores encontrados: pruebas_roomba, enviar_a_roomba_a_casa, roomba_cargada_y_lista_para_limpiar, roomba_reset_contador_inicio_o_lost_1h, roomba_reset_contador_limpieza_actual, roomba_dock_bermuda_distancias_umbral_dinamico_ui_helpers, luces_pasillo_on y script.volver_a_la_base_roomba.

La búsqueda MCP fue **parcial**: 26 automatizaciones y 16 scripts no se pudieron inspeccionar por 404 de las definiciones YAML. No es un inventario exhaustivo de contadores ni dashboards. No se han cambiado entidades, automatizaciones ni configuración activa. La plantilla de `home-assistant.yaml` del repo es un ejemplo upstream, no una copia verificada de la instalación.

Las lecturas históricas consultadas mostraron actividad y corriente de cepillo, pero no identificaron inequívocamente el episodio de parada por batería. No se afirma que los umbrales nuevos estén calibrados ni que el fallo 2026 previo esté reproducido.
