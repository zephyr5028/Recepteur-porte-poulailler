/**
    \file AccusCommun.h
    \class AccusCommun
    \brief Commun de la gestion des blocs accumulateurs NIMH de 4,8v
    \details Suivi de la tension et la valeur minimum
    \author Zephyr5028
    \date été 2017
  Prototype de la classe AccusCommun
*/

#ifndef AccusCommun_h
#define AccusCommun_h

#include "Arduino.h"

class AccusCommun
{

  public:
    AccusCommun(); // constructeur
    // constructeur complet
    AccusCommun( const float tensionMiniAccus,  const int R1,  const int R2, const float  Vref, const int maxCAD);
    ~AccusCommun(); // destructeur

    void init();// initialisation
    int lectureAccusCAD(const byte accusPin); // lecture CAD
    float tensionAccus(const byte accusPin); // convertion  CAD vers tension en volt


  protected:
    const int m_maxCAD; // maximum du convertisseur analogique numérique
    const float m_tensionMiniAccus; // tension mini de l'accu
    const int m_R1 ; // resistance  R1 du pont
    const int m_R2 ; // resistance R2 du pont
    const float  m_Vref; // tension de reference
    int m_valMinCAD; // valeur minimale du CAD en fonction de la tension mini de l'accu


};

#endif

