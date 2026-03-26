from esphome import automation
from esphome.automation import maybe_simple_id
import esphome.codegen as cg
from esphome.components import sensor, uart
import esphome.config_validation as cv
from esphome.const import (
    CONF_CHANNEL, CONF_CURRENT, CONF_ENERGY, CONF_FREQUENCY, CONF_ID, CONF_NAME, CONF_POWER, CONF_TEMPERATURE, CONF_TOTAL_POWER, CONF_VOLTAGE, CONF_POWER_FACTOR, DEVICE_CLASS_CURRENT, DEVICE_CLASS_ENERGY, DEVICE_CLASS_FREQUENCY, DEVICE_CLASS_POWER, DEVICE_CLASS_TEMPERATURE, DEVICE_CLASS_VOLTAGE, DEVICE_CLASS_POWER_FACTOR, ICON_CURRENT_AC, ICON_THERMOMETER, STATE_CLASS_MEASUREMENT, STATE_CLASS_TOTAL_INCREASING, UNIT_AMPERE, UNIT_CELSIUS, UNIT_HERTZ, UNIT_KILOWATT_HOURS, UNIT_VOLT, UNIT_WATT,
)

ICON_POWER = "mdi:gauge"
ICON_ENERGY = "mdi:lightning-bolt"
ICON_FREQUENCY = "mdi:metronome"
ICON_VOLTAGE = "mdi:sine-wave"
ICON_POWER_FACTOR = "mdi:angle-acute"

DEPENDENCIES = ["uart"]
AUTO_LOAD = ["bl0910-m"]
CONF_TOTAL_ENERGY = "total_energy"
CONF_RESISTOR = "resistor"  # 新增电阻配置项

bl0910_ns = cg.esphome_ns.namespace("bl0910")
BL0910 = bl0910_ns.class_("BL0910", cg.PollingComponent, uart.UARTDevice)
ResetEnergyAction = bl0910_ns.class_("ResetEnergyAction", automation.Action)

def create_sensor_schema(icon, accuracy_decimals, device_class, unit, state_class):
    return sensor.sensor_schema(
        icon=icon,
        accuracy_decimals=accuracy_decimals,
        device_class=device_class,
        unit_of_measurement=unit,
        state_class=state_class,
    )

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(BL0910),
            cv.Optional(CONF_FREQUENCY): create_sensor_schema(ICON_FREQUENCY, 2, DEVICE_CLASS_FREQUENCY, UNIT_HERTZ, STATE_CLASS_MEASUREMENT),
            cv.Optional(CONF_TEMPERATURE): create_sensor_schema(ICON_THERMOMETER, 2, DEVICE_CLASS_TEMPERATURE, UNIT_CELSIUS, STATE_CLASS_MEASUREMENT),
            cv.Optional(CONF_VOLTAGE): create_sensor_schema(ICON_VOLTAGE, 1, DEVICE_CLASS_VOLTAGE, UNIT_VOLT, STATE_CLASS_MEASUREMENT),
            cv.Optional(CONF_TOTAL_POWER): create_sensor_schema(ICON_POWER, 3, DEVICE_CLASS_POWER, UNIT_WATT, STATE_CLASS_MEASUREMENT),
            cv.Optional(CONF_TOTAL_ENERGY): create_sensor_schema(ICON_ENERGY, 3, DEVICE_CLASS_ENERGY, UNIT_KILOWATT_HOURS, STATE_CLASS_TOTAL_INCREASING),
        }
    )
    .extend(
        cv.Schema(
            {
                cv.Optional(f"{CONF_CHANNEL}_{i + 1}"): cv.Schema(
                    {
                        cv.Optional(CONF_CURRENT): cv.maybe_simple_value(
                            create_sensor_schema(ICON_CURRENT_AC, 3, DEVICE_CLASS_CURRENT, UNIT_AMPERE, STATE_CLASS_MEASUREMENT),
                            key=CONF_NAME,
                        ),
                        cv.Optional(CONF_POWER): cv.maybe_simple_value(
                            create_sensor_schema(ICON_POWER, 3, DEVICE_CLASS_POWER, UNIT_WATT, STATE_CLASS_MEASUREMENT),
                            key=CONF_NAME,
                        ),
                        cv.Optional(CONF_ENERGY): cv.maybe_simple_value(
                            create_sensor_schema(ICON_ENERGY, 3, DEVICE_CLASS_ENERGY, UNIT_KILOWATT_HOURS, STATE_CLASS_TOTAL_INCREASING),
                            key=CONF_NAME,
                        ),
                        cv.Optional(CONF_POWER_FACTOR): cv.maybe_simple_value(
                            create_sensor_schema(ICON_POWER_FACTOR, 3, DEVICE_CLASS_POWER_FACTOR, "", STATE_CLASS_MEASUREMENT),
                            key=CONF_NAME,
                        ),
                        cv.Optional(CONF_RESISTOR, default=5.1): cv.float_,  # 允许配置自定义电阻，默认5.1
                    }
                )
                for i in range(10)
            }
        )
    )
    .extend(uart.UART_DEVICE_SCHEMA)
    .extend(cv.polling_component_schema("10s"))
)

FINAL_VALIDATE_SCHEMA = uart.final_validate_device_schema(
    "bl0910", baud_rate=19200, require_tx=True, require_rx=True
)

@automation.register_action(
    "bl0910.reset_energy",
    ResetEnergyAction,
    maybe_simple_id(
        {
            cv.Required(CONF_ID): cv.use_id(BL0910),
        }
    ),
)
async def reset_energy_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    return var

async def register_sensor(var, config, sensor_name, sensor_fn):
    if sensor_config := config.get(sensor_name):
        sens = await sensor.new_sensor(sensor_config)
        cg.add(sensor_fn(sens))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    await register_sensor(var, config, CONF_FREQUENCY, var.set_frequency_sensor)
    await register_sensor(var, config, CONF_TEMPERATURE, var.set_temperature_sensor)
    await register_sensor(var, config, CONF_VOLTAGE, var.set_voltage_sensor)
    await register_sensor(var, config, CONF_TOTAL_POWER, var.set_total_power_sensor)
    await register_sensor(var, config, CONF_TOTAL_ENERGY, var.set_total_energy_sensor)

    for i in range(10):
        if channel_config := config.get(f"{CONF_CHANNEL}_{i + 1}"):
            await register_sensor(var, channel_config, CONF_CURRENT, getattr(var, f"set_current_{i + 1}_sensor"))
            await register_sensor(var, channel_config, CONF_POWER, getattr(var, f"set_power_{i + 1}_sensor"))
            await register_sensor(var, channel_config, CONF_ENERGY, getattr(var, f"set_energy_{i + 1}_sensor"))
            await register_sensor(var, channel_config, CONF_POWER_FACTOR, getattr(var, f"set_power_factor_{i + 1}_sensor"))
            # 将 YAML 中配置的电阻值传递给 C++ 对象
            cg.add(var.set_resistor(i, channel_config[CONF_RESISTOR]))
