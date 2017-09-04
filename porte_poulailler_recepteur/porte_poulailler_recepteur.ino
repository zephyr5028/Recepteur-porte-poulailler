/**@file*/
/**
    \recepteur radio et enregistreur pour la porte du poulailler  Ver 1.0.1
    \file porte_poulailler_recepteur
    \brief recepteur enregistreur des informations envoyés par la(les) porte de poulailler
    \details Electronique avec microcontroleur, alimentée par batteries, couplées à un capteur solaire si utilisation extérieure. Capteur de lumière pour l'étalonnage des boitiers de porte.
    \author Zephyr5028
    \juillet 2017
*/
/**
    recepteur_enregistreur_porte-poulailler : affichage + clavier + rtc + capteur de lumière + lecteur de carte sd + recpeteur radio 433mhz
*/

/**
  - 23 07 2017 installation du programme
  - 30 08 2017 en utilisation
  - 03 09 2017 classe accus remodelee
*/
/*
           nom macro  valeur de la macro                                forme syntaxique
           __LINE__  numéro de la ligne courante du programme source   entier
           __FILE__  nom du fichier source en cours de compilation     chaîne
           __DATE__  la date de la compilation                         chaîne
           __TIME__  l'heure de la compilation                         chaîne
           __STDC__  1 si le compilateur est ISO, 0 sinon              entier
*/

#include "AA_fonctions.h" //prtotypes defines et structures

/** Numero de serie du boitier */
/*--------------------------------------------------------------------------------*/
const char affNumBoitier[] PROGMEM = "N005;"; // numero de serie du boitier recepteur
/*--------------------------------------------------------------------------------*/
/// choisir entre un afficheur lcd I2C de type Digole (PICF182) ou de type LiquidCrystal (PCF8574)
//#define LCD_DIGOLE  // utilisation de lcd avec circuit I2C Digole - PIC16F182
#define LCD_LIQIDCRYSTAL  // utilisation de lcd liquid crystal I2C - PCF8574
/*--------------------------------------------------------------------------------*/
/** progmem  mémoire flash */
const char listeDayWeek[] PROGMEM = "DimLunMarMerJeuVenSam"; // day of week en mémoire flash
const char affichageMenu[] PROGMEM = "      Date      .      Heure     .  Temperature   .  Lumiere LDR   .   Light Meter  . Tension bat N1 . Tension bat N2 .Recepteur_0     .Recepteur_1     .Recepteur_2     .Recepteur_3     ";
const char affichageBatterieFaible[] PROGMEM = "*** Batterie faible ! ***";
const char affichageBonjour[] PROGMEM = "Recepteur porte . Version 1.0.1  .Porte Poulailler.Manque carte RTC";
const char affichageTexte[] PROGMEM = "CFVH lux C;V;L;l;okErr SDErr fi" ;// petits textes

/** power and tools */
PowerTools tools (BUZZER_PIN, BUZZER); // Class PowerTools - objet tools
StructPowerAndTools outils;// structure

/** Accus */
Accus accusN1 (ACCU_N1, PIN_ACCUS_N1, ACCUS_TESION_MINIMALE, ACCUS_R1, ACCUS_R2, V_REFERENCE, MAX_CAD);// objet accusN1
Accus accusN2 (ACCU_N2, PIN_ACCUS_N2, ACCUS_TESION_MINIMALE, ACCUS_R1, ACCUS_R2, V_REFERENCE, MAX_CAD );// objet accusN2

/** lumiere */
Lumiere lum(PIN_LUMIERE, LDR_R2, V_REFERENCE, MAX_CAD); // Classe Lumiere  -  objet lum
/** lumiere BH1750 **/
LumBH1750 lightMeter(BH1750_I2C_ADDRESS); // Classe LumBH1750  -  objet lightMeter

/** interruptions */
StructInterruptions interrupt;// structure

/** menus */
Bouton bp(PIN_BP, DEBOUNCE); // class Bouton - objet bp
Bouton boitier(PIN_BOITIER, DEBOUNCE); // class Bouton - objet boitier
Clavier clavier(PIN_SENSOR_CLAVIER, DEBOUNCE, LIGNES_MENU ); // class Clavier - objet clavier
StructMenus menu; // structure

