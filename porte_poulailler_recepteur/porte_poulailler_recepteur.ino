/**@file*/
/**
    \recepteur radio et enregistreur pour la porte du poulailler  Ver 1.0.0
    \file porte_poulailler_recepteur
    \brief recepteur enregistreur des informations envoyés par la(les) porte de poulailler
    \details Electronique avec microcontroleur, alimentée par batteries, couplées à un capteur solaire si utilisation extérieure. Capteur de lumière pour l'étalonnage des boitiers de porte.
    \author Zephyr5028
    \juillet 2017
*/
/**
    recpeteur_enregistreur_porte-poulailler : affichage + clavier + rtc + capteur de lumière + lecteur de carte sd + recpeteur radio 433mhz
*/

/**
  - 23 07 2017 installation du programme
*/
/*
           nom macro  valeur de la macro                                forme syntaxique
           __LINE__  numéro de la ligne courante du programme source   entier
           __FILE__  nom du fichier source en cours de compilation     chaîne
           __DATE__  la date de la compilation                         chaîne
           __TIME__  l'heure de la compilation                         chaîne
           __STDC__  1 si le compilateur est ISO, 0 sinon              entier
*/

/*--------------------------------------------------------------------------------*/
/// choisir entre un afficheur lcd I2C de type Digole (PICF182) ou de type LiquidCrystal (PCF8574)

//#define LCD_DIGOLE  // utilisation de lcd avec circuit I2C Digole - PIC16F182

#define LCD_LIQIDCRYSTAL  // utilisation de lcd liquid crystal I2C - PCF8574

/*--------------------------------------------------------------------------------*/

/**------Bibliothèque Flash pour mise en mémoire flash de l'arduino F()--------*/
#include <Flash.h>
#include <avr/pgmspace.h> // non nécessaire maintenant


/** power and tools */
/** watchdog - Optimisation de la consommation */
#include "PowerTools.h"
#define DEBUG false // positionner debug 
unsigned int memoireLibre = 0; // variable pour calcul de la memoire libre
volatile int f_wdt = 1; // flag watchdog
boolean reglage = false; // menu=false ou reglage=true
#define BUZZER false // positionner BUZZER en fonction de la presence ou pas d'un buzzer sur la carte (true = presence)
#define BUZZER_PIN 7 // broche du buzzer
PowerTools tools (BUZZER_PIN, BUZZER, DEBUG ); // objet tools and power


/** definitions */
#define V_REFERENCE 5.05 // tension de reference
#define MAX_CAD 1023  // maximum du convertisseur analogique digital

/** Accus */
#include "Accus.h"
#define PIN_ACCUS_N1  A6  // analog pin A6 : tension batterie N1
#define PIN_ACCUS_N2  A7  // analog pin A7 : tension batterie N2
#define ACCUS_TESION_MINIMALE  4.8 //valeur minimum de l'accu 4.8v
#define ACCUS_R1 4700 // resistance  R1 du pont
#define ACCUS_R2 10000 // resistance  R2 du pont
#define ACCU_N1 true  // batterie N1 presente si true
#define ACCU_N2 true // batterie N2 presente  si true
boolean batterieFaible = false; //  batterie < ACCUS_TESION_MINIMALE = true
Accus accusN1 (PIN_ACCUS_N1, ACCUS_TESION_MINIMALE, ACCUS_R1, ACCUS_R2, V_REFERENCE, MAX_CAD, DEBUG );
Accus accusN2 (PIN_ACCUS_N2, ACCUS_TESION_MINIMALE, ACCUS_R1, ACCUS_R2, V_REFERENCE, MAX_CAD, DEBUG );

/** lumiere */
#include "Lumiere.h"
#define PIN_LUMIERE A0  // analog pin A0 : luminosite
#define LDR_R2 10000 // resistance  R2 du pont avec la LDR
Lumiere lum(PIN_LUMIERE, LDR_R2, V_REFERENCE, MAX_CAD, DEBUG ); // objet lumiere

