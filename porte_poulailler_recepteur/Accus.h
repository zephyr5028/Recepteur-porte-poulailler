/**
    \file Accus.h
    \class Accus
    \brief Gestion des blocs accumulateurs NIMH de 4,8v
    \details Suivi de la tension et la valeur minimum
    \author Zephyr5028
    \date hiver 2017
Prototype de la classe Accus
*/

#ifndef Accus_h
#define Accus_h

#include "Arduino.h"

class Accus
{

  public:
    // constructeur avec debug
    Accus( const byte accusPin, const float tensionMiniAccus = 4.8, const float rapportConvertion = 690, boolean debug = false );
    ~Accus(); // destructeur

    bool accusFaible();//batterie cdes < 4 volt
    int tensionAccusCAD(); // tension de l'accu CAD
    float tensionAccus(int valAccus); // convertion tension CAD tension de l'accu
    float tensionAccusCADversFloat();// lecture et convertion vers tension batterie en float



  protected:
    const boolean m_debug ; // pour affichage console si nécessaire
    const byte m_accusPin; //analog pin - tension batterie
    // Convert the analog reading (which goes from 0 - 1023) to a voltage about (0 - 6V)
    const float m_rapportConvertion; // different pour chaque accu
    const int m_maxCAD; // maximum du convertisseur analogique numérique 1023
    const float m_tensionMiniAccus; // valeur minimum de l'accu 4.8v

};

#endif