/**afficheurs lcd */
StructAffichage affi; // structure
/** LCD DigoleSerialI2C */
#ifdef  LCD_DIGOLE
/// I2C:Arduino UNO: SDA (data line) is on analog input pin 4, and SCL (clock line) is on analog input pin 5 on UNO and Duemilanove
#include "LcdDigoleI2C.h"
LcdDigoleI2C mydisp( &Wire, '\x27', COLONNES, DEBUG); // classe lcd digole i2c (lcd 2*16 caracteres) - objet mydisp
#endif
/** LCD PCF8574 */
#ifdef LCD_LIQIDCRYSTAL
#include "LcdPCF8574.h"
// Set the LCD address to 0x27 for a 16 chars and 2 line display pour pcf8574t / si pcf8574at alors l'adresse est 0x3f
LcdPCF8574  mydisp(0x3f, COLONNES, LIGNES);// Classe LcdPCF8574  -  objet mydisp
#endif

/** RTC_DS3231 */
tmElements_t tm; // declaration de tm pour la lecture des informations date et heure
HorlogeDS3232 rtc(ADRESSE_BOITIER_24C32, PIN_RTC_INT);// Classe HorlogeDS3232  -  objet rtc

/**  gestion  radio 433MHz  recepteur */
ReceiverRXB6 myRXB6; // class ReceiverRXB6 - objet myRXB6
StructRecepteurRXB6 receiver; // structure

/** gestion carte SD */
File monFichier;
StructCarteSD sd;// structure


