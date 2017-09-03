/** Accus.cpp
  définitions de la classe Accus
  bloc accus NIMH
*/

#include "Accus.h"

//constructeur
Accus::Accus(  const boolean accuPresent, const byte accusPin) :
  AccusCommun( ),  m_accuPresent (accuPresent), m_accusPin(accusPin), m_batterieFaible(false)
{

}
//constructeur complet
Accus::Accus(  const boolean accuPresent, const byte accusPin, const float tensionMiniAccus, const int R1, const int R2, const float  Vref, const int maxCAD) :
  AccusCommun( tensionMiniAccus, R1, R2, Vref, maxCAD),  m_accuPresent (accuPresent), m_accusPin(accusPin), m_batterieFaible(false)
{

}

Accus::~Accus()
{
  
}


///-----return batterieFaible-----
void Accus::accusFaible() {
  if ((AccusCommun::lectureAccusCAD(m_accusPin) < AccusCommun::m_valMinCAD) and m_accuPresent) { /// si la batterie est faible
    m_batterieFaible = true; // batterie faible
  } else {
    m_batterieFaible = false;
  }
}

///------- convertion CAD  vers tension batterie -----
float Accus::tensionAccus() {
    return AccusCommun::tensionAccus(m_accusPin);
}

//-----accesseur - getter-----
boolean Accus::get_m_batterieFaible() {
  return m_batterieFaible;
}

//-----mutateur - setter-----
void Accus::set_m_batterieFaible(boolean batterieFaible) {
  m_batterieFaible = batterieFaible;
}



