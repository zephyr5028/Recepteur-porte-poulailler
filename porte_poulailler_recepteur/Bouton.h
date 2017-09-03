/** Bouton.h
  prototype de la Bouton
*/

#ifndef Bouton_h
#define Bouton_h

#include "Arduino.h"

class Bouton
{
  public:

    Bouton(); // constructeur
    Bouton( const byte pinInter, const int debounce = 50); // surcharge du constructeur
    ~Bouton(); // destructeur

    bool testToucheBp();  //test touche Bp
    void testRelacheBp (volatile bool &interruptBp);//test relache Bp
    void testInterruptionBp (volatile bool &interruptBp); //test Bp suite interruption
    void testInterruptionBoitier (volatile bool &interruptOuvBoi); //test IT ouverture boitier
    bool testBoitierOuvert(const volatile bool &interruptOuvBoi, const bool &boitierOuvert);//test boitier ouvert
    bool testBoitierFerme(const volatile bool &interruptOuvBoi, const  bool &boitierOuvert); //test boitier ferme

  protected:

    const byte m_pinInter; // pin interrupteur ou bouton poussoir. d9 par defaut
    const int m_debounce; // debounce latency in ms (50)
    unsigned int m_tempoDebounce; // variable pour le calcul de l'anti-rebond
    bool m_relacheBp; // relache du Bp
    bool m_debouncePret;// pour le relache du bp

};

#endif

