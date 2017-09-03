/** Bouton.cpp
  définitions de la classe Bouton
*/

#include "Bouton.h"

Bouton::Bouton( ) : m_pinInter(9), m_debounce(350), m_relacheBp(true), m_tempoDebounce(0), m_debouncePret(false)
{

}

/* sucharge du constructeur */
Bouton::Bouton( const byte pinInter ,const int debounce) : m_pinInter(pinInter),
  m_relacheBp(true), m_debounce(debounce), m_tempoDebounce(0), m_debouncePret(false)
{

}

Bouton::~Bouton() {

}

///-----test touche Bp-----
bool Bouton::testToucheBp() {
  if (((millis() - m_tempoDebounce) > m_debounce)  and  m_relacheBp == true and !digitalRead(m_pinInter)) {
    m_relacheBp = false;
    m_debouncePret = true;// pour le relache du bp
    return true;
  } else {
    return false;
  }
}

///-----test relache Bp-----
void Bouton::testRelacheBp (volatile bool & interruptBp) {
  if (m_relacheBp == false and digitalRead(m_pinInter) and m_debouncePret ) {
    m_tempoDebounce = millis();// pour eviter des declenchements intempestifs
    m_debouncePret = false;
  }
  if ((millis() - m_tempoDebounce) > m_debounce ) {
    interruptBp = false; // autorisation de la prise en compte de l'IT
    m_relacheBp = true;
  }
}

///-----test IT Bp-----
void Bouton::testInterruptionBp (volatile bool & interruptBp) {
  if (!digitalRead(m_pinInter) and !interruptBp) { // entree 9 pour interruption BP
    interruptBp = true;
    m_tempoDebounce = millis();
  }
}

///-----test IT ouverture boitier-----
void Bouton::testInterruptionBoitier (volatile bool & interruptOuvBoi) {
  if (!digitalRead(m_pinInter) and !interruptOuvBoi) { // entree 9 pour interruption BP
    interruptOuvBoi = true;
  }
}

///-----test boitier ouvert------
bool  Bouton::testBoitierOuvert(const volatile bool & interruptOuvBoi, const bool & boitierOuvert) {
  if ( interruptOuvBoi and !digitalRead(m_pinInter) and !boitierOuvert) {    //  interruption ouverture boitier
    return true;
  } else {
    return false;
  }
}

///-----test boitier ferme------
bool  Bouton::testBoitierFerme(const volatile bool & interruptOuvBoi, const bool & boitierOuvert) {
  if (digitalRead(m_pinInter) and boitierOuvert) { //  fermeture boitier
    return true;
  } else {
    return false;
  }
}

