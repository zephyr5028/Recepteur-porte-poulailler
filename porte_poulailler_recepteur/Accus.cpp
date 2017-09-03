/** Accus.cpp
  définitions de la classe Accus
  bloc accus NIMH
*/

#include "Accus.h"

//constructeur avec debug
Accus::Accus(  const boolean accuPresent, const byte accusPin, const float tensionMiniAccus, const int R1, const int R2, const float  Vref, const int maxCAD, const boolean debug) :
  m_accuPresent (accuPresent), m_accusPin(accusPin), m_tensionMiniAccus(tensionMiniAccus), m_R1(R1), m_R2(R2), m_Vref(Vref), m_maxCAD(maxCAD), m_debug(debug), m_valMinCAD(668), m_batterieFaible(false)
{
}

Accus::~Accus()
{
}

///-----initialisation-----
void Accus::init () {
  //utilisation d'un pont de resistances : vout = vin * R2 / R1 + R2
  float vout = (m_tensionMiniAccus * m_R2) / (m_R1 + m_R2) ;
  m_valMinCAD = (vout * m_maxCAD) / m_Vref ;// calcul de la valeur CAd pour la tension minimale
}

///------- lecture tension batterie CAD-----
int Accus::lectureAccusCAD() {
  int lectureAccus = analogRead(m_accusPin); //read the input on analog pin tension batterie
  return lectureAccus;
}

///-----return batterieFaible-----
void Accus::accusFaible() {
  if ((lectureAccusCAD() < m_valMinCAD) and m_accuPresent) { /// si la batterie est faible
   m_batterieFaible = true; // batterie faible
  } else {
    m_batterieFaible = false;
  }
}

///------- convertion CAD  vers tension batterie -----
float Accus::tensionAccus() {
  //utilisation d'un pont de resistances : vout = vin * R2 / R1 + R2
  float vout = (lectureAccusCAD() * m_Vref) / m_maxCAD;
  float vin = (vout * (m_R1 + m_R2)) / m_R2;
  // vin : tension de l'accu
  return vin;
}


//-----accesseur - getter-----
boolean Accus::get_m_batterieFaible() {
  return m_batterieFaible;
}

//-----mutateur - setter-----
void Accus::set_m_batterieFaible(boolean batterieFaible) {
  m_batterieFaible = batterieFaible;
}



