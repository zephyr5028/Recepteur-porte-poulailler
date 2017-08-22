/** Lumiere.cpp
  définitions de la classe Lumiere
  LDR
*/

#include "Lumiere.h"

//constructeur avec debug
Lumiere::Lumiere( const byte lumierePin, const int R2, const float  Vref, const int maxCAD, const boolean debug) :
  m_lumierePin(lumierePin), m_R2(R2), m_Vref(Vref), m_maxCAD(maxCAD), m_debug(debug)
{
  
}

Lumiere::~Lumiere()
{
  
}

///------- lecture luminosite CAD-----
int Lumiere::luminositeCAD() {
  int valLumiere = analogRead(m_lumierePin); //read the input on analog pin tension batterie
  return valLumiere;
}

///------- convertion CAD  vers  LDR ( resitance ou lux )-----
unsigned int Lumiere::tensionLuminosite() {
  float resistorVoltage = (luminositeCAD() * m_Vref) / m_maxCAD;
  // utilisation d'un pont de resistances : vout = vin * R2 / R1 + R2
  float ldrVoltage = m_Vref - resistorVoltage;
   unsigned int ldrResistance = (ldrVoltage * m_R2) / resistorVoltage;  
  // courbe lux / resistance de la LDR
  float LUX_CALC_SCALAR = 12518931;
  float LUX_CALC_EXPONENT = -1.28;
  int voltage = LUX_CALC_SCALAR * pow(ldrResistance, LUX_CALC_EXPONENT);
  return voltage;
}



