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

#include "AccusCommun.h"
#include "Arduino.h"

class Accus  : public AccusCommun
{

  public:
    // constructeur
    Accus( const boolean accuPresent, const byte accusPin);
    // constructeur complet
    Accus(const boolean accuPresent, const byte accusPin, const float tensionMiniAccus,  const int R1,  const int R2, const float  Vref, const int maxCAD);
    ~Accus(); // destructeur

    //void init();// initialisation
    void accusFaible();//batterie faible  , (batterie < tensionMiniAccus) = true
    float tensionAccus(); // convertion  CAD vers tension en volt
    boolean get_m_batterieFaible();//accesseur - getter
    void set_m_batterieFaible(boolean decalage);//mutateur - setter


  protected:
    const boolean m_accuPresent; // presence de l'accu
    const byte m_accusPin; //analog pin - tension batterie
    boolean m_batterieFaible; // etat de la batterie

};

#endif