/* setup */
void setup() {

  Serial.begin(9600);

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

  menu.g_incrementation = MENU_DATE; // position du  menu au demarrage
  deroulementMenu (menu.g_incrementation); // affichage du menu

  /*
    LOW : l’interruption est déclenchée quand la broche concernée est LOW. Comme il s’agit d’un état et non d’un événement, l’interruption sera déclenchée tant que la broche est LOW.
    Par conséquent, dès que l’ISR aura terminé son exécution, elle la recommencera. Pendant ce temps, loop() ne sera pas exécuté.
    CHANGE : l’interruption est déclenchée quand la broche concernée change d’état, c’est à dire passe de LOW à HIGH ou bien de HIGH à LOW. Il s’agit d’un événement.
    RISING : l’interruption est déclenchée quand la broche concernée passe de LOW à HIGH. Il s’agit également d’un événement.
    FALLING : l’interruption est déclenchée quand la broche concernée passe de HIGH à LOW. Il s’agit encore d’un événement.
  */
  attachInterrupt(1, myInterruptINT1, FALLING); // validation de l'interruption sur int1 (d3)
  attachInterrupt(0, myInterruptINT0, FALLING); // validation de l'interruption sur int0 (d2)

  tools.setupPower(); // initialisation power de l'arduino

  tools.setup_watchdog(9); // initialisation : maxi de 8 secondes pour l'it du watchdog

  // tools.setupBuzzer(1000); // initialisation du buzzer et test

  // initialisation des accus - initialisation commune
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

  if (!SD.begin(CHIP_SELECT)) {
    if (Serial) {
      // texte = "Err SD"
      Serial.println(tools.affTexteProgmem(affichageTexte, 19, 6));
    }
    sd.g_sdCard = false;
  } else {
    if (Serial) {
      // texte = "ok"
      Serial.println(tools.affTexteProgmem(affichageTexte, 17, 2));
    }
    sd.g_sdCard = true;
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

  outils.g_memoireLibre = freeMemory(); // calcul de la  memoire sram libre

  buildFileName();

  accusN1.accusFaible(); // verification de la tension batterie N1
  accusN2.accusFaible();// verification de la tension batterie N2

  routineTestFermetureBoitier(); // test fermeture boitier
  routineTestOuvertureBoitier();// test ouverture boitier

  routineInterruptionBp(); // routine interruption Bp

  int lux = lightMeter.readLightLevel();
  float temp = rtc.calculTemperature (TYPE_TEMPERATURE);

  routineReceptionMessage(lux, temp); // routine reception message et affichage

  routineGestionWatchdog(); // routine de gestion du watchdog
}


/* clavier */
///-----lecture clavier------
void  lectureClavier() {
  if (menu.g_boitierOuvert) {
    menu.g_touche = clavier.read_key(PIN_SENSOR_CLAVIER); /// read key sensor = A1
    clavier.relacheTouche(menu.g_touche, menu.g_relache);
    if (clavier.deplacementDansMenu(menu.g_touche, menu.g_relache, outils.g_reglage) or menu.g_touche == 4) {
      clavier.positionMenu(menu.g_incrementation, menu.g_touche); /// position du menu pour l'affichage
      deroulementMenu (menu.g_incrementation); /// affichage du menu
    }
  }
}

///---- temporisation pour l'affichage: affichage du menu lorsque temps est > boucleTemps-----
void temporisationAffichage(const int boucleTemps) {
  if (  affi.g_temps > boucleTemps and menu.g_touche != -1 /*and !monServo.get_m_servoAction()*/) {
    deroulementMenu (menu.g_incrementation); // affichage du menu en fonction de incrementation
    affi.g_temps = 0;
  } else {
    affi.g_temps += 1;
  }
}

///----- build fileName-----
void buildFileName() {
  RTC.read(tm); // lecture date et heure
  /*
     // espace à la place du zero - prend beacoup de place en memoire programme
    sprintf(fileName, "%d%d%d.txt", tm.Day, tm.Month, tm.Year + 1970);
  */
  // nom du fichier sur la carte SD : anneemoisjour.txt
  sd.g_fileName = "";// raz du nom de fichier
  sd.g_fileName += tm.Year + 1970; // année depuis 1970
  sd.g_fileName.concat(mydisp.transformation( "", tm.Month));// print mois
  sd.g_fileName.concat(mydisp.transformation( "", tm.Day));// print Day;
  sd.g_fileName += ".txt";
}

///----routine lecture et ecriture date and time-----
void ecritureDateTime() {
  RTC.write(tm);
  if (menu.g_incrementation == MENU_HEURE) {
    displayTime(); // affichage de l'heure
  }
  if (menu.g_incrementation == MENU_DATE) {
    displayDate(); // affichage de la date
  }
}

/* affichages */
///-----routine display Date-----
void displayDate() {
  if (menu.g_boitierOuvert) { // si le boitier est ouvert
    RTC.read(tm); // lecture date et heure
    // affichage du jour de la semaine
    char semaine[4];
    tools.clearChaine (semaine, 4);
    byte j = ((tm.Wday - 1) * 3);
    for (byte i = j; i < j + 3; i++) {
      semaine[i - j] = pgm_read_byte(listeDayWeek + i);
    }
    mydisp.affichageDateHeure(semaine, tm.Day, tm.Month, tm.Year);
  }
}

///-----routine display Time-----
void displayTime () {
  if (menu.g_boitierOuvert) { // si le boitier est ouvert
    RTC.read(tm); // lecture date et heure
    // texte = H
    char texte[2];
    tools.clearChaine (texte, 2);
    texte[0] = pgm_read_byte(affichageTexte + 3 );
    mydisp.affichageDateHeure(texte, tm.Hour, tm.Minute, tm.Second);
  }
}


///----- affichage du circuit BH1750 Light Meter -----
void affiLightMeter () {
  if (menu.g_boitierOuvert) { // si le boitier est ouvert
    uint16_t lux = lightMeter.readLightLevel();
    byte ligne = 1;
    bool nonReglable = 1; // pour afficher le curseur sur la premiere ligne car non reglable
    // texte = " lux"
    char texte[5];
    tools.clearChaine (texte, 5);
    for (byte i = 4; i < 8; i++) {
      texte[i - 4] = pgm_read_byte(affichageTexte +  i);
    }
    mydisp.affichageLumFinCourse(lux, ligne, texte, nonReglable);
  }
}

///-----routine lecture d'un message en plusieurs morceaux-----
void morceaux16caracteres (byte debut) {
  mydisp.cursorPosition(0, 1); // decalage, ligne
  for ( byte i = debut ; i <  debut + 16; i++) {
    testAndAffichage(i);// test et affichage du caractere
  }
}

///-----test et affichage-----
void testAndAffichage (byte positionDansTableau) {
  char caractere;
  if (receiver.g_message[positionDansTableau] == ';' or receiver.g_message[positionDansTableau] == '\0')  caractere = ' ' ; else caractere = receiver.g_message[positionDansTableau];
  mydisp.print(caractere);// valable pour digoleSerial et liquidCrystal
}

///-----affichage Recepteur-----
void affiRecepteur() {
  if (menu.g_boitierOuvert) { // si le boitier est ouvert
    mydisp.cursorPosition(12, 0); // decalage, ligne
    for (byte i = 0; i < 4; i++) {
      testAndAffichage(i); // test et affichage du caractere
    }
    if (menu.g_incrementation == MENU_RECEPTEUR_0) {
      byte debut = 5;
      morceaux16caracteres (debut);
    }
    if (menu.g_incrementation == MENU_RECEPTEUR_1) {
      byte debut = 21;
      morceaux16caracteres (debut);
    }
    if (menu.g_incrementation == MENU_RECEPTEUR_2) {
      byte debut = 37;
      morceaux16caracteres (debut);
    }
    if (menu.g_incrementation == MENU_RECEPTEUR_3) {
      byte debut = 53;
      morceaux16caracteres (debut);
    }
  }
}

/* afficheur */
///-----retro eclairage de l'afficheur-----
void eclairageAfficheur() {
  if (menu.g_boitierOuvert) {
    if (clavier.testTouche5(menu.g_touche, menu.g_relache)) {
      mydisp.retroEclairage();// retro eclairage
    }
  }
}

/* batteries */
///-------affichage tension batterie N1
void affiTensionBatCdes() {
  float voltage = accusN1.tensionAccus();// read the input on analog pin A6 : tension batterie N1
  // print out the value you read:
  if (menu.g_boitierOuvert) { // si le boitier est ouvert
    byte ligne = 1;
    // texte = V
    char texte[2] ;
    tools.clearChaine (texte, 2);
    texte[0] = pgm_read_byte(affichageTexte + 2 );
    mydisp.affichageVoltage(  voltage, texte,  ligne);
  }
}

///-------affichage tension batterie N2
void affiTensionBatServo() {
  float voltage = accusN2.tensionAccus();// read the input on analog pin A7 : tension batterie N2
  // print out the value you read:
  if (menu.g_boitierOuvert) { // si le boitier est ouvert
    byte ligne = 1;
    // texte = V
    char texte[2];
    tools.clearChaine (texte, 2);
    texte[0] = pgm_read_byte(affichageTexte + 2 );
    mydisp.affichageVoltage(  voltage, texte,  ligne);
  }
}

/* reglage de la date */
///----routine reglage jour semaine, jour, mois, annee-----
void reglageDate () {
  if (menu.g_boitierOuvert) {
    byte deplacement = 3;
    if (menu.g_incrementation == MENU_DATE) {
      mydisp.cursorPositionReglages (menu.g_touche, menu.g_relache, outils.g_reglage, 14, deplacement, 14);// position du cuseur pendant les reglages
    }
    if ((menu.g_touche == 2 or menu.g_touche == 3) and menu.g_incrementation == MENU_DATE and menu.g_relache == true and outils.g_reglage == true) { // si appui sur les touches 2 ou 3 pour reglage des valeurs
      menu.g_relache = false;
      if (mydisp.get_m_decalage() == deplacement) {
        tm.Wday = rtc.reglageHeure(menu.g_touche, tm.Wday, JOUR_SEMAINE);
        ecritureDateTime();
      }
      if (mydisp.get_m_decalage() == 2 * deplacement) {
        tm.Day = rtc.reglageHeure(menu.g_touche, tm.Day, JOUR);
        ecritureDateTime();
      }
      if (mydisp.get_m_decalage() == 3 * deplacement) {
        tm.Month = rtc.reglageHeure(menu.g_touche, tm.Month, MOIS);
        ecritureDateTime();
      }
      if (mydisp.get_m_decalage() == 4 * deplacement) {
        tm.Year = rtc.reglageHeure(menu.g_touche, tm.Year, ANNEE);
        ecritureDateTime();
      }
    }
  }
}


/* reglage time */
///-----routine reglage heure , minute , seconde-----
void reglageTime () {
  if (menu.g_boitierOuvert) {
    byte deplacement = 4;
    if (menu.g_incrementation == MENU_HEURE) {
      mydisp.cursorPositionReglages (menu.g_touche, menu.g_relache, outils.g_reglage, 14, deplacement, 14);// position du cuseur pendant les reglages
    }
    if ((menu.g_touche == 2 or menu.g_touche == 3) and menu.g_incrementation == MENU_HEURE and menu.g_relache == true and outils.g_reglage == true ) { // si appui sur les touches 2 ou 3 pour reglage des valeurs
      menu.g_relache = false;
      if (mydisp.get_m_decalage() == deplacement) {
        tm.Hour = rtc.reglageHeure(menu.g_touche, tm.Hour, HEURE);
        ecritureDateTime(); // routine écriture date and time
      }
      if (mydisp.get_m_decalage() == 2 * deplacement) {
        tm.Minute = rtc.reglageHeure(menu.g_touche, tm.Minute, MINUTES);
        ecritureDateTime();
      }
      if (mydisp.get_m_decalage() == 3 * deplacement) {
        tm.Second = rtc.reglageHeure(menu.g_touche, tm.Second, SECONDES);
        ecritureDateTime();
      }
    }
  }
}


/* temperature */
///-----routine lecture température sur ds3231 rtc type celsius=true ,fahrenheit=false------
void read_temp(const boolean typeTemperature) {
  float temp = rtc.calculTemperature (typeTemperature);//valeur de la temperature en fonction du type
  if ( menu.g_boitierOuvert) { // si le boitier est ouvert
    byte ligne = 1;
    // texte = C ou F
    char texte[2];
    tools.clearChaine (texte, 2);
    // texte[0] = pgm_read_byte(affichageTexte );
    // String texte = "";
    // if (typeTemperature) texte = "C"; else texte = "F";
    if (typeTemperature) texte[0] = pgm_read_byte(affichageTexte ); else texte[0] = pgm_read_byte(affichageTexte + 1 );
    mydisp.affichageVoltage(temp, texte, ligne);
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
  receiver.g_receiveMessage = true;
}

//-----routine interruption D3 INT1-----
void myInterruptINT1() {
  // rtc.testInterruptRTC(interruptRTC);// test de l'entree 5 - interruption du rtc
  bp.testInterruptionBp (interrupt.g_interruptBp);// test l'it du bp
  boitier.testInterruptionBoitier (interrupt.g_interruptOuvBoi);// test l'it de l'interrupteur du boitier
}

///-----routine interruption Bp-----
void routineInterruptionBp() {
  if (interrupt.g_interruptBp) { // test de l'appui sur bouton bp
    if (bp.testToucheBp ()) { //debounce pour le bp
      //...
    }
    bp.testRelacheBp(interrupt.g_interruptBp);// test du relache du bp
  }
}

///-----test ouverture boitier-----
void routineTestOuvertureBoitier()  {
  if ( boitier.testBoitierOuvert(interrupt.g_interruptOuvBoi, menu.g_boitierOuvert)) {
    menu.g_boitierOuvert = true; // boitier ouvert
    mydisp.gestionCurseur(1); // activation du curseur
    deroulementMenu (menu.g_incrementation); /// affichage du menu
  }
}

///-----test fermeture boitier-----
void  routineTestFermetureBoitier() {
  if (boitier.testBoitierFerme(interrupt.g_interruptOuvBoi, menu.g_boitierOuvert)) {
    mydisp.razLcd(); // extinction et raz du lcd
    menu.g_boitierOuvert = false; // boitier ferme
    interrupt.g_interruptOuvBoi = false; // autorisation de la prise en compte de l'IT
    mydisp.choixRetroEclairage (0);// extinction retro eclairage
  }
}

/* lumiere
  -affichage
*/
///-----routine lecture et affichage de la lumière-----
void lumiere() {
  unsigned int lumValue = lum.tensionLuminosite(); // luminosite CAD sur pin A0
  if (menu.g_boitierOuvert) { // si le boitier est ouvert
    byte ligne = 1;// première ligne car non reglable
    bool nonReglable = 1; // pour afficher le curseur sur la premiere ligne car non reglable
    // texte = " lux"
    char texte[5];
    tools.clearChaine (texte, 5);
    for (byte i = 4; i < 8; i++) {
      texte[i - 4] = pgm_read_byte(affichageTexte +  i);
    }
    mydisp.affichageLumFinCourse(lumValue, ligne, texte, nonReglable);
  }
}

///-----routine reception message-----
void routineReceptionMessage (int lux, float temp) {
  if (receiver.g_receiveMessage) {
    noInterrupts();

    if (myRXB6.reception(receiver.g_message, receiver.g_taille_message)) {
      if (Serial) {
        Serial.println("");
        Serial.print(tools.affTexteProgmem(affNumBoitier, 0, 6));
        Serial.print(temp);//valeur de la temperature
        // texte = " C;"
        Serial.print(tools.affTexteProgmem(affichageTexte, 9, 2));
        Serial.print(accusN1.tensionAccus());
        // texte = "V;"
        Serial.print(tools.affTexteProgmem(affichageTexte, 11, 2));
        Serial.print(accusN2.tensionAccus());
        // texte = "V;"
        Serial.print(tools.affTexteProgmem(affichageTexte, 11, 2));
        Serial.print(lum.tensionLuminosite());
        // texte = "L;"
        Serial.print(tools.affTexteProgmem(affichageTexte, 13, 2));
        Serial.print(lux);
        // texte = "l;"
        Serial.print(tools.affTexteProgmem(affichageTexte, 15, 2));
        // if (bat.g_batterieFaible) { // affichage si la batterie est faible
        if (accusN1.get_m_batterieFaible() or accusN2.get_m_batterieFaible()) { // affichage si la batterie est faible
          Serial.print(tools.affTexteProgmem(affichageBatterieFaible, 0, 25));
        }
        Serial.println("");
      }

      if (sd.g_sdCard) {
        // si le fichier que vous voulez ouvrir n'existe pas, il sera créé (8 caracteres maxi).
        monFichier = SD.open(sd.g_fileName, FILE_WRITE); // ouvre le fichier en mode ecriture
        //monFichier = SD.open("test.txt", FILE_READ); // ouvre le fichier en mode lecture
        //monFichier = SD.open("test.txt"); // ouvre le fichier en mode lecture

        // vérifier si le fichier est ouvert
        if (monFichier) {
          monFichier.print((char*) receiver.g_message);
          monFichier.println("");
          // numero de serie du boitier recepteur "N00x;"
          monFichier.print(tools.affTexteProgmem(affNumBoitier, 0, 6));
          //monFichier.print((char*)numeroSerieBoitier);
          monFichier.print(temp);//valeur de la temperature
          // texte = " C;"
          monFichier.print(tools.affTexteProgmem(affichageTexte, 9, 2));
          monFichier.print(accusN1.tensionAccus());
          // texte = "V;"
          monFichier.print(tools.affTexteProgmem(affichageTexte, 11, 2));
          monFichier.print(accusN2.tensionAccus());
          // texte = "V;"
          monFichier.print(tools.affTexteProgmem(affichageTexte, 11, 2));
          monFichier.print(lum.tensionLuminosite());
          // texte = "L;"
          monFichier.print(tools.affTexteProgmem(affichageTexte, 13, 2));
          monFichier.print(lux);
          // texte = "l;"
          monFichier.print(tools.affTexteProgmem(affichageTexte, 15, 2));
          // if (bat.g_batterieFaible) { // affichage si la batterie est faible
          if (accusN1.get_m_batterieFaible() or accusN2.get_m_batterieFaible()) { // affichage si la batterie est faible
            monFichier.print(tools.affTexteProgmem(affichageBatterieFaible, 0, 25));
          }
          monFichier.println("");
          monFichier.close(); // fermer le fichier
        } else {
          // si le fichier ne s'est pas ouvert :
          if (Serial) {
            // texte = "Err fi"
            Serial.println(tools.affTexteProgmem(affichageTexte, 25, 6));
          }
        }
      }
    }
    receiver.g_receiveMessage = false;
    receiver.g_message[VW_MAX_MESSAGE_LEN] = {0};
    receiver.g_taille_message = VW_MAX_MESSAGE_LEN;
    interrupts();
  }
}

/* menu */
///-----routine affichage menus------
void deroulementMenu (byte increment) {
  if (menu.g_boitierOuvert) {
    byte j = ((increment - 1) * (COLONNES + 1)); // tous les 16 caractères
    mydisp.cursorPosition(0, 0); // decalage, ligne
    for (byte i = j; i < j + COLONNES; i++) { // boucle pour afficher 16 caractères sur le lcd
      char   texteMenu = pgm_read_byte(affichageMenu + i); // utilisation du texte présent en mèmoire flash
      mydisp.print(texteMenu);// valable pour digoleSerial et liquidCrystal
    }
    switch (increment) { // test de la valeur de incrementation pour affichage des parametres
      case MENU_DATE: // Date
        displayDate(); // affichage de la date
        break;
      case MENU_HEURE: // heure
        displayTime(); // affichage de l'heure
        break;
      case MENU_TEMPERATURE: // temperature
        read_temp(TYPE_TEMPERATURE); // read temperature celsius=true
        break;
      case MENU_LUMIERE: //lumiere
        lumiere(); // lecture et affichage de la lumiere
        break;
      case MENU_LIGHTMETER:  // mesure du circuit BH1750
        affiLightMeter(); //
        break;
      case MENU_TENSION_BAT_N1:  // tension batterie commandes N1
        affiTensionBatCdes(); //
        break;
      case MENU_TENSION_BAT_N2:  // tension batterie servo N2
        affiTensionBatServo(); //
        break;
      case MENU_RECEPTEUR_0:  // recepteur - 0
        affiRecepteur(); //
        break;
      case MENU_RECEPTEUR_1:  // recepteur - 1
        affiRecepteur(); //
        break;
      case MENU_RECEPTEUR_2:  // recepteur - 2
        affiRecepteur(); //
        break;
      case MENU_RECEPTEUR_3:  // recepteur - 3
        affiRecepteur(); //
        break;
    }
  }
}

/* interruption du watchdog */
///----Watchdog Interrupt Service est exécité lors d'un timeout du WDT----
ISR(WDT_vect) {
  if (outils.g_flag_wdt == 0) {
    outils.g_flag_wdt = 1; // flag
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
  if ((outils.g_flag_wdt == 1 ) and (!menu.g_boitierOuvert)) { // si le boitier est ferme et watchdog=1
    //...
    outils.g_flag_wdt = 0;
    enterSleep(); //Revenir en mode veille
  }
}

///-----routine affichage au demarrage-----
void affichageDemarrage (byte colonne) {
  char chaine[16];
  tools.clearChaine (chaine, 16);
  char chaine_1[16];
  tools.clearChaine (chaine_1, 16);
  for (byte i = colonne; i < 16 + colonne; i++) { // boucle pour afficher 16 caractères sur le lcd
    chaine[i - colonne] = pgm_read_byte(affichageBonjour + i); // utilisation du texte présent en mèmoire flash
  }
  colonne += 17;
  for (byte i = colonne; i < 16 + colonne; i++) { // boucle pour afficher 16 caractères sur le lcd
    chaine_1[i - colonne] = pgm_read_byte(affichageBonjour + i); // utilisation du texte présent en mèmoire flash
  }
  mydisp.bonjour(chaine, chaine_1); // affichage version sur les deux lignes
}

