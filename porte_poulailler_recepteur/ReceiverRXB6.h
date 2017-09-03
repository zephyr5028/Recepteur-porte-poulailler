/** Receiver.h
  prototype de la classe Receiver
  shield RXB6 - recepteur 433.92Mhz
*/

#ifndef ReceiverRXB6_h
#define ReceiverRXB6_h

#include <VirtualWire.h>
#include "Arduino.h"

class ReceiverRXB6
{

  public:

    // constructeur

    ReceiverRXB6(); // constructeur
    ~ReceiverRXB6(); // destructeur

    void init();// initialisation
    boolean reception(uint8_t* chaine, uint8_t taille); //reception message
    void receivePrint(uint8_t* chaine, uint8_t taille);// impression de la reception d'un message



  protected:


};

#endif

