/** Radio.cpp
  définitions de la classe Radio
  radio 433MHz
*/

#include "Radio.h"

//constructeur avec debug et radio pour affichage si nécessaire
Radio::Radio(byte pinEmRadio, byte pinSwitchEmissionRadio, int vitesseTransmission, byte taille, bool radio, const boolean debug) :  m_pinEmRadio(pinEmRadio), m_pinSwitchEmissionRadio(pinSwitchEmissionRadio),
  m_vitesseTransmission(vitesseTransmission), m_taille(taille), m_radio(radio), m_debug(debug)
{
  m_chaine[m_taille] = {0};// initialisation du tableau
}

Radio::~Radio()
{
}

///-----initialisation-----
void Radio::init () {
  m_chaine[0] = '\0'; // effacement du tableau
  vw_set_tx_pin(m_pinEmRadio); // broche d10 emetteur
  vw_setup(m_vitesseTransmission); // initialisation de la bibliothèque avec la vitesse (vitesse_bps)
}

///-----raz de m_chaine-----
void Radio::raz_m_chaine() {
  m_chaine[0] = '\0'; // effacement du tableau
}

///----message Radio-----
void Radio::messageRadio(char *chaine1) {
  strcat(chaine1, "\0");
  vw_send((uint8_t *)chaine1, strlen(chaine1) + 1); // On envoie le message
  // strlen : Retourne le nombre de caractères de cs sans tenir compte du caractère de fin de chaîne.
  vw_wait_tx(); // On attend la fin de l'envoi
  //delay(10);
}

///----message Radio sans parametre-----
void Radio::messageSansParametre() {
  //strcat(m_chaine, "\0");
  byte leng = strlen(m_chaine);// longueur du tableau
  m_chaine[leng + 1] = '\0';// fin de tableau
  vw_send((uint8_t *)m_chaine, leng + 1); // On envoie le message
  // strlen : Retourne le nombre de caractères de cs sans tenir compte du caractère de fin de chaîne.
  vw_wait_tx(); // On attend la fin de l'envoi
  //delay(10);
  for (byte i = 0; i < leng + 1; i++)   m_chaine[i] = {0};
}

///----routine construction message radio----
void Radio::envoiMessage(char *chaine1) {
  char chaineComp[] = "Fin";
  if (strcmp(chaineComp, chaine1) != 0) { // test de la dernière chaine
    //  strcat(m_chaine, chaine1);
    byte lenChaine1(0);
    while (chaine1[lenChaine1] != '\0') lenChaine1++;
    byte  len_m_chaine(0);
    while (m_chaine[len_m_chaine] != '\0')  len_m_chaine++;
    for (byte pos = 0; pos < lenChaine1; pos++) {
      m_chaine[len_m_chaine + pos] = chaine1[pos];
      m_chaine[len_m_chaine + lenChaine1] = '\0';
    }
  } else {
    messageSansParametre();// envoi du message contenu dans m_chaine[]
  }
}

///----chaine radio fin de ligne avant transmission-----
void Radio::chaineVide() {
  if (m_radio) {
    char chaine1[m_taille - 1] = "";
    strcat(chaine1, "Fin");
    Radio::envoiMessage(chaine1);// on envoie le message
  }
}

///-----envoi message float avec test de l'ouverture du boitier plus texte-----
void Radio::envoiFloat(float valeur, boolean boitierOuvert, char *texte ) {
  if (m_radio and !boitierOuvert) {
    char chaine[m_taille - 1] = "";
    char valeur_temp[8] = "";
    dtostrf(valeur, 2, 2, valeur_temp);
    strcat(chaine, valeur_temp);
    strcat(chaine, texte);
    Radio::envoiMessage(chaine);// on envoie le message
  }
}

///-----envoi message unsigned int avec test de l'ouverture du boitier plus texte-----
void Radio::envoiUnsignedInt(unsigned int valeur, boolean boitierOuvert, char *texte) {
  if (m_radio and !boitierOuvert) {
    char chaine1[m_taille - 1] = "";
    char valeur_temp[8] = "";
    sprintf(valeur_temp, "%u", valeur); // u Unsigned decimal integer
    strcat(chaine1, valeur_temp);
    strcat(chaine1, texte);
    Radio::envoiMessage(chaine1);// on envoie le message
  }
}

///-----envoi message int avec test de l'ouverture du boitier plus texte-----
void Radio::envoiInt(int valeur, boolean boitierOuvert, char *texte) {
  if (m_radio and !boitierOuvert) {
    char chaine1[m_taille - 1] = "";
    char valeur_temp[8] = "";
    sprintf(valeur_temp, "%d", valeur); // d signed decimal integer
    strcat(chaine1, valeur_temp);
    strcat(chaine1, texte);
    Radio::envoiMessage(chaine1);// on envoie le message
  }
}

////-----envoi texte avec test de l'ouverture du boitier-----
void Radio::envoiTexte(boolean boitierOuvert, char *texte) {
   if (m_radio and !boitierOuvert) {
    char chaine1[m_taille - 1] = "";
    strcat(chaine1, texte);
    Radio::envoiMessage(chaine1);// on envoie le message
  }
}

///-----test du switch emission radio on/off-----
void Radio::testSwitchEmissionRadio() {
 if (m_radio and !digitalRead(m_pinSwitchEmissionRadio)) {
    raz_m_chaine();// effacement du tableau
    m_radio = false;
  } else if (!m_radio and digitalRead(m_pinSwitchEmissionRadio)) {
    m_radio = true;
  }
}

//-----accesseur - getter-----
bool Radio::get_m_radio() {
  return m_radio;
}

//-----mutateur - setter-----
void Radio::set_m_radio(bool radio) {
  m_radio = radio;
}

