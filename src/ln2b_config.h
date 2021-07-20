//
// Block messages
//
#define VORBLOCK            42
#define RUECKBLOCK          43
#define ERLAUBNISABGABE     44
#define ERLAUBNISANFRAGE    45
#define ACHSSENSOR          46
#define ESIG                47
#define CONFIG              48
#define AZGT                49
#define BLOCK_ZUSTAND       50
#define BLOCK_BEFEHL        51
#define ANSTOSSMELDUNG      52
#define GSKZ                53
#define VORBLOCK_ACK        142
#define RUECKBLOCK_ACK      143
#define ERLAUBNISABGABE_ACK 144
#define ERLAUBNISANFRAGE_ACK  145
#define END                 192    // indicates end of packet
#define ESC                 219    // indicates byte stuffing
#define ESC_END             220    // ESC ESC_END means END data byte
#define ESC_ESC             221    // ESC ESC_ESC means ESC data byte

#define RX_INPUT_PIN    0 
#define RX2_INPUT_PIN   5 
#define LN_TX_PIN       7 

#define KEINER 0
#define ERHALTEN 1
#define ABGEGEBEN 2
#define HOLEN 3

#define BLOCK_FREE  1
#define BLOCK_OCC 2

#define On true
#define Off false

/*
 * endfeldState values
 */
#define frei_boot 0
#define frei 1
#define belegt 2
#define signal_gezogen 3
#define anschalter_aktiv 4 
#define erste_achse 5
#define geraeumt 6 

/*
 * anfangsfeldState values
 */
//#define frei 1
//#define belegt 2
#define fahrt 3
#define fluegel_kupplung 4
#define einfahrsignal 5
#define fahrt_pre 6

/*
 * eventnumbers
 */
#define AusfahrtSignalFahrt 11
#define AusfahrtSignalHalt 12
#define GleiskontaktBefahren 13
#define ErlaubnisAbgabeTasteBedient 14 //EaT_BlGT
#define EinfahrtSignalFahrt 15
#define EinfahrtSignalHalt 16
#define RbT_BlGT_Bedient 17 //RbT-BlGT
//#define GleiskontaktExitBefahren 18
#define VbHT_BlGTBedient 19 //VbHT_BlGT
#define AsT_RbT_Bedient 20 //AsT_RbT
#define AsLT_RbT_Bedient 21 //AsLT_RbT

/*
 * led ports
 */
#define LED_BLOCK_POWER_BLUE       A0
#define LED_YELLOW_BLOCKING_HERE   A1
#define LED_RED_BLOCK_OCCUPIED     A2
#define LED_GREEN_BLOCK_ACTIVE     A3

/*
 * LNCV
 */

#define VERSION 5
#define ARTNR 1532 //enter 15320 in article ID on Intellibox
#define LNCV_COUNT 24

#define LNUStoer 2              //Sensor, output to panel High=Stoer, Low = OK
#define LNASpM 4                //switch, Ausfahrsperre blauer klaus Tf71, output to panel
#define LNBlockMelder 5         //sensor, output to panel 
#define LNWSpM 6                //switch, Wiederholsperre blauer klaus RB, output to panel
#define LNVorblockMelder 7      //sensor, ouput
#define LNRueckBlockMelder 8    //sensor, ouput
#define LNAnschaltMelder  9     //sensor, output
#define LNRaeumungsMelder 11    //sensor, output
#define LNWecker 12             //sensor, output to panel
#define LNTastensperre_EaT 13   //to lock EaT button on SMF Panel (LNCV 38 of the SMF EaT module)
#define LNErlaubnisAbgabeRichting 14 //Erlaubnismelder is coupled to EaT direction state/arrow on panel (LNCV 87 of EaT module)
