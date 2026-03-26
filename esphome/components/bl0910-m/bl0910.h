#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/core/datatypes.h"

namespace esphome
{
  namespace bl0910
  {
    // 新增：区分请求哪种类型的数据，以调用对应的倍率计算函数
    enum DataType { TYPE_VOLTAGE, TYPE_CURRENT, TYPE_POWER, TYPE_ENERGY, TYPE_FREQUENCY, TYPE_TEMPERATURE, TYPE_TOTAL_POWER, TYPE_TOTAL_ENERGY };

    struct DataPacket
    { 
      uint8_t l{0};
      uint8_t m{0};
      uint8_t h{0};
      uint8_t checksum; 
      uint8_t address;
    } __attribute__((packed));

    struct ube24_t
    { 
      uint8_t l{0};
      uint8_t m{0};
      uint8_t h{0};
    } __attribute__((packed));

    struct sbe24_t
    { 
      uint8_t l{0};
      uint8_t m{0};
      int8_t h{0};
    } __attribute__((packed));

    template <typename... Ts>
    class ResetEnergyAction;

    class BL0910;

    using ActionCallbackFuncPtr = void (BL0910::*)();

    class BL0910 : public PollingComponent, public uart::UARTDevice
    {
      SUB_SENSOR(voltage)
      SUB_SENSOR(current_1)
      SUB_SENSOR(current_2)
      SUB_SENSOR(current_3)
      SUB_SENSOR(current_4)
      SUB_SENSOR(current_5)
      SUB_SENSOR(current_6)
      SUB_SENSOR(current_7)
      SUB_SENSOR(current_8)
      SUB_SENSOR(current_9)
      SUB_SENSOR(current_10)
      SUB_SENSOR(power_1)
      SUB_SENSOR(power_2)
      SUB_SENSOR(power_3)
      SUB_SENSOR(power_4)
      SUB_SENSOR(power_5)
      SUB_SENSOR(power_6)
      SUB_SENSOR(power_7)
      SUB_SENSOR(power_8)
      SUB_SENSOR(power_9)
      SUB_SENSOR(power_10)
      SUB_SENSOR(total_power)
      SUB_SENSOR(power_factor_1)
      SUB_SENSOR(power_factor_2)
      SUB_SENSOR(power_factor_3)
      SUB_SENSOR(power_factor_4)
      SUB_SENSOR(power_factor_5)
      SUB_SENSOR(power_factor_6)
      SUB_SENSOR(power_factor_7)
      SUB_SENSOR(power_factor_8)
      SUB_SENSOR(power_factor_9)
      SUB_SENSOR(power_factor_10)
      SUB_SENSOR(energy_1)
      SUB_SENSOR(energy_2)
      SUB_SENSOR(energy_3)
      SUB_SENSOR(energy_4)
      SUB_SENSOR(energy_5)
      SUB_SENSOR(energy_6)
      SUB_SENSOR(energy_7)
      SUB_SENSOR(energy_8)
      SUB_SENSOR(energy_9)
      SUB_SENSOR(energy_10)
      SUB_SENSOR(total_energy)
      SUB_SENSOR(frequency)
      SUB_SENSOR(temperature)

    public:
      void loop() override;
      void update() override;
      void setup() override;
      void dump_config() override;

      // 新增：供 Python 设置各个通道的电阻值
      void set_resistor(uint8_t channel, float resistor) { this->resistor_[channel] = resistor; }

    protected:
      template <typename... Ts>
      friend class ResetEnergyAction;
      void reset_energy_();
      
      // 修改：将函数签名改为接受数据类型和通道索引
      void read_data_(uint8_t address, uint8_t data_type, uint8_t channel, sensor::Sensor *sensor);
      void calculate_power_factor_(sensor::Sensor *current_sensor, sensor::Sensor *voltage_sensor, sensor::Sensor *power_sensor, sensor::Sensor *power_factor_sensor);
      void bias_correction_(uint8_t address, float measurements, float correction, uint8_t channel);
      void gain_correction_(uint8_t address, float measurements, float correction, uint8_t channel);
      
      uint8_t current_channel_{0};
      size_t enqueue_action_(ActionCallbackFuncPtr function);
      void handle_actions_();

      // 新增：存储 10 个通道的电阻值，默认全为 5.1Ω
      float resistor_[10]{5.1, 5.1, 5.1, 5.1, 5.1, 5.1, 5.1, 5.1, 5.1, 5.1};

    private:
      std::vector<ActionCallbackFuncPtr> action_queue_{};
    };

    template <typename... Ts>
    class ResetEnergyAction : public Action<Ts...>, public Parented<BL0910>
    {
    public:
      void play(Ts... x) override { this->parent_->enqueue_action_(&BL0910::reset_energy_); }
    };

  } 
}