/** lumiere BH1750 **/
#include "LumBH1750.h"
#define BH1750_I2C_ADDRESS 0x23 // adresse du circuit I2C
LumBH1750 lightMeter(BH1750_I2C_ADDRESS, DEBUG); // objet BH150


/** interruptions */
volatile boolean interruptBp = false; // etat interruption entree 9
//volatile boolean interruptRTC = false; // etat interruption entree 5
volatile boolean interruptOuvBoi = false; // etat interruption entree 6

/** Clavier */
/* menus */
#include "Clavier.h"
const byte menuDate = 1;
const byte menuHeure = 2;
const byte menuTemperature = 3;
const byte menuLumiere = 4;
const byte menuTensionBatCdes = 6; // tension batterie N1
const byte menuTensionBatServo = 7; // tension batterie N2
const byte menuLightMeter = 5; // mesure du circuit BH1750
const byte menuRecepteur = 8; // recepteur radio
const byte lignesMenu = 8; // nombre de lignes du menu
const byte colonnes = 16; // colonnes de l'afficheur
const byte oldkey = -1;
const byte sensorClavier = A1; //pin A1 pour le clavier
const byte pinBp = 9; // pin D9 bouton poussoir ouverture / fermeture
const byte pinBoitier = 6; //pin D6 interrupteur ouverture boitier
const int debounce = 500; // debounce latency in ms
byte incrementation = 0; // incrementation verticale
boolean relache = false; // relache de la touche
byte touche = -1; // valeur de la touche appuyee (-1 pour non appuyée)
boolean  boitierOuvert = true; // le boitier est ouvert
Clavier clav(lignesMenu, pinBp, pinBoitier, debounce, DEBUG ); // class Clavier avec le nombre de lignes du menu

/** LCD DigoleSerialI2C */
#define LCD_AFFICHAGE_TEMPS_BOUCLE  1000  // temps entre deux affichages
int temps = 0;// pour calcul dans la fonction temporisationAffichage
bool LcdCursor = true; //curseur du lcd if true = enable
#ifdef  LCD_DIGOLE
/// I2C:Arduino UNO: SDA (data line) is on analog input pin 4, and SCL (clock line) is on analog input pin 5 on UNO and Duemilanove
#include "LcdDigoleI2C.h"
LcdDigoleI2C mydisp( &Wire, '\x27', colonnes, debug); // classe lcd digole i2c (lcd 2*16 caracteres)
const char affichageBonjour[] PROGMEM = "Recepteur porte . Version 1.0.0  .Porte Poulailler.Manque carte RTC";
#endif
#ifdef LCD_LIQIDCRYSTAL
#include "LcdPCF8574.h"
// Set the LCD address to 0x27 for a 16 chars and 2 line display pour pcf8574t / si pcf8574at alors l'adresse est 0x3f
LcdPCF8574  mydisp(0x3f, 16, 2);
const char affichageBonjour[] PROGMEM = "Recepteur porte . Version 1.0.0  .Recepteur porte .Manque carte RTC";
#endif

/** RTC_DS3231 */
#include "HorlogeDS3232.h"
const byte rtcINT = 5; // digital pin D5 as l'interruption du rtc - NON CABLEE
const byte adresseBoitier24C32 = 0x57;// adresse du boitier memoire eeprom 24c32
const byte jourSemaine = 1, jour = 2, mois = 3, annee = 4, heure = 5, minutes = 6, secondes = 7;
//const byte alarm_1 = 1; // alarme 1
//const byte alarm_2 = 2; // alarme 2
const bool typeTemperature = true; // true = celsius , false = fahrenheit
tmElements_t tm; // declaration de tm pour la lecture des informations date et heure
HorlogeDS3232 rtc(adresseBoitier24C32, rtcINT, DEBUG );

/**  gestion  radio 433MHz  recepteur */
#include "ReceiverRXB6.h"
// N.B. La constante VW_MAX_MESSAGE_LEN est fournie par la lib VirtualWire
uint8_t message[VW_MAX_MESSAGE_LEN] = {0};
uint8_t taille_message = VW_MAX_MESSAGE_LEN;
volatile boolean receiveMessage = false ;
ReceiverRXB6 myRXB6(DEBUG);

