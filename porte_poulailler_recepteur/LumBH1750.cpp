/* LumBH1750.h
  définitions de la classe LumBH1750
  shield BH1750 - capteur de lumière
*/

#include "LumBH1750.h"

LumBH1750::LumBH1750() : BH1750(), m_debug(false)
{

}

LumBH1750::~LumBH1750() {

}

///-----initialisation-----
void LumBH1750::init() {
  //BH1750_Init(BH1750_I2C_ADDRESS);
  byte buff[2] = {};
}

