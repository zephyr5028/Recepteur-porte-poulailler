/** Receiver.h
  prototype de la classe Receiver
  shield RXB6 - recepteur 433.92Mhz
*/

#ifndef ReceiverRXB6_h
#define ReceiverRXB6_h


#include "Arduino.h"

class ReceiverRXB6  
{

  public:

    // constructeur

   ReceiverRXB6(  boolean debug = false ); // constructeur avec debug
    ~ReceiverRXB6(); // destructeur

    void init();// initialisation



  protected:

    boolean m_debug ; // pour affichage console si nécessaire



};

#endif

