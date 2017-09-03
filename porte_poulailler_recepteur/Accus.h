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
    Accus(const boolean accuPresent, const byte accusPin, const float tensionMiniAccus = 4.8,  const int R1 = 4700,  const int R2 = 10000, const float  Vref = 5.0, const int maxCAD = 1023, boolean debug = false);
    ~Accus(); // destructeur

    void init();// initialisation
    void accusFaible();//batterie faible  , (batterie < tensionMiniAccus) = true
    int lectureAccusCAD(); // lecture CAD
    float tensionAccus(); // convertion  CAD vers tension en volt
    boolean get_m_batterieFaible();//accesseur - getter
    void set_m_batterieFaible(boolean decalage);//mutateur - setter


  protected:
    const boolean m_debug ; // pour affichage console si nécessaire
    const boolean m_accuPresent; // presence de l'accu
    const byte m_accusPin; //analog pin - tension batterie
    const int m_maxCAD; // maximum du convertisseur analogique numérique
    const float m_tensionMiniAccus; // tension mini de l'accu
    const int m_R1 ; // resistance  R1 du pont
    const int m_R2 ; // resistance R2 du pont
    const float  m_Vref; // tension de reference
    int m_valMinCAD; // valeur minimale du CAD en fonction de la tension mini de l'accu
    boolean m_batterieFaible; // etat de la batterie

};

#endif