/** gestion carte SD */
#include <SD.h>
const int chipSelect = 4;
boolean sdCard = false; //presence SD card true
File monFichier;


/** progmem  mémoire flash */
const char listeDayWeek[] PROGMEM = "DimLunMarMerJeuVenSam"; // day of week en mémoire flash
const char affichageMenu[] PROGMEM = "      Date      .      Heure     .  Temperature   .  Lumiere LDR   .   Light Meter  . Tension bat N1 . Tension bat N2 .   Recepteur    ";
const char affichageBatterieFaible[] PROGMEM = "*** Batterie faible ! ***";
const char affichageTexte[] PROGMEM = "CFVH lux C;V;L;l;" ;// petits textes
//////////////////////
const char numeroSerieBoitier[] = "N005;\0"; // numero de serie du boitier recepteur
const char affNumBoitier[] PROGMEM = "N005;"; // numero de serie du boitier recepteur
/////////////////////


/* clavier */
///-----lecture clavier------
void  lectureClavier() {
  if (boitierOuvert) {
    touche = clav.read_key(sensorClavier); /// read key sensor = A1
    clav.relacheTouche(touche, relache);
    if (clav.deplacementDansMenu(touche, relache, reglage) or touche == 4) {
      clav.positionMenu(incrementation, touche); /// position du menu pour l'affichage
      deroulementMenu (incrementation); /// affichage du menu
    }
  }
}


///---- temporisation pour l'affichage: affichage du menu lorsque temps est > boucleTemps-----
void temporisationAffichage(const int boucleTemps) {
  if (  temps > boucleTemps and touche != -1 /*and !monServo.get_m_servoAction()*/) {
    deroulementMenu (incrementation); // affichage du menu en fonction de incrementation
    temps = 0;
  } else {
    temps += 1;
  }
}


///----routine lecture et ecriture date and time-----
void ecritureDateTime() {
  RTC.write(tm);
  if (incrementation == menuHeure) {
    displayTime(); // affichage de l'heure
  }
  if (incrementation == menuDate) {
    displayDate(); // affichage de la date
  }
}

/* affichages */
///-----routine display Date-----
void displayDate() {
  if ( boitierOuvert) { // si le boitier est ouvert
    RTC.read(tm); // lecture date et heure
    // affichage du jour de la semaine
    char semaine[4] = "";
    byte j = ((tm.Wday - 1) * 3);
    for (byte i = j; i < j + 3; i++) {
      semaine[i - j] = pgm_read_byte(listeDayWeek + i);
    }
    mydisp.affichageDateHeure(semaine, tm.Day, tm.Month, tm.Year);
  }
}

///-----routine display Time-----
void displayTime () {
  if ( boitierOuvert) { // si le boitier est ouvert
    RTC.read(tm); // lecture date et heure
    // texte = H
    char texte[1] = "";
    texte[0] = pgm_read_byte(affichageTexte + 3 );
    mydisp.affichageDateHeure(texte, tm.Hour, tm.Minute, tm.Second);
  }
}


///----- affichage du circuit BH1750 Light Meter -----
void affiLightMeter () {
  if ( boitierOuvert) { // si le boitier est ouvert
    uint16_t lux = lightMeter.readLightLevel();
    byte ligne = 1;
    bool nonReglable = 1; // pour afficher le curseur sur la premiere ligne car non reglable
    // texte = " lux"
    char texte[5] = "";
    for (byte i = 4; i < 8; i++) {
      texte[i - 4] = pgm_read_byte(affichageTexte +  i);
    }
    mydisp.affichageLumFinCourse(lux, ligne, texte, nonReglable);
  }

}

///-----affiRecepteur-----
void affiRecepteur() {
  if ( boitierOuvert) { // si le boitier est ouvert
    byte ligne = 1;
    bool nonReglable = 1; // pour afficher le curseur sur la premiere ligne car non reglable
    mydisp.resetPos(ligne);
  }

}

