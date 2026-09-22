"""Native ESPHome registration for the Roomba polling component."""

from esphome import codegen as cg
import esphome.config_validation as cv
from esphome.components import time, uart
from esphome.const import CONF_ID, CONF_UPDATE_INTERVAL

CONF_UART_ID = "uart_id"
CONF_BRC_PIN = "brc_pin"
CONF_LAZY_650_ENABLED = "lazy_650_enabled"
CONF_TIME_ID = "time_id"

RoombaComponent = cg.global_ns.class_("RoombaComponent", cg.PollingComponent, uart.UARTDevice)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(RoombaComponent),
        cv.Required(CONF_UART_ID): cv.use_id(uart.UARTComponent),
        cv.Required(CONF_BRC_PIN): cv.int_range(min=0, max=48),
        cv.Required(CONF_TIME_ID): cv.use_id(time.RealTimeClock),
        cv.Optional(CONF_LAZY_650_ENABLED, default=True): cv.boolean,
    }
).extend(cv.polling_component_schema("10s"))


async def to_code(config):
    cg.add_global(cg.RawStatement('#include "Roomba.h"'), prepend=True)
    parent = await cg.get_variable(config[CONF_UART_ID])
    clock = await cg.get_variable(config[CONF_TIME_ID])
    var = cg.new_Pvariable(
        config[CONF_ID],
        config[CONF_BRC_PIN],
        parent,
        config[CONF_UPDATE_INTERVAL].total_milliseconds,
        config[CONF_LAZY_650_ENABLED],
        clock,
    )
    await cg.register_component(var, config)
