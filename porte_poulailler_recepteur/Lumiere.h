/** Lumiere.h
  prototype de la classe Lumiere
  ldr
*/

#ifndef Lumiere_h
#define Lumiere_h

#include "Arduino.h"

class Lumiere
{

  public:

    // constructeur avec debug
    Lumiere( const byte lumierePin, const int R2 = 10000, const float  Vref = 5.0, const int maxCAD = 1023);
    ~Lumiere(); // destructeur


    int luminositeCAD(); //lecture de la luminosite CAD
    unsigned int tensionLuminosite(); // convertion CAD  vers LDR


  protected:

    const byte m_lumierePin; //analog pin - tension luminosite
    const int m_R2 ; // resistance R2 du pont
    const float  m_Vref; // tension de reference
    const int m_maxCAD; // maximum du convertisseur analogique numérique

};

#endif