/* afficheur */
///-----retro eclairage de l'afficheur-----
void eclairageAfficheur() {
  if (boitierOuvert) {
    if (clav.testTouche5(touche, relache)) {
      mydisp.retroEclairage();// retro eclairage
    }
  }
}

/* batteries */
///-------affichage tension batterie N1
void affiTensionBatCdes() {
  float voltage = accusN1.tensionAccus();// read the input on analog pin A6 : tension batterie N1
  // print out the value you read:
  if ( boitierOuvert) { // si le boitier est ouvert
    byte ligne = 1;
    // texte = V
    char texte[1] = "";
    texte[0] = pgm_read_byte(affichageTexte + 2 );
    mydisp.affichageVoltage(  voltage, texte,  ligne);
  }
}

///-------affichage tension batterie N2
void affiTensionBatServo() {
  float voltage = accusN2.tensionAccus();// read the input on analog pin A7 : tension batterie N2
  // print out the value you read:
  if ( boitierOuvert) { // si le boitier est ouvert
    byte ligne = 1;
    // texte = V
    char texte[1] = "";
    texte[0] = pgm_read_byte(affichageTexte + 2 );
    mydisp.affichageVoltage(  voltage, texte,  ligne);
  }
}



/* reglage de la date */
///----routine reglage jour semaine, jour, mois, annee-----
void reglageDate () {
  if (boitierOuvert) {
    byte deplacement = 3;
    if (incrementation == menuDate) {
      mydisp.cursorPositionReglages (touche, relache, reglage, 14, deplacement, 14);// position du cuseur pendant les reglages
    }
    if ((touche == 2 or touche == 3) and incrementation == menuDate and relache == true and reglage == true) { // si appui sur les touches 2 ou 3 pour reglage des valeurs
      relache = false;
      if (mydisp.get_m_decalage() == deplacement) {
        tm.Wday = rtc.reglageHeure(touche, tm.Wday, jourSemaine);
        ecritureDateTime();
      }
      if (mydisp.get_m_decalage() == 2 * deplacement) {
        tm.Day = rtc.reglageHeure(touche, tm.Day, jour);
        ecritureDateTime();
      }
      if (mydisp.get_m_decalage() == 3 * deplacement) {
        tm.Month = rtc.reglageHeure(touche, tm.Month, mois);
        ecritureDateTime();
      }
      if (mydisp.get_m_decalage() == 4 * deplacement) {
        tm.Year = rtc.reglageHeure(touche, tm.Year, annee);
        ecritureDateTime();
      }
    }
  }
}


/* reglage time */
///-----routine reglage heure , minute , seconde-----
void reglageTime () {
  if (boitierOuvert) {
    byte deplacement = 4;
    if (incrementation == menuHeure) {
      mydisp.cursorPositionReglages (touche, relache, reglage, 14, deplacement, 14);// position du cuseur pendant les reglages
    }
    if ((touche == 2 or touche == 3) and incrementation == menuHeure and relache == true and reglage == true ) { // si appui sur les touches 2 ou 3 pour reglage des valeurs
      relache = false;
      if (mydisp.get_m_decalage() == deplacement) {
        tm.Hour = rtc.reglageHeure(touche, tm.Hour, heure);
        ecritureDateTime(); // routine écriture date and time
      }
      if (mydisp.get_m_decalage() == 2 * deplacement) {
        tm.Minute = rtc.reglageHeure(touche, tm.Minute, minutes);
        ecritureDateTime();
      }
      if (mydisp.get_m_decalage() == 3 * deplacement) {
        tm.Second = rtc.reglageHeure(touche, tm.Second, secondes);
        ecritureDateTime();
      }
    }
  }
}


/* temperature */
///-----routine lecture température sur ds3231 rtc type celsius=true ,fahrenheit=false------
void read_temp(const boolean typeTemperature) {
  float t = rtc.calculTemperature (typeTemperature);//valeur de la temperature en fonction du type
  if ( boitierOuvert) { // si le boitier est ouvert
    byte ligne = 1;
    // texte = C ou F
    char texte[1] = "";
    texte[0] = pgm_read_byte(affichageTexte );
    // String texte = "";
    // if (typeTemperature) texte = "C"; else texte = "F";
    if (typeTemperature) texte[0] = pgm_read_byte(affichageTexte ); else texte[0] = pgm_read_byte(affichageTexte + 1 );
    mydisp.affichageVoltage(t, texte, ligne);
  }
}


