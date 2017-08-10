/* LumBH1750.h
  définitions de la classe LumBH1750
  shield BH1750 - capteur de lumière
*/

/*
  BH1750 can be physically configured to use two I2C addresses:
    - 0x23 (most common) (if ADD pin had < 0.7VCC voltage)
    - 0x5C (if ADD pin had > 0.7VCC voltage)
  Library use 0x23 address as default, but you can define any other address.
  If you had troubles with default value - try to change it to 0x5C.
*/

#include "LumBH1750.h"

LumBH1750::LumBH1750(const byte BH1750_I2C_ADDRESS, boolean debug) : BH1750(BH1750_I2C_ADDRESS),  m_debug(debug)
{

}

LumBH1750::~LumBH1750()
{

}

 /*
    BH1750 had six different measurment modes. They are divided in two groups -
    continuous and one-time measurments. In continuous mode, sensor continuously
    measures lightness value. And in one-time mode, sensor makes only one
    measurment, and going to Power Down mode after this.
    Each mode, has three different precisions:
      - Low Resolution Mode - (4 lx precision, 16ms measurment time)
      - High Resolution Mode - (1 lx precision, 120ms measurment time)
      - High Resolution Mode 2 - (0.5 lx precision, 120ms measurment time)
    By default, library use Continuous High Resolution Mode, but you can set
    any other mode, by define it to BH1750.begin() or BH1750.configure() functions.
    [!] Remember, if you use One-Time mode, your sensor will go to Power Down mode
    each time, when it completes measurment and you've read it.
    Full mode list:
      BH1750_CONTINUOUS_LOW_RES_MODE
      BH1750_CONTINUOUS_HIGH_RES_MODE (default)
      BH1750_CONTINUOUS_HIGH_RES_MODE_2
      BH1750_ONE_TIME_LOW_RES_MODE
      BH1750_ONE_TIME_HIGH_RES_MODE
      BH1750_ONE_TIME_HIGH_RES_MODE_2
  */
///-----initialisation-----
void LumBH1750::init(uint8_t mode) {
  begin(mode);
}

