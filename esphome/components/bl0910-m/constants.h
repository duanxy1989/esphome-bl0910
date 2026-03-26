#pragma once
#include <cstdint>

namespace esphome
{
    namespace bl0910
    {
        // 动态转换系数计算函数 (基于各个通道的采样电阻 R)
        inline float get_iref(float r) { return 1.097 / (12875.0 * r); }
        inline float get_pref(float r) { return 120340.9 / (4041259.0 * r); }
        inline float get_eref(float r) { 
            return (4194304.0 * 0.032768 * 16.0) / (3600000.0 * 16.0 * (404125.0 * (r * 10.0) / 120340.9)); 
        }
        inline float get_ki(float r) { return (12875.0 * r) / 1.097; }

        // 静态固定系数
        static const float BL0910_UREF = 109700.0 / (1316200000); // Voltage
        static const float BL0910_FREF = 10000000.0; // Frequency

        // Register address
        // Voltage
        static const uint8_t BL0910_V_RMS = 0x16;

        // Current
        static const uint8_t BL0910_I_1_RMS = 0x0C;
        static const uint8_t BL0910_I_2_RMS = 0x0D;
        static const uint8_t BL0910_I_3_RMS = 0x0E;
        static const uint8_t BL0910_I_4_RMS = 0x0F;
        static const uint8_t BL0910_I_5_RMS = 0x10;
        static const uint8_t BL0910_I_6_RMS = 0x11;
        static const uint8_t BL0910_I_7_RMS = 0x12;
        static const uint8_t BL0910_I_8_RMS = 0x13;
        static const uint8_t BL0910_I_9_RMS = 0x14;
        static const uint8_t BL0910_I_10_RMS = 0x15;

        // Power
        static const uint8_t BL0910_WATT_1 = 0X22;
        static const uint8_t BL0910_WATT_2 = 0X23;
        static const uint8_t BL0910_WATT_3 = 0X24;
        static const uint8_t BL0910_WATT_4 = 0X25;
        static const uint8_t BL0910_WATT_5 = 0X26;
        static const uint8_t BL0910_WATT_6 = 0X27;
        static const uint8_t BL0910_WATT_7 = 0X28;
        static const uint8_t BL0910_WATT_8 = 0X29;
        static const uint8_t BL0910_WATT_9 = 0X2A;
        static const uint8_t BL0910_WATT_10 = 0X2B;
        static const uint8_t BL0910_WATT_SUM = 0X2C;

        // Active pulse count, unsigned
        static const uint8_t BL0910_CF_1_CNT = 0X2F;
        static const uint8_t BL0910_CF_2_CNT = 0X30;
        static const uint8_t BL0910_CF_3_CNT = 0X31;
        static const uint8_t BL0910_CF_4_CNT = 0X32;
        static const uint8_t BL0910_CF_5_CNT = 0X33;
        static const uint8_t BL0910_CF_6_CNT = 0X34;
        static const uint8_t BL0910_CF_7_CNT = 0X35;
        static const uint8_t BL0910_CF_8_CNT = 0X36;
        static const uint8_t BL0910_CF_9_CNT = 0X37;
        static const uint8_t BL0910_CF_10_CNT = 0X38;
        static const uint8_t BL0910_CF_SUM_CNT = 0X39;

        // Voltage frequency
        static const uint8_t BL0910_FREQUENCY = 0X4E;

        // Internal temperature
        static const uint8_t BL0910_TEMPERATURE = 0X5E;

        // Calibration register
        // RMS gain adjustment register
        static const uint8_t BL0910_RMSGN_1 = 0x6C;
        static const uint8_t BL0910_RMSGN_2 = 0x6D;
        static const uint8_t BL0910_RMSGN_3 = 0x6E;
        static const uint8_t BL0910_RMSGN_4 = 0x6F;
        static const uint8_t BL0910_RMSGN_5 = 0x70;
        static const uint8_t BL0910_RMSGN_6 = 0x71;
        static const uint8_t BL0910_RMSGN_7 = 0x72;
        static const uint8_t BL0910_RMSGN_8 = 0x73;
        static const uint8_t BL0910_RMSGN_9 = 0x74;
        static const uint8_t BL0910_RMSGN_10 = 0x75;

        // RMS offset correction register
        static const uint8_t BL0910_RMSOS_1 = 0x77;
        static const uint8_t BL0910_RMSOS_2 = 0x78;
        static const uint8_t BL0910_RMSOS_3 = 0x79;
        static const uint8_t BL0910_RMSOS_4 = 0x7A;
        static const uint8_t BL0910_RMSOS_5 = 0x7B;
        static const uint8_t BL0910_RMSOS_6 = 0x7C;
        static const uint8_t BL0910_RMSOS_7 = 0x7D;
        static const uint8_t BL0910_RMSOS_8 = 0x7E;
        static const uint8_t BL0910_RMSOS_9 = 0x7F;
        static const uint8_t BL0910_RMSOS_10 = 0x80;

        // Active power gain adjustment register
        static const uint8_t BL0910_WATTGN_1 = 0xB6;
        static const uint8_t BL0910_WATTGN_2 = 0xB7;
        static const uint8_t BL0910_WATTGN_3 = 0xB8;
        static const uint8_t BL0910_WATTGN_4 = 0xB9;
        static const uint8_t BL0910_WATTGN_5 = 0xBA;
        static const uint8_t BL0910_WATTGN_6 = 0xBB;
        static const uint8_t BL0910_WATTGN_7 = 0xBC;
        static const uint8_t BL0910_WATTGN_8 = 0xBD;
        static const uint8_t BL0910_WATTGN_9 = 0xBE;
        static const uint8_t BL0910_WATTGN_10 = 0xBF;

        // User write protection setting register
        static const uint8_t BL0910_USR_WRPROT = 0x9E;
        // Reset Register
        static const uint8_t BL0910_SOFT_RESET = 0x9F;
        
        static const uint8_t BL0910_READ_COMMAND = 0x35; 
        static const uint8_t BL0910_WRITE_COMMAND = 0xCA;
        
        const uint8_t BL0910_INIT[2][6] = {
            {BL0910_WRITE_COMMAND, BL0910_SOFT_RESET, 0x5A, 0x5A, 0x5A, 0x52},
            {BL0910_WRITE_COMMAND, BL0910_USR_WRPROT, 0x55, 0x55, 0x00, 0xB7}
        };
        static const uint8_t USR_WRPROT_WITABLE[6] = {0xCA, 0x9E, 0x55, 0x55, 0x00, 0xB7};
        static const uint8_t USR_SOFT_RESET[6] = {0xCA, 0x9F, 0x5A, 0x5A, 0x5A, 0x52};
        static const uint8_t USR_WRPROT_ONLYREAD[6] = {0xCA, 0x9E, 0x00, 0x00, 0x00, 0x61};

    } 
}