/** interruptions
  -routine interruption D2 INT0
  -routine interruption D3 INT1
  -routine interruption Bp
  -routine interruption boitier ouvert
  -test fermeture boitier
*/

///-----routine interruption D2 INT0------
void myInterruptINT0() {
  receiveMessage = true;
}

//-----routine interruption D3 INT1-----
void myInterruptINT1() {
  // rtc.testInterruptRTC(interruptRTC);// test de l'entree 5 - interruption du rtc
  clav.testInterruptionBp (interruptBp);// test l'it du bp
  clav.testInterruptionBoitier (interruptOuvBoi);// test l'it de l'interrupteur du boitier
}

///-----routine interruption Bp-----
void routineInterruptionBp() {
  if (interruptBp) { // test de l'appui sur bouton bp
    if (clav.testToucheBp ()) { //debounce pour le bp

    }
    clav.testRelacheBp(interruptBp);// test du relache du bp
  }
}


///-----test ouverture boitier-----
void routineTestOuvertureBoitier()  {
  if ( clav.testBoitierOuvert( interruptOuvBoi, boitierOuvert)) {

    boitierOuvert = true; // boitier ouvert
    mydisp.gestionCurseur(1); // activation du curseur
    deroulementMenu (incrementation); /// affichage du menu
  }
}

///-----test fermeture boitier-----
void  routineTestFermetureBoitier() {
  if (clav.testBoitierFerme(interruptOuvBoi, boitierOuvert)) {
    mydisp.razLcd(); // extinction et raz du lcd
    boitierOuvert = false; // boitier ferme
    interruptOuvBoi = false; // autorisation de la prise en compte de l'IT
    mydisp.choixRetroEclairage (0);// extinction retro eclairage

  }
}

/* lumiere
  -affichage
*/
///-----routine lecture et affichage de la lumière-----
void lumiere() {
  unsigned int lumValue = lum.tensionLuminosite(); // luminosite CAD sur pin A0
  if ( boitierOuvert) { // si le boitier est ouvert
    byte ligne = 1;// première ligne car non reglable
    bool nonReglable = 1; // pour afficher le curseur sur la premiere ligne car non reglable
    mydisp.affichageLumFinCourse(lumValue, ligne, " lux", nonReglable);
  }
}

///-----rouine affichage avec PROGMEM------
String affTexteProgmem ( const char* nomFichier, byte iDepart, byte nbCaracteres) {
  String chaine = "";
  char texte[nbCaracteres + 1] = "";
  byte i = iDepart;
  for (i; i <  iDepart + nbCaracteres ; i++) {
    texte[i - iDepart] = pgm_read_byte(nomFichier + i);
  }
  chaine += texte;
  chaine += "\0";
return  chaine;
}

