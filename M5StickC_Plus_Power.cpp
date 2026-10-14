// SPDX-License-Identifier: EUPL-1.2-or-later
// Copyright (c) 2026 M5StickC Plus Power contributors

#include "wled.h"
#include <Wire.h>

// M5StickC Plus 1.1: enable external 5V via the AXP192 (I2C address 0x34).
// Uses WLED's shared, globally-configured I2C bus.
// In WLED Config -> Usermods, set global I2C SDA=21 and SCL=22.
// WLED itself initialises Wire and reserves the I2C pins via pinManager.

class M5StickCPlusPower : public Usermod {
private:
  bool enableRegisterBit(uint8_t reg, uint8_t mask) {
    Wire.beginTransmission(0x34);
    Wire.write(reg);

    if (Wire.endTransmission(false) != 0) return false;
    if (Wire.requestFrom((uint8_t)0x34, (uint8_t)1) != 1) return false;

    const uint8_t value = Wire.read();

    Wire.beginTransmission(0x34);
    Wire.write(reg);
    Wire.write(value | mask);

    return Wire.endTransmission() == 0;
  }

public:
  void setup() override {
    // AXP192 is wired to these fixed pins on M5StickC Plus 1.1.
    // Do not access I2C if WLED's shared bus is disabled or on other pins.
    if (i2c_sda != 21 || i2c_scl != 22) return;

    // Enable EXTEN, then external 5V boost (preserving other register bits).
    if (!enableRegisterBit(0x12, 0x40)) return;
    enableRegisterBit(0x10, 0x04);
  }

  void loop() override {
    // No continuous processing required.
  }
};

static M5StickCPlusPower m5StickPower;
REGISTER_USERMOD(m5StickPower);
