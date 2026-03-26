#include "bl0910.h"
#include "constants.h"
#include <queue>
#include "esphome/core/log.h"

namespace esphome
{
  namespace bl0910
  {
    static const char *const TAG = "bl0910";
    constexpr uint32_t to_uint32_t(ube24_t input) { return input.h << 16 | input.m << 8 | input.l; }
    constexpr int32_t to_int32_t(sbe24_t input) { return input.h << 16 | input.m << 8 | input.l; }
    constexpr uint8_t bl0910_checksum(const uint8_t address, const DataPacket *data)
    {
      return (address + data->l + data->m + data->h) ^ 0xFF;
    }

    void BL0910::loop()
    {
      if (this->current_channel_ == UINT8_MAX) return;

      this->flush();

      // 注意：channel_ 参数传入索引 0-9 对应 1-10 通道
      switch (this->current_channel_)
      {
      case 0:
        this->read_data_(BL0910_TEMPERATURE, TYPE_TEMPERATURE, 0, this->temperature_sensor_); 
        break;
      case 1:
        this->read_data_(BL0910_I_1_RMS, TYPE_CURRENT, 0, this->current_1_sensor_);
        this->read_data_(BL0910_WATT_1, TYPE_POWER, 0, this->power_1_sensor_);
        this->read_data_(BL0910_CF_1_CNT, TYPE_ENERGY, 0, this->energy_1_sensor_);
        this->calculate_power_factor_(this->current_1_sensor_, this->voltage_sensor_, this->power_1_sensor_, this->power_factor_1_sensor_);
        break;
      case 2:
        this->read_data_(BL0910_I_2_RMS, TYPE_CURRENT, 1, this->current_2_sensor_);
        this->read_data_(BL0910_WATT_2, TYPE_POWER, 1, this->power_2_sensor_);
        this->read_data_(BL0910_CF_2_CNT, TYPE_ENERGY, 1, this->energy_2_sensor_);
        this->calculate_power_factor_(this->current_2_sensor_, this->voltage_sensor_, this->power_2_sensor_, this->power_factor_2_sensor_);
        break;
      case 3:
        this->read_data_(BL0910_I_3_RMS, TYPE_CURRENT, 2, this->current_3_sensor_);
        this->read_data_(BL0910_WATT_3, TYPE_POWER, 2, this->power_3_sensor_);
        this->read_data_(BL0910_CF_3_CNT, TYPE_ENERGY, 2, this->energy_3_sensor_);
        this->calculate_power_factor_(this->current_3_sensor_, this->voltage_sensor_, this->power_3_sensor_, this->power_factor_3_sensor_);
        break;
      case 4:
        this->read_data_(BL0910_I_4_RMS, TYPE_CURRENT, 3, this->current_4_sensor_);
        this->read_data_(BL0910_WATT_4, TYPE_POWER, 3, this->power_4_sensor_);
        this->read_data_(BL0910_CF_4_CNT, TYPE_ENERGY, 3, this->energy_4_sensor_);
        this->calculate_power_factor_(this->current_4_sensor_, this->voltage_sensor_, this->power_4_sensor_, this->power_factor_4_sensor_);
        break;
      case 5:
        this->read_data_(BL0910_I_5_RMS, TYPE_CURRENT, 4, this->current_5_sensor_);
        this->read_data_(BL0910_WATT_5, TYPE_POWER, 4, this->power_5_sensor_);
        this->read_data_(BL0910_CF_5_CNT, TYPE_ENERGY, 4, this->energy_5_sensor_);
        this->calculate_power_factor_(this->current_5_sensor_, this->voltage_sensor_, this->power_5_sensor_, this->power_factor_5_sensor_);
        break;
      case 6:
        this->read_data_(BL0910_I_6_RMS, TYPE_CURRENT, 5, this->current_6_sensor_);
        this->read_data_(BL0910_WATT_6, TYPE_POWER, 5, this->power_6_sensor_);
        this->read_data_(BL0910_CF_6_CNT, TYPE_ENERGY, 5, this->energy_6_sensor_);
        this->calculate_power_factor_(this->current_6_sensor_, this->voltage_sensor_, this->power_6_sensor_, this->power_factor_6_sensor_);
        break;
      case 7:
        this->read_data_(BL0910_I_7_RMS, TYPE_CURRENT, 6, this->current_7_sensor_);
        this->read_data_(BL0910_WATT_7, TYPE_POWER, 6, this->power_7_sensor_);
        this->read_data_(BL0910_CF_7_CNT, TYPE_ENERGY, 6, this->energy_7_sensor_);
        this->calculate_power_factor_(this->current_7_sensor_, this->voltage_sensor_, this->power_7_sensor_, this->power_factor_7_sensor_);
        break;
      case 8:
        this->read_data_(BL0910_I_8_RMS, TYPE_CURRENT, 7, this->current_8_sensor_);
        this->read_data_(BL0910_WATT_8, TYPE_POWER, 7, this->power_8_sensor_);
        this->read_data_(BL0910_CF_8_CNT, TYPE_ENERGY, 7, this->energy_8_sensor_);
        this->calculate_power_factor_(this->current_8_sensor_, this->voltage_sensor_, this->power_8_sensor_, this->power_factor_8_sensor_);
        break;
      case 9:
        this->read_data_(BL0910_I_9_RMS, TYPE_CURRENT, 8, this->current_9_sensor_);
        this->read_data_(BL0910_WATT_9, TYPE_POWER, 8, this->power_9_sensor_);
        this->read_data_(BL0910_CF_9_CNT, TYPE_ENERGY, 8, this->energy_9_sensor_);
        this->calculate_power_factor_(this->current_9_sensor_, this->voltage_sensor_, this->power_9_sensor_, this->power_factor_9_sensor_);
        break;
      case 10:
        this->read_data_(BL0910_I_10_RMS, TYPE_CURRENT, 9, this->current_10_sensor_);
        this->read_data_(BL0910_WATT_10, TYPE_POWER, 9, this->power_10_sensor_);
        this->read_data_(BL0910_CF_10_CNT, TYPE_ENERGY, 9, this->energy_10_sensor_);
        this->calculate_power_factor_(this->current_10_sensor_, this->voltage_sensor_, this->power_10_sensor_, this->power_factor_10_sensor_);
        break;
      case (UINT8_MAX - 2):
        this->read_data_(BL0910_FREQUENCY, TYPE_FREQUENCY, 0, this->frequency_sensor_); 
        this->read_data_(BL0910_V_RMS, TYPE_VOLTAGE, 0, this->voltage_sensor_);       
        break;
      case (UINT8_MAX - 1):
        // 对于硬件总功率/电量寄存器，默认应用通道 1 (索引 0) 的电阻基准
        this->read_data_(BL0910_WATT_SUM, TYPE_TOTAL_POWER, 0, this->total_power_sensor_);  
        this->read_data_(BL0910_CF_SUM_CNT, TYPE_TOTAL_ENERGY, 0, this->total_energy_sensor_); 
        break;
      default:
        this->current_channel_ = UINT8_MAX - 2; 
        return;
      }
      this->current_channel_++;
      this->handle_actions_();
    }