///-----routine reception message-----
void routineReceptionMessage (int lux, float temp) {
  if (receiveMessage) {
    noInterrupts();
    if (myRXB6.reception(message, taille_message)) {
      if (Serial) {
        Serial.println("");
        // numero de serie du boitier recepteur "N00x;"
        Serial.print(affTexteProgmem(affNumBoitier, 0, 5));
        Serial.print(temp);//valeur de la temperature
        // texte = " C;"
        Serial.print(affTexteProgmem(affichageTexte, 9, 2));
        Serial.print(accusN1.tensionAccus());
        // texte = "V;"
        Serial.print(affTexteProgmem(affichageTexte, 11, 2));
        Serial.print(accusN2.tensionAccus());
        // texte = "V;"
        Serial.print(affTexteProgmem(affichageTexte, 11, 2));
        Serial.print(lum.tensionLuminosite());
        // texte = "L;"
        Serial.print(affTexteProgmem(affichageTexte, 13, 2));
        Serial.print(lux);
        // texte = "l;"
        Serial.println(affTexteProgmem(affichageTexte, 15, 2));
      }

      if (sdCard) {
        // si le fichier que vous voulez ouvrir n'existe pas, il sera créé.
        monFichier = SD.open("boitiers.txt", FILE_WRITE); // ouvre le fichier en mode ecriture
        //monFichier = SD.open("test.txt", FILE_READ); // ouvre le fichier en mode lecture
        //monFichier = SD.open("test.txt"); // ouvre le fichier en mode lecture

        // vérifier si le fichier est ouvert
        if (monFichier) {
          monFichier.print((char*) message);
          monFichier.println("");
          // numero de serie du boitier recepteur "N00x;"
          monFichier.print(affTexteProgmem(affNumBoitier, 0, 5));
          //monFichier.print((char*)numeroSerieBoitier);
          monFichier.print(temp);//valeur de la temperature
          // texte = " C;"
          monFichier.print(affTexteProgmem(affichageTexte, 9, 2));
          monFichier.print(accusN1.tensionAccus());
          // texte = "V;"
          monFichier.print(affTexteProgmem(affichageTexte, 11, 2));
          monFichier.print(accusN2.tensionAccus());
          // texte = "V;"
          monFichier.print(affTexteProgmem(affichageTexte, 11, 2));
          monFichier.print(lum.tensionLuminosite());
          // texte = "L;"
          monFichier.print(affTexteProgmem(affichageTexte, 13, 2));
          monFichier.print(lux);
          // texte = "l;"
          monFichier.println(affTexteProgmem(affichageTexte, 15, 2));
          monFichier.close(); // fermer le fichier
        } else {
          // si le fichier ne s'est pas ouvert :
          if (Serial) {
            //Serial.println("Err");
          }
        }
      }
    }
    receiveMessage = false;
    message[VW_MAX_MESSAGE_LEN] = {0};
    taille_message = VW_MAX_MESSAGE_LEN;
    interrupts();
  }
}

/* menu */
///-----routine affichage menus------
void deroulementMenu (byte increment) {
  if (boitierOuvert) {
    byte j = ((increment - 1) * (colonnes + 1)); // tous les 16 caractères
    mydisp.cursorPosition(0, 0, ((char *)"")); // decalage, ligne, texte
    for (byte i = j; i < j + colonnes; i++) { // boucle pour afficher 16 caractères sur le lcd
      char temp = pgm_read_byte(affichageMenu + i); // utilisation du texte présent en mèmoire flash
      mydisp.print(temp);// valable pour digoleSerial et liquidCrystal
    }
    switch (increment) { // test de la valeur de incrementation pour affichage des parametres
      case 1: // Date
        displayDate(); // affichage de la date
        break;
      case 2: // heure
        displayTime(); // affichage de l'heure
        break;
      case 3: // temperature
        read_temp(true); // read temperature celsius=true
        break;
      case 4: //lumiere
        lumiere(); // lecture et affichage de la lumiere
        break;
      case 5:  // mesure du circuit BH1750
        affiLightMeter(); //
        break;
      case 6:  // tension batterie commandes
        affiTensionBatCdes(); //
        break;
      case 7:  // tension batterie servo
        affiTensionBatServo(); //
        break;
      case 8:  // recepteur
        affiRecepteur(); //
        break;
    }
  }
}

/* interruption du watchdog */
///----Watchdog Interrupt Service est exécité lors d'un timeout du WDT----
ISR(WDT_vect) {
  if (f_wdt == 0) {
    f_wdt = 1; // flag
  }
}

///-----entree dans le mode sleep-----
void enterSleep(void) {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_mode(); //Entre dans le mode veille choisi
  // Le programme va reprendre ici après le timeout du WDT
  sleep_disable(); // La 1ère chose à faire est de désactiver le mode veille
}

///-----routine de gestion du watchdog-----
void routineGestionWatchdog() {
  if ((f_wdt == 1 ) and (!boitierOuvert)) { // si le boitier est ferme et watchdog=1
    f_wdt = 0;
    enterSleep(); //Revenir en mode veille
  }
}

