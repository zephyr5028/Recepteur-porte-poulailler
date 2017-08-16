/** Accus.cpp
  définitions de la classe Accus
  bloc accus NIMH
*/

#include "Accus.h"

//constructeur avec debug
Accus::Accus(  const byte accusPin, const float tensionMiniAccus, const float rapportConvertion, const boolean debug) :
  m_accusPin(accusPin), m_tensionMiniAccus(tensionMiniAccus), m_rapportConvertion(rapportConvertion), m_debug(debug), m_maxCAD(1023)
{
}

Accus::~Accus()
{
}

/**
   \brief teste si la tension est faible
   \details Lecture et conversion de la valeur mesurée sur un pont de résistances en parallèle de l'accu
   \param m_accusPin numéro de l'entrée analogique
   \return batterieFaible si < 4,8v
*/
bool Accus::accusFaible() {
  bool batterieFaible;
  int convertMiniTension = (m_maxCAD * m_tensionMiniAccus) / m_rapportConvertion; /// convertion tension mini en pas du CAD
  int valAccus = analogRead(m_accusPin); /// read the input on analog pin  tension batterie
  if (valAccus < convertMiniTension) { /// si la batterie est faible < 4,8v (654)
    return batterieFaible = true;
  } else {
    return batterieFaible = false;
  }
}

///------- lecture tension batterie CAD-----
int Accus::tensionAccusCAD() {
  int valAccus = analogRead(m_accusPin); //read the input on analog pin tension batterie
  //Serial.print (m_accusPin);
  //Serial.print ("  ");
  //Serial.println (valAccus);
  return valAccus;
}

///------- convertion CAD  vers tension batterie -----
float Accus::tensionAccus(int valAccus) {
  // Convert the analog reading (which goes from 0 - 1023) to a voltage about (0 - 6V)
  /*http://www.mon-club-elec.fr/pmwiki_reference_arduino/pmwiki.php?n=Main.Map
     L'instruction map() utilise des valeurs entières qui ne peuvent fournir les décimales, alors que les calculs le devraient. La partie décimale est tronquée, et les valeurs ne sont pas arrondies ou moyennées.
    Syntaxe
    map (valeur, limite_basse_source, limite_haute_source, limite_basse_destination, limite_haute_destination)
    Paramètres
    valeur : le nombre à ré-étalonner
    limite_basse_source: la valeur de la limite inférieure de la fourchette de départ
    limite_haute_source: la valeur de la limite supérieure de la fourchette de départ
    limite_basse_destination: la valeur de la limite inférieure de la fourchette de destination
    limite_haute_destination: la valeur de la limite supérieure de la fourchette de destination
    Valeur renvoyée
    La valeur ré-étalonnée
  */
  float voltage = map (valAccus, 0, 1023, 0, m_rapportConvertion);
  voltage /= 100;
  // pour le calcul de la variable ACCUS_CONVERSION_RAPPORT_ACCUS_Nx = tension batterie mesuree multimetre * 100 * 1023 / valAccus
  return voltage;
}

///-----lecture et convertion vers tension batterie en float-----
float Accus::tensionAccusCADversFloat() {
  float volt = tensionAccus(tensionAccusCAD());
  return volt;
}