    void BL0910::setup()
    {
      this->flush();                                                     
      this->write_array(USR_SOFT_RESET, sizeof(USR_SOFT_RESET));         
      this->write_array(USR_WRPROT_WITABLE, sizeof(USR_WRPROT_WITABLE)); 

      for(int i = 0; i < 10; i++) {
         this->bias_correction_(BL0910_RMSOS_1 + i, 0, 0, i);
         this->gain_correction_(BL0910_RMSGN_1 + i, 1, 1, i);
      }
    }

    void BL0910::update() { this->current_channel_ = 0; }

    std::queue<ActionCallbackFuncPtr> enqueue_action_;
    size_t BL0910::enqueue_action_(ActionCallbackFuncPtr function)
    {
      this->action_queue_.push_back(function);
      return this->action_queue_.size();
    }

    void BL0910::handle_actions_()
    {
      if (this->action_queue_.empty()) return;
      ActionCallbackFuncPtr ptr_func = nullptr;
      for (int i = 0; i < this->action_queue_.size(); i++)
      {
        ptr_func = this->action_queue_[i];
        if (ptr_func)
        {
          ESP_LOGI(TAG, "HandleActionCallback[%d]...", i);
          (this->*ptr_func)();
        }
      }
      while (this->available()) { this->read(); }
      this->action_queue_.clear();
      this->flush();
    }

    void BL0910::reset_energy_()
    {
      this->write_array(BL0910_INIT[0], 6);
      delay(1);
      this->flush();
      ESP_LOGW(TAG, "Device reset with init command.");
    }

