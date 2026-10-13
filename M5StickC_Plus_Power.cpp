#include "wled.h"
#include <Wire.h>

// M5StickC Plus 1.1 - AXP192 external 5V power

class M5StickCPlusPower : public Usermod {

private:
  bool enableRegisterBit(uint8_t reg, uint8_t mask) {
    Wire1.beginTransmission(0x34);
    Wire1.write(reg);

    if (Wire1.endTransmission(false) != 0) return false;
    if (Wire1.requestFrom((uint8_t)0x34, (uint8_t)1) != 1)
      return false;

    uint8_t value = Wire1.read();

    Wire1.beginTransmission(0x34);
    Wire1.write(reg);
    Wire1.write(value | mask);

    return Wire1.endTransmission() == 0;
  }

public:
  void setup() override {
    // AXP192 uses GPIO21 (SDA) and GPIO22 (SCL)
    Wire1.begin(21, 22);

    // Enable EXTEN power output
    enableRegisterBit(0x12, 0x40);

    // Enable external 5V boost converter
    enableRegisterBit(0x10, 0x04);
  }

  void loop() override {
    // Nothing required
  }
};

static M5StickCPlusPower m5StickPower;
REGISTER_USERMOD(m5StickPower);
