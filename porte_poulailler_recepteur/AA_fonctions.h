/*

    fonctions du programme  : recepteur radio et enregistreur pour la porte du poulailler

*/

#ifndef AA_fonctions_h
#define AA_fonctions_h

/**------Bibliothèque Flash pour mise en mémoire flash de l'arduino F()--------*/
#include <Flash.h>
#include <avr/pgmspace.h> // non nécessaire maintenant
/** power and tools */
#include "PowerTools.h"
#define DEBUG false // positionner debug
#define BUZZER false // positionner BUZZER en fonction de la presence ou pas d'un buzzer sur la carte (true = presence)
#define BUZZER_PIN 7 // broche du buzzer
/** definitions */
#define V_REFERENCE 5.14 // tension de reference
#define MAX_CAD 1023  // maximum du convertisseur analogique digital
/** Accus */
#include "Accus.h"
#define PIN_ACCUS_N1  A6  // analog pin A6 : tension batterie N1
#define PIN_ACCUS_N2  A7  // analog pin A7 : tension batterie N2
#define ACCUS_TESION_MINIMALE  4.8 //valeur minimum de l'accu 4.8v
#define ACCUS_R1 4700 // resistance  R1 du pont
#define ACCUS_R2 10000 // resistance  R2 du pont
#define ACCU_N1 true  // batterie N1 presente si true
#define ACCU_N2 false // batterie N2 presente si true
/** lumiere */
#include "Lumiere.h"
#define PIN_LUMIERE A0  // analog pin A0 : luminosite
#define LDR_R2 10000 // resistance  R2 du pont avec la LDR
/** lumiere BH1750 **/
#include "LumBH1750.h"
#define BH1750_I2C_ADDRESS 0x23 // adresse du circuit I2C
/** menus */
#define MENU_DATE 1 // date
#define MENU_HEURE 2 // heure 
#define MENU_TEMPERATURE 3 // temperature
#define MENU_LUMIERE 4 // lumiere lux ou resistance de la ldr
#define MENU_LIGHTMETER 5 //mesure du circuit BH1750 en lux
#define MENU_TENSION_BAT_N1 6  // tension batterie N1 en volt
#define MENU_TENSION_BAT_N2 7  // tension batterie N2 en volt
#define MENU_RECEPTEUR_0  8  // recepteur radio - 0 : 1ere partie
#define MENU_RECEPTEUR_1  9  // recepteur radio - 1 : 2eme partie
#define MENU_RECEPTEUR_2  10  // recepteur radio - 2 : 3eme partie
#define MENU_RECEPTEUR_3  11  // recepteur radio - 3 : 4eme partie
/** Clavier - boutons */
#include "Clavier.h"
#define PIN_BP 9  // pin D9 bouton poussoir ouverture / fermeture
#define PIN_BOITIER 6 //pin D6 interrupteur ouverture boitier
#define PIN_SENSOR_CLAVIER A1 //pin A1 pour le clavier
#define DEBOUNCE 500 // debounce latency in ms
#define LIGNES_MENU 11 // nombre de lignes du menu
/**afficheurs lcd */
#define COLONNES 16 // colonnes de l'afficheur
#define LIGNES 2 // lignes de l'afficheur
#define LCD_AFFICHAGE_TEMPS_BOUCLE  1000  // temps entre deux affichages
/** RTC_DS3231 */
#include "HorlogeDS3232.h"
#define PIN_RTC_INT 5  // digital pin D5,  interruption du rtc......NON CABLEE
#define ADRESSE_BOITIER_24C32 0x57 // adresse du boitier memoire eeprom 24c32
#define JOUR_SEMAINE 1
#define JOUR 2
#define MOIS 3
#define ANNEE 4
#define HEURE 5
#define MINUTES 6
#define SECONDES 7
#define TYPE_TEMPERATURE true // true = celsius , false = fahrenheit
/**  gestion  radio 433MHz  recepteur */
#include "ReceiverRXB6.h"
/** gestion carte SD */
#include <SD.h>
#define CHIP_SELECT 4 // chip select de la carte sd

/* clavier */
//-----lecture clavier------
void  lectureClavier();

//---- temporisation pour l'affichage: affichage du menu lorsque temps est > boucleTemps-----
void temporisationAffichage(const int boucleTemps);

///----- build fileName-----
void buildFileName();

//----routine lecture et ecriture date and time-----
void ecritureDateTime();

/* affichages */
//-----routine display Date-----
void displayDate();

//-----routine display Time-----
void displayTime ();

///----- affichage du circuit BH1750 Light Meter -----
void affiLightMeter ();

///-----routine lecture d'un message en plusieurs morceaux-----
void morceaux16caracteres (byte debut);

///-----affichage Recepteur-----
void affiRecepteur();

/* afficheur */
//-----retro eclairage de l'afficheur-----
void eclairageAfficheur();

/* batteries */
//-------affichage tension batterie commandes
void affiTensionBatCdes();

//-------affichage tension batterie servo-moteur
void affiTensionBatServo();

/* reglage de la date */
//----routine reglage jour semaine, jour, mois, annee-----
void reglageDate ();

/* reglage time */
//-----routine reglage heure , minute , seconde-----
void reglageTime ();

/* temperature */
//-----routine lecture température sur ds3231 rtc type celsius=true ,fahrenheit=false------
void read_temp(const boolean typeTemperature);


/** interruptions
  -routine interruption D2 INT0
  -routine interruption D3 INT1
  -routine interruption Bp
  -routine interruption boitier ouvert
  -test fermeture boitier
*/
//-----routine interruption D2 INT0------
void myInterruptINT0();

//-----routine interruption D3 INT1-----
void myInterruptINT1();

//-----routine interruption Bp-----
void routineInterruptionBp();

//-----test ouverture boitier-----
void routineTestOuvertureBoitier();

//-----test fermeture boitier-----
void  routineTestFermetureBoitier();

/** lumiere  -  affichage
*/
//-----routine lecture et affichage de la lumière-----
void lumiere();

///-----routine reception message-----
void routineReceptionMessage (int lux, float temp);

/* menu */
//-----routine affichage menus------
void deroulementMenu (byte increment);

/* interruption du watchdog */
//----Watchdog Interrupt Service est exécité lors d'un timeout du WDT----
ISR(WDT_vect);

//-----entree dans le mode sleep-----
void enterSleep(void);

//-----routine de gestion du watchdog-----
void routineGestionWatchdog();

//-----routine affichage au demarrage-----
void affichageDemarrage (byte colonne);

#endif