    // 重构的读取函数：引入类型判断和动态电阻倍率
    void BL0910::read_data_(const uint8_t address, const uint8_t data_type, const uint8_t channel, sensor::Sensor *sensor)
    {
      if (sensor == nullptr) return;

      DataPacket buffer;
      ube24_t data_u24;
      sbe24_t data_s24;
      float value = 0;

      bool signed_result = (data_type == TYPE_TEMPERATURE || data_type == TYPE_POWER || data_type == TYPE_TOTAL_POWER);
      this->flush();
      this->write_byte(BL0910_READ_COMMAND);
      this->write_byte(address);

      if (this->read_array((uint8_t *)&buffer, sizeof(buffer) - 1))
      {
        if (bl0910_checksum(address, &buffer) == buffer.checksum)
        {
          if (signed_result) {
            data_s24.l = buffer.l; data_s24.m = buffer.m; data_s24.h = buffer.h;
          } else {
            data_u24.l = buffer.l; data_u24.m = buffer.m; data_u24.h = buffer.h;
          }
        }
        else
        {
          ESP_LOGW(TAG, "Checksum failed. Discarding message.");
          return;
        }
      } 
      else 
      {
        // 修复原版 Bug：读取失败时直接返回，而不是发送 0 值
        return;
      }

      // 获取当前通道在 YAML 中配置的电阻值
      float r = this->resistor_[channel];

      // 根据数据类型，应用对应的动态乘子
      switch (data_type)
      {
      case TYPE_VOLTAGE:
        value = (float)to_uint32_t(data_u24) * BL0910_UREF;
        break;
      case TYPE_CURRENT:
        value = (float)to_uint32_t(data_u24) * get_iref(r);
        break;
      case TYPE_POWER:
        value = (float)to_int32_t(data_s24) * get_pref(r);
        break;
      case TYPE_ENERGY:
        value = (float)to_uint32_t(data_u24) * get_eref(r);
        break;
      case TYPE_TOTAL_POWER:
        value = (float)to_int32_t(data_s24) * get_pref(r) * 16.0;
        break;
      case TYPE_TOTAL_ENERGY:
        value = (float)to_uint32_t(data_u24) * get_eref(r) * 16.0;
        break;
      case TYPE_FREQUENCY:
        value = BL0910_FREF / (float)to_uint32_t(data_u24);
        break;
      case TYPE_TEMPERATURE:
        value = (float)to_int32_t(data_s24);
        value = (value - 64.0) * 12.5 / 59.0 - 40.0;
        break;
      }
      sensor->publish_state(value);
    }

    void BL0910::calculate_power_factor_(sensor::Sensor *current_sensor, sensor::Sensor *voltage_sensor, sensor::Sensor *power_sensor, sensor::Sensor *power_factor_sensor)
    {
      if (current_sensor != nullptr && voltage_sensor != nullptr && power_sensor != nullptr && power_factor_sensor != nullptr)
      {
        float power_factor = (current_sensor->state * voltage_sensor->state) / power_sensor->state;
        power_factor_sensor->publish_state(power_factor);
      }
    }

    void BL0910::bias_correction_(uint8_t address, float measurements, float correction, uint8_t channel)
    {
      DataPacket data;
      float ki = get_ki(this->resistor_[channel]);
      float i_rms0 = measurements * ki;
      float i_rms = correction * ki;
      int32_t value = (i_rms * i_rms - i_rms0 * i_rms0) / 256; 
      data.l = (value >> 0) & 0xFF;
      data.m = (value >> 8) & 0xFF;
      data.h = (value >> 16) & 0xFF;
      data.checksum = bl0910_checksum(address, &data);
      this->flush();
      this->write_byte(BL0910_WRITE_COMMAND);
      this->write_byte(address);
      this->write_array((uint8_t *)&data, sizeof(data));
    }

    void BL0910::gain_correction_(uint8_t address, float measurements, float correction, uint8_t channel)
    {
      DataPacket data;
      float ki = get_ki(this->resistor_[channel]);
      float i_rms0 = measurements * ki;
      float i_rms = correction * ki;
      int32_t value = int((i_rms / i_rms0 - 1) * 65536); 
      data.l = (value >> 0) & 0xFF;
      data.m = (value >> 8) & 0xFF;
      data.h = (value >> 16) & 0xFF;
      data.checksum = bl0910_checksum(address, &data);
      this->flush();
      this->write_byte(BL0910_WRITE_COMMAND);
      this->write_byte(address);
      this->write_array((uint8_t *)&data, sizeof(data));
    }

    void BL0910::dump_config()
    {
      ESP_LOGCONFIG(TAG, "BL0910:");
      LOG_SENSOR("  ", "Voltage", this->voltage_sensor_);
      // ... 保持原有不变，节约篇幅 ...
    }

  } 
}