///-----routine affichage au demarrage-----
void affichageDemarrage (byte colonne) {
  char temp[16] = {0};
  char temp1[16] = {0};
  for (byte i = colonne; i < 16 + colonne; i++) { // boucle pour afficher 16 caractères sur le lcd
    temp[i - colonne] = pgm_read_byte(affichageBonjour + i); // utilisation du texte présent en mèmoire flash
  }
  colonne += 17;
  for (byte i = colonne; i < 16 + colonne; i++) { // boucle pour afficher 16 caractères sur le lcd
    temp1[i - colonne] = pgm_read_byte(affichageBonjour + i); // utilisation du texte présent en mèmoire flash
  }
  mydisp.bonjour(temp, temp1); // affichage version sur les deux lignes
}

/* setup */
void setup() {

  Serial.begin(9600);
  // if (Serial) console = true; else console = false; //if serial UART to USB is connected show console O/P.

  rtc.init();// initialisation de l'horloge et verification de la presence de la carte RTC / memoire 24C32

  mydisp.init(); // initialisation for text LCD adapter

  byte colonne = 0;
  // affichage version au demarrage ou  defaut RTC
  if (rtc.testPresenceCarteRTC()) {
    affichageDemarrage (colonne);
  } else {
    colonne = 34;// position dans la mèmoire flash
    affichageDemarrage(colonne);
  }

  incrementation = menuDate; // position du  menu au demarrage
  deroulementMenu (incrementation); // affichage du menu
  /*
    lum.set_m_ouverture( rtc.i2c_eeprom_read_byte( 0x14));// lecture du type d'ouverture @14  de l'eeprom de la carte rtc (i2c @ 0x57)
    delay(10);
    lum.set_m_fermeture( rtc.i2c_eeprom_read_byte( 0x15)); // lecture du type de fermeture @15   de l'eeprom de la carte rtc (i2c @ 0x57)
    delay(10);

    byte val1 = rtc.i2c_eeprom_read_byte( 0x16); // lecture pf lumière du matin (byte)
    delay(10);
    byte val2 = rtc.i2c_eeprom_read_byte( 0x17); // lecture Pf lumiere du matin (byte)
    delay(10);
    lum.set_m_lumMatin((val2 << 8) + val1);// mots 2 byte vers mot int lumMatin

    val1 = rtc.i2c_eeprom_read_byte( 0x18); // lecture pf lumière du soir (byte)
    delay(10);
    val2 = rtc.i2c_eeprom_read_byte( 0x19); // lecture Pf lumiere du soir (byte)
    delay(10);
    lum.set_m_lumSoir((val2 << 8) + val1);// mots 2 byte vers mot int lumSoir

    val1 = rtc.i2c_eeprom_read_byte( 0x20); // lecture pf fin de course haut (byte)
    delay(10);
    val2 = rtc.i2c_eeprom_read_byte( 0x21); // lecture Pf fin de course haut (byte)
    delay(10);
    // rotary.set_m_finDeCourseFermeture ((val2 << 8) + val1);  // mots 2 byte vers mot int finDeCourseFermeture

    val1 = rtc.i2c_eeprom_read_byte( 0x22); // lecture pf fin de course bas (byte)
    delay(10);
    val2 = rtc.i2c_eeprom_read_byte( 0x23); // lecture Pf fin de course bas (byte)
    delay(10);
    // rotary.set_m_finDeCourseOuverture ((val2 << 8) + val1);  // mots 2 byte vers mot int finDeCourseOuverture
  */

  /*
    LOW : l’interruption est déclenchée quand la broche concernée est LOW. Comme il s’agit d’un état et non d’un événement, l’interruption sera déclenchée tant que la broche est LOW.
    Par conséquent, dès que l’ISR aura terminé son exécution, elle la recommencera. Pendant ce temps, loop() ne sera pas exécuté.
    CHANGE : l’interruption est déclenchée quand la broche concernée change d’état, c’est à dire passe de LOW à HIGH ou bien de HIGH à LOW. Il s’agit d’un événement.
    RISING : l’interruption est déclenchée quand la broche concernée passe de LOW à HIGH. Il s’agit également d’un événement.
    FALLING : l’interruption est déclenchée quand la broche concernée passe de HIGH à LOW. Il s’agit encore d’un événement.
  */
  attachInterrupt(1, myInterruptINT1, FALLING); // validation de l'interruption sur int1 (d3)
  //attachInterrupt(digitalPinToInterrupt(1), myInterruptINT1, FALLING); // validation de l'interruption sur int1 (d3)
  attachInterrupt(0, myInterruptINT0, FALLING); // validation de l'interruption sur int0 (d2)
  // attachInterrupt(digitalPinToInterrupt(0), myInterruptINT0, CHANGE); // validation de l'interruption sur int0 (d2)

  tools.setupPower(); // initialisation power de l'arduino

  tools.setup_watchdog(9); // initialisation : maxi de 8 secondes pour l'it du watchdog

  // tools.setupBuzzer(1000); // initialisation du buzzer et test

  // initialisation des accus
  accusN1.init();
  accusN2.init();

  /// initialisation BH1750 light meter
  /*
      BH1750 had six different measurment modes. They are divided in two groups -
      continuous and one-time measurments. In continuous mode, sensor continuously
      measures lightness value. And in one-time mode, sensor makes only one
      measurment, and going to Power Down mode after this.
      Each mode, has three different precisions:
        - Low Resolution Mode - (4 lx precision, 16ms measurment time)
        - High Resolution Mode - (1 lx precision, 120ms measurment time)
        - High Resolution Mode 2 - (0.5 lx precision, 120ms measurment time)
      By default, library use Continuous High Resolution Mode, but you can set
      any other mode, by define it to BH1750.begin() or BH1750.configure() functions.
      [!] Remember, if you use One-Time mode, your sensor will go to Power Down mode
      each time, when it completes measurment and you've read it.
      Full mode list:
        BH1750_CONTINUOUS_LOW_RES_MODE
        BH1750_CONTINUOUS_HIGH_RES_MODE (default)
        BH1750_CONTINUOUS_HIGH_RES_MODE_2
        BH1750_ONE_TIME_LOW_RES_MODE
        BH1750_ONE_TIME_HIGH_RES_MODE
        BH1750_ONE_TIME_HIGH_RES_MODE_2
  */
  lightMeter.init(BH1750_CONTINUOUS_HIGH_RES_MODE);

  /// initialisation recepteur RXB6
  myRXB6.init();

  if (!SD.begin(chipSelect)) {
    if (Serial) {
      //Serial.println("Err");
    }
    sdCard = false;
  } else {
    if (Serial) {
      //Serial.println("ok");
    }
    sdCard = true;
  }

}

