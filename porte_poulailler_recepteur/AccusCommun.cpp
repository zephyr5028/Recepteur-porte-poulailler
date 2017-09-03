/** AccusCommun.cpp
  définitions de la classe AccusCommun
  bloc accus NIMH
*/

#include "AccusCommun.h"

//constructeur
AccusCommun::AccusCommun() : m_tensionMiniAccus(4.8), m_R1(4700), m_R2(10000), m_Vref(5.00), m_maxCAD(1023), m_valMinCAD(668)
{
}
//constructeur complet
AccusCommun::AccusCommun( const float tensionMiniAccus, const int R1, const int R2, const float  Vref, const int maxCAD) :
  m_tensionMiniAccus(tensionMiniAccus), m_R1(R1), m_R2(R2), m_Vref(Vref), m_maxCAD(maxCAD), m_valMinCAD(668)
{
}

AccusCommun::~AccusCommun()
{
}

///-----initialisation-----
void AccusCommun::init () {
  //utilisation d'un pont de resistances : vout = vin * R2 / R1 + R2
  float tensionPont = (m_tensionMiniAccus * m_R2) / (m_R1 + m_R2) ;
  m_valMinCAD = (tensionPont * m_maxCAD) / m_Vref ;// calcul de la valeur CAd pour la tension minimale
}

///------- lecture tension batterie CAD-----
int AccusCommun::lectureAccusCAD(const byte accusPin) {
  return  analogRead(accusPin); //read the input on analog pin tension batterie
}

///------- convertion CAD  vers tension batterie -----
float AccusCommun::tensionAccus(const byte accusPin) {
  //utilisation d'un pont de resistances : vout = vin * R2 / R1 + R2
  float tensionPont = (lectureAccusCAD(accusPin) * m_Vref) / m_maxCAD;
  float tensionAccu = (tensionPont * (m_R1 + m_R2)) / m_R2;
  return tensionAccu;
}



