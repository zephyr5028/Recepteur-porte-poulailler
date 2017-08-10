/** LumBH1750.h
  prototype de la classe LumBH1750
  shield BH1750 - capteur de lumière
*/

#ifndef LumBH1750_h
#define LumBH1750_h

#include <Wire.h>
#include <BH1750.h>    //https://github.com/claws/BH1750

#include "Arduino.h"

class LumBH1750   : public BH1750
{

  public:

    // constructeur

    LumBH1750(const byte BH1750_I2C_ADDRESS = 0x23,  boolean debug = false ); // constructeur avec debug
    ~LumBH1750(); // destructeur

    void init(uint8_t mode);// initialisation



  protected:

    boolean m_debug ; // pour affichage console si nécessaire



};

#endif

