/** Receiver.h
  definitions de la classe Receiver
  shield RXB6 - recepteur 433.92Mhz
*/

#include "ReceiverRXB6.h"

ReceiverRXB6::ReceiverRXB6( boolean debug) : m_debug(debug)
{

}

ReceiverRXB6::~ReceiverRXB6()
{

}


///-----initialisation-----
void ReceiverRXB6::init() {
  vw_set_rx_pin(2); // broche  recepteur
  vw_setup(600); // initialisation de la bibliothèque avec la vitesse (vitesse_bps)
  vw_rx_start(); // On peut maintenant recevoir des messages
}

///------reception message-----
boolean ReceiverRXB6::reception(uint8_t* chaine, uint8_t taille) {
  if (vw_get_message(chaine, &taille))  {   // Non-blocking
    receivePrint( chaine, taille);
    return true;
  }
  return false;
}

///---- impression recepteur chaine-----
void ReceiverRXB6::receivePrint(uint8_t* chaine, uint8_t taille) {
  for (byte i = 0; i < taille - 1; i++)    {
    //Serial.print(m_message[i], HEX);
    Serial.write(chaine[i]);
  }
}

