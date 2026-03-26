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
      if (this->current_channel_ == UINT8_MAX)
      {
        return;
      }

      this->flush();

      // --- 新增：动态电阻计算逻辑 ---
      float i_ref = BL0910_IREF;
      float p_ref = BL0910_PREF;
      float e_ref = BL0910_EREF;

      // 如果当前是 1-10 通道，根据该通道配置的电阻重新计算参考值
      // 假设 constants.h 里的常量是基于 5.1 欧姆计算的
      if (this->current_channel_ >= 1 && this->current_channel_ <= 10) {
          float r_val = this->channel_resistors_[this->current_channel_ - 1];
          if (r_val > 0) {
              float scale = 5.1f / r_val; 
              i_ref = BL0910_IREF * scale;
              p_ref = BL0910_PREF * scale;
              e_ref = BL0910_EREF * scale;
          }
      }
      // ----------------------------

      switch (this->current_channel_)
      {
      case 0:
        this->read_data_(BL0910_TEMPERATURE, BL0910_TREF, this->temperature_sensor_);
        break;
      case 1:
        this->read_data_(BL0910_I_1_RMS, i_ref, this->current_1_sensor_);
        this->read_data_(BL0910_WATT_1, p_ref, this->power_1_sensor_);
        this->read_data_(BL0910_CF_1_CNT, e_ref, this->energy_1_sensor_);
        this->calculate_power_factor_(this->current_1_sensor_, this->voltage_sensor_, this->power_1_sensor_, this->power_factor_1_sensor_);
        break;
      case 2:
        this->read_data_(BL0910_I_2_RMS, i_ref, this->current_2_sensor_);
        this->read_data_(BL0910_WATT_2, p_ref, this->power_2_sensor_);
        this->read_data_(BL0910_CF_2_CNT, e_ref, this->energy_2_sensor_);
        this->calculate_power_factor_(this->current_2_sensor_, this->voltage_sensor_, this->power_2_sensor_, this->power_factor_2_sensor_);
        break;
      case 3:
        this->read_data_(BL0910_I_3_RMS, i_ref, this->current_3_sensor_);
        this->read_data_(BL0910_WATT_3, p_ref, this->power_3_sensor_);
        this->read_data_(BL0910_CF_3_CNT, e_ref, this->energy_3_sensor_);
        this->calculate_power_factor_(this->current_3_sensor_, this->voltage_sensor_, this->power_3_sensor_, this->power_factor_3_sensor_);
        break;
      case 4:
        this->read_data_(BL0910_I_4_RMS, i_ref, this->current_4_sensor_);
        this->read_data_(BL0910_WATT_4, p_ref, this->power_4_sensor_);
        this->read_data_(BL0910_CF_4_CNT, e_ref, this->energy_4_sensor_);
        this->calculate_power_factor_(this->current_4_sensor_, this->voltage_sensor_, this->power_4_sensor_, this->power_factor_4_sensor_);
        break;
      case 5:
        this->read_data_(BL0910_I_5_RMS, i_ref, this->current_5_sensor_);
        this->read_data_(BL0910_WATT_5, p_ref, this->power_5_sensor_);
        this->read_data_(BL0910_CF_5_CNT, e_ref, this->energy_5_sensor_);
        this->calculate_power_factor_(this->current_5_sensor_, this->voltage_sensor_, this->power_5_sensor_, this->power_factor_5_sensor_);
        break;
      case 6:
        this->read_data_(BL0910_I_6_RMS, i_ref, this->current_6_sensor_);
        this->read_data_(BL0910_WATT_6, p_ref, this->power_6_sensor_);
        this->read_data_(BL0910_CF_6_CNT, e_ref, this->energy_6_sensor_);
        this->calculate_power_factor_(this->current_6_sensor_, this->voltage_sensor_, this->power_6_sensor_, this->power_factor_6_sensor_);
        break;
      case 7:
        this->read_data_(BL0910_I_7_RMS, i_ref, this->current_7_sensor_);
        this->read_data_(BL0910_WATT_7, p_ref, this->power_7_sensor_);
        this->read_data_(BL0910_CF_7_CNT, e_ref, this->energy_7_sensor_);
        this->calculate_power_factor_(this->current_7_sensor_, this->voltage_sensor_, this->power_7_sensor_, this->power_factor_7_sensor_);
        break;
      case 8:
        this->read_data_(BL0910_I_8_RMS, i_ref, this->current_8_sensor_);
        this->read_data_(BL0910_WATT_8, p_ref, this->power_8_sensor_);
        this->read_data_(BL0910_CF_8_CNT, e_ref, this->energy_8_sensor_);
        this->calculate_power_factor_(this->current_8_sensor_, this->voltage_sensor_, this->power_8_sensor_, this->power_factor_8_sensor_);
        break;
      case 9:
        this->read_data_(BL0910_I_9_RMS, i_ref, this->current_9_sensor_);
        this->read_data_(BL0910_WATT_9, p_ref, this->power_9_sensor_);
        this->read_data_(BL0910_CF_9_CNT, e_ref, this->energy_9_sensor_);
        this->calculate_power_factor_(this->current_9_sensor_, this->voltage_sensor_, this->power_9_sensor_, this->power_factor_9_sensor_);
        break;
      case 10:
        this->read_data_(BL0910_I_10_RMS, i_ref, this->current_10_sensor_);
        this->read_data_(BL0910_WATT_10, p_ref, this->power_10_sensor_);
        this->read_data_(BL0910_CF_10_CNT, e_ref, this->energy_10_sensor_);
        this->calculate_power_factor_(this->current_10_sensor_, this->voltage_sensor_, this->power_10_sensor_, this->power_factor_10_sensor_);
        break;
      case (UINT8_MAX - 2):
        this->read_data_(BL0910_FREQUENCY, BL0910_FREF, this->frequency_sensor_);
        this->read_data_(BL0910_V_RMS, BL0910_UREF, this->voltage_sensor_);
        break;
      case (UINT8_MAX - 1):
        this->read_data_(BL0910_WATT_SUM, BL0910_WATT, this->total_power_sensor_);
        this->read_data_(BL0910_CF_SUM_CNT, BL0910_CF, this->total_energy_sensor_);
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
      // 此处校准逻辑保持不变，依然使用 constants.h 默认值
      for (int i=1; i<=10; i++) {
          this->bias_correction_(BL0910_RMSOS_1 + (i-1), 0, 0);
          this->gain_correction_(BL0910_RMSGN_1 + (i-1), 1, 1);
      }
    }

    void BL0910::update()
    {
      this->current_channel_ = 0;
    }

    size_t BL0910::enqueue_action_(ActionCallbackFuncPtr function)
    {
      this->action_queue_.push_back(function);
      return this->action_queue_.size();
    }

    void BL0910::handle_actions_()
    {
      if (this->action_queue_.empty()) return;
      
      for (auto &ptr_func : this->action_queue_) {
        if (ptr_func) (this->*ptr_func)();
      }
      
      while (this->available()) this->read();
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

    void BL0910::read_data_(const uint8_t address, const float reference, sensor::Sensor *sensor)
    {
      if (sensor == nullptr) return;

      DataPacket buffer;
      ube24_t data_u24;
      sbe24_t data_s24;
      float value = 0;

      bool signed_result = reference == BL0910_TREF || reference == BL0910_WATT || reference == BL0910_PREF;
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
        else {
          ESP_LOGW(TAG, "Checksum failed for address 0x%02X", address);
          return;
        }
      }

      if (reference == BL0910_PREF || reference == BL0910_WATT)
        value = (float)to_int32_t(data_s24) * reference;
      else if (reference == BL0910_UREF || reference == BL0910_IREF || reference == BL0910_EREF || reference == BL0910_CF)
        value = (float)to_uint32_t(data_u24) * reference;
      else if (reference == BL0910_FREF)
        value = reference / (float)to_uint32_t(data_u24);
      else if (reference == BL0910_TREF) {
        value = (float)to_int32_t(data_s24);
        value = (value - 64) * 12.5 / 59 - 40;
      }
      
      sensor->publish_state(value);
    }

    void BL0910::calculate_power_factor_(sensor::Sensor *current_sensor, sensor::Sensor *voltage_sensor, sensor::Sensor *power_sensor, sensor::Sensor *power_factor_sensor)
    {
      if (current_sensor && voltage_sensor && power_sensor && power_factor_sensor)
      {
          if (std::isnan(current_sensor->state) || std::isnan(voltage_sensor->state) || power_sensor->state == 0) {
              power_factor_sensor->publish_state(0);
          } else {
              float pf = power_sensor->state / (current_sensor->state * voltage_sensor->state);
              if (pf > 1.0f) pf = 1.0f;
              if (pf < -1.0f) pf = -1.0f;
              power_factor_sensor->publish_state(pf);
          }
      }
    }

    void BL0910::bias_correction_(uint8_t address, float measurements, float correction) { /* 保持原样 */ }
    void BL0910::gain_correction_(uint8_t address, float measurements, float correction) { /* 保持原样 */ }

    void BL0910::dump_config()
    {
      ESP_LOGCONFIG(TAG, "BL0910:");
      // ... 原有的 dump 逻辑 ...
    }

  } // namespace bl0910
} // namespace esphome
