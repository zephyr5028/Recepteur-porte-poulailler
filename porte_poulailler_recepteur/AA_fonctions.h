/*

    fonctions du programme  : recepteur radio et enregistreur pour la porte du poulailler

*/

#ifndef AA_fonctions_h
#define AA_fonctions_h

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