/* loop */
void loop() {

  lectureClavier(); // lecture du clavier
  temporisationAffichage(LCD_AFFICHAGE_TEMPS_BOUCLE) ; // temporisation pour l'affichage

  /// reglages
  reglageTime(); // reglages de l'heure, minute, seconde si touche fleche droite
  reglageDate(); // reglage de la date si touche fleche droite
  eclairageAfficheur(); // retro eclairage de l'afficheur

  memoireLibre = freeMemory(); // calcul de la  memoire sram libre

  /// test suivant le nombre de batteries presentes
  if (ACCU_N1 and ACCU_N2) {
    batterieFaible = accusN1.accusFaible() or accusN2.accusFaible(); // test de la tension des batteries
  } else if (ACCU_N1) {
    batterieFaible = accusN1.accusFaible() ;// test de la tension de la batterie N1
  } else if (ACCU_N2) {
    batterieFaible = accusN2.accusFaible() ;// test de la tension de la batterie N2
  }

  routineTestFermetureBoitier(); // test fermeture boitier
  routineTestOuvertureBoitier();// test ouvertuer boitier

  routineInterruptionBp(); // routine interruption Bp

  int lux = lightMeter.readLightLevel();
  float temp = rtc.calculTemperature (true);

  routineReceptionMessage(lux, temp); // routine reception message et affichage

  routineGestionWatchdog(); // routine de gestion du watchdog
}



