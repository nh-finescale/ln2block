//##########################################################################
//#
//#	LocoNet2Block		copyright: Michael Pfeil
//#
//#	Dieses Programm ist für die Hardware Loconet2Block geschrieben.
//# Hier wird die Funktionalität des Streckenblocks abgebildet.
//#	Als Vorlage dienten die State-Modelle von Stefan Bormann.
//#
//##########################################################################

//----------------------------------------------------------------------
//	The main version is defined by PLATINE_VERSION (compile_options.h)
//
//#define VERSION_MAIN		PLATINE_VERSION
#define	VERSION_MINOR		16
#define VERSION_BUGFIX		0


//##########################################################################
//#
//#		Version History:
//#
//#-------------------------------------------------------------------------
//#
//#	Version:	x.16.00		vom: 31.03.2022
//#
//#	Bugfix:
//#		-	change the handling of receiving block messages due to
//#			timing issues. Long messages like train number messages
//#			(and some times short messages too) were not read in correctly.
//#			Solution: put the decoding of the SLIP message in a
//#			state machine and only read bytes from the serial interface
//#			when they are available.
//#			 -	new function HandleBlockMessage()
//#			 -	new function CheckForBlockMessage()
//#			 -	changed function loop()
//#			 -	deleted function RecvSlipPacket()
//#
//#-------------------------------------------------------------------------
//#
//#	Version:	x.15.05		vom: 31.03.2022
//#
//#	Bugfix:
//#		-	correction of parameter usage for switching train number
//#			messages on / off
//#			 -	function MyLoconetClass::LoconetReceived()
//#		-	avoid evaluation of per loop-back received train number message
//#			 -	new member variable 'm_bIgnoreMsg' in MyLoconetClass
//#			 -	function MyLoconetClass::Init()
//#			 -	function MyLoconetClass::SendBlock2Station()
//#			 -	function MyLoconetClass::CheckForMessageAndStoreInDataPool()
//#
//#-------------------------------------------------------------------------
//#
//#	Version:	x.15.04		vom: 23.03.2022
//#
//#	Bugfix:
//#		-	Whenever a message "Erlaubnisabgabe" is seen in the state
//#			"ERLAUBNIS_STATE_ERHALTEN" the marker for the message
//#			will be cleared.
//#
//#-------------------------------------------------------------------------
//#
//#	Version:	x.15.03		vom: 05.03.2022
//#
//#	Implementation:
//#		-	change debug messages for block messages
//#
//#-------------------------------------------------------------------------
//#
//#	Version:	x.15.02		vom: 04.03.2022
//#
//#	Implementation:
//#		-	add address to enable handling of train number messages
//#		-	add flag that shows if handling of train number messages
//#			is enabled
//#
//#-------------------------------------------------------------------------
//#
//#	Version:	x.15.01		vom: 04.03.2022
//#
//#	Implementation:
//#		-	put indicator into ZN message which ZN field to address
//#
//#-------------------------------------------------------------------------
//#
//#	Version:	x.15.00		vom: 26.02.2022
//#
//#	Implementation:
//#		-	change display library to simple_oled_sh1106 lib
//#
//#-------------------------------------------------------------------------
//#
//#	Version:	x.14.01		vom: 26.02.2022
//#
//#	Implementation:
//#		-	change version numbering
//#			the main version is now defined by the board version
//#			minor version is for new features
//#			bugfix is for changes in a feature
//#		-	add address for annunciator field use for train numbers
//#
//#-------------------------------------------------------------------------
//#
//#	Version:	2.14	vom: 25.02.2022
//#
//#	Implementation:
//#		-	add new message handling for FREMO train numbers
//#
//#-------------------------------------------------------------------------
//#
//#	Version:	2.13	vom: 25.02.2022
//#
//#	Implementation:
//#		-	new config bit: SPLIT_PERMIT_INDICATOR_MSG
//#			if the bit is set then only one 'MELDER_ERLAUBNIS' message
//#			will be send during 'Erlaubniswechsel'
//#
//#-------------------------------------------------------------------------
//#
//#	Version: 2.12	vom: 16.01.2022
//#
//#	Fehlerbeseitigung:
//#		-	Das 'Merken' von Tasten-Nachrichten führte immer wieder zu
//#			Problemen, wie z.B.: 'automatisches' Auslösen eines State-
//#			Wechsels. Dieses Problem ist nun behoben.
//#
//#-------------------------------------------------------------------------
//#
//#	Version: 2.11	vom: 07.01.2022
//#
//#	Umsetzung:
//#		-	Anpassung an Platine Version 6
//#			Einbindung der DIP-Switches für die Konfiguration
//#
//#	Fehlerbeseitigung:
//#		-	Der Anrückmelder funktionierte nicht beim Rückblocken.
//#			Dieser Fehler ist nun beseitigt.
//#
//#-------------------------------------------------------------------------
//#
//#	Version: 2.10	vom: 05.01.2022
//#
//#	Fehlerbeseitigung:
//#		-	Die Ansteuerung des Anrückmelders war nicht in Ordnung.
//#			Sie funktioniert jetzt wie gewünscht.
//#
//#-------------------------------------------------------------------------
//#
//#	Version: 2.09	vom: 29.12.2021
//#
//#	Umsetzung:
//#		-	Reset per Taster eingebaut.
//#		-	Block ON / OFF eingebaut.
//#			Dafür auch eine neue Funktion erstellt:
//#				SwitchBlockOff()
//#			Weiter wurde der Initialisierungsvorgang angepasst
//#			 -	Funktion: Setup()
//#		-	Die Grüne LED zeigt nun "Block belegt" an
//#		-	Wenn der Programmiermodus beendet wird, wird nun ein RESET
//#			ausgelöst.
//#
//#	Fehlerbeseitigung:
//#		-	Richtungsbetrieb funktionierte nicht korrekt, da die Erlaubnis
//#			nicht ordnungsgemäß abgegeben wurde, bzw. die internen Zustände
//#			für die abgegebene Erlaubnis nicht richtig eingestellt wurden.
//#
//#-------------------------------------------------------------------------
//#
//#	Version: 2.08	vom: 07.12.2021
//#
//#	Umsetzung:
//#		-	Richtungsbetrieb eingebaut
//#			Wenn der Richtungsbetrieb aktiv ist, dann wird, wenn erlaubt,
//#			jede Sekunde die Erlaubnis abgegeben.
//#			Sollte eine Erlaubnis-Abgabe-Nachricht empfangen werden, so
//#			wird die Erlaubnis direkt wieder abgegeben.
//#
//#-------------------------------------------------------------------------
//#
//#	Version: 2.07	vom: 01.12.2021
//#
//#	Umsetzung:
//#		-	Platz geschaffen für neue Konfigurations-LNCVs.
//#		-	3 LNCVs für Timer (Entry, Exit, Contact) und die entsprechenden
//#			Zugriffsfunktionen hinzugefügt.
//#			(Default-Zeiten der Timer = 1s bzw. 1000ms)
//#		-	Bei der Nutzung des internen Kontaktes wird nun darauf
//#			geachtet, ob die Information invertiert werden soll oder nicht.
//#			(Funktion: InterpretData() )
//#		-	Ein Timer für den Freimeldung des internen Kontaktes
//#			eingebaut. Die Freimeldung wird dabei erst nach Ablauf des
//#			Timers gesendet. Gestartet wird der Timer, wenn der interne
//#			Kontakt 'frei' meldet. Sollte der interne Kontakt wieder
//#			'belegt' melden, bevor der Timer abgelaufen ist, dann wird
//#			der Timer angehalten und auf '0' gesetzt (re-trigger)
//#		-	In der Funktion SendMessageWithInAdr() die Information
//#			nicht invertieren, wenn der interne Kontakt benutzt wird und
//#			es sich um eine Einfahr-Kontakt-Nachricht handelt.
//#
//#	Fehlerbeseitigung:
//#		-	Im Zustand "ANFANGSFELD_STATE_FLUEGEL_KUPPLUNG" wird nun für
//#			den Felderblock die Auswertung der Nachrichten richtig
//#			verarbeitet.
//#		-	Im State ENDFELD_STATE_ERSTE_ACHSE wird der Timer nun richtig
//#			behandelt:
//#			Gestartet wird der Timer immer, wenn eine Gleiskontakt-Frei-
//#			Nachricht eingetroffen ist.
//#			Wenn der Timer als re-triggerbar konfiguriert ist, dann wird
//#			bei einer Gleiskontakt-Belegt-Nachricht der Timer angehalten
//#			und zurückgesetzt, falls der Timer nicht vorher abgelaufen war.
//#
//#-------------------------------------------------------------------------
//#
//#	Version: 2.06	vom: 15.11.2021
//#
//#	Umsetzung:
//#		-	Ein neues Konfigurationsbit "FELDERBLOCK" eingebaut.
//#			Wenn das Bit gesetzt ist, wird im Anfangsfeld im Zustand
//#			"ANFANGSFELD_STATE_FLUEGEL_KUPPLUNG" kein automatischer
//#			Vorblock mehr gesendet. Stattdessen wird darauf gewartet,
//#			dass der Gleiskontakt wieder frei ist und dann
//#			"Hilfsvorblock" empfangen wurde.
//#
//#-------------------------------------------------------------------------
//#
//#	Version: 2.05	vom: 14.11.2021
//#
//#	Umsetzung:
//#		-	Im State ENDFELD_STATE_ERSTE_ACHSE wird bei jeder Gleiskontakt-
//#			Belegt-Nachricht der Timer für GERAEUMT neu gestartet.
//#
//#-------------------------------------------------------------------------
//#	Version: 2.04	vom: 12.11.2021
//#
//#	Umsetzung:
//#		-	Das Display kann nun um 180 Grad gedreht werden.
//#			Gesteuert wird dies durch das Konfigurationsbit 'DISPLAY_FLIP'.
//#			Ist das Bit gesetzt, wird das Display um 180 Grad gedreht.
//#
//#-------------------------------------------------------------------------
//#	Version: 2.03	vom: 28.10.2021
//#
//#	Umsetzung:
//#		-	Neue LNCV #9 für eine Wartezeit zwischen zwei zu sendenden
//#			Loconet-Nachrichten eingebaut.
//#			Beim Einlesen der Konfigurationswerte wird darauf geachtet,
//#			dass die Wartezeit nicht kürzer als 5 ms ist.
//#			Die Default Wartezeit ist 10 ms
//#
//#-------------------------------------------------------------------------
//#	Version: 2.02	vom: 13.10.2021
//#
//#	Fehlerbeseitigung:
//#		-	Im Zustand "ANFANGSFELD_STATE_FAHRT" war die Abfrage für
//#			Einfahrkontakt oder Ausfahrkontakt fehlerhaft.
//#			Dies ist nun korrigiert.
//#
//#-------------------------------------------------------------------------
//#	Version: 2.01	vom: 07.10.2021
//#
//#	Fehlerbeseitigung:
//#		-	Das Flag für das Senden der Erlaubnisabgabe wurde falsch
//#			gesetzt. Dieser Fehler ist behoben.
//#
//#-------------------------------------------------------------------------
//#	Version: 2.00	vom: 12.08.2021
//#
//#	Umsetzung:
//#		-	Anpassung an Platinen Version 5
//#		-	Auftrennung der INO-Datei in mehrere Dateien für
//#			die einzelnen Klassen
//#		-	Überarbeitung der Debug-Ausgaben
//#		-	Auswertung der Block-Nachrichten überarbeitet
//#
//#-------------------------------------------------------------------------
//#	Version: 1.11	vom: 08.08.2021
//#
//#	Umsetzung:
//#		-	Anzeige der Platinenversion im Titel
//#		-	Die Zeit zwischen den Zuständen '1. Achse' und 'Geräumt' kann
//#			zwischen 10s und 5s konifguriert werden.
//#
//#-------------------------------------------------------------------------
//#	Version: 1.10	vom: 06.08.2021
//#
//#	Umsetzung:
//#		-	Anbindung eines Schlüsselkastens.
//#			Konfigurierbar, ob der Schlüsselkasten direkt angesprochen,
//#			oder ob erst der Fahrdienstleiter im Bahnhof "gefragt" wird.
//#
//#	Fehlerbeseitigung:
//#		-	Bei den Tasten wurde auf 'Direction' geachtet, was dazu
//#			führte, dass manchmal eine Taste nicht erkannt wurde.
//#
//#-------------------------------------------------------------------------
//#	Version: 1.09	vom: 05.08.2021
//#
//#	Fehlerbeseitigung:
//#		-	Bei der Umstellung der Funktion 'SendLncvMessage' von Adressen
//#			auf Index wurde dies beim Anrückmelder nicht beachtet.
//#
//#-------------------------------------------------------------------------
//#	Version: 1.08	vom: 04.08.2021
//#
//#	Umsetzung:
//#		-	Konfigurierbarkeit, ob Loconet2Block oder der Stelltisch
//#			den Anrückmelder ansteuert.
//#		-	Umstellung der Konfigurationsabfrage von 'Nummer' auf 'Maske'.
//#
//#	Fehlerbeseitigung:
//#		-	Beim Starten wurde nicht darauf geachtet, ob E-Sig bzw. A-Sig
//#			als Sensor oder Switch konfiguriert sind.
//#			Dies kann im Augenblick nicht behoben werden, da es
//#			anscheinend keine Abfrage-Nachricht für Sensoren gibt.
//#			Als Workaround kann die Prüfschleife auf 'OK' konfiguriert
//#			werden.
//#
//#-------------------------------------------------------------------------
//#	Version: 1.07	vom: 02.08.2021
//#
//#	Umsetzung:
//#		-	Vor dem Abfragen von Tasten werden die entsprechenden Flags
//#			zurückgesetzt, egal, ob das Flag gesetzt war oder nicht.
//#		-	Per DEFINE kann die Ansteuerung der Hupe umgestellt werden,
//#			ob Loconet2Block 3 x hupt oder der Stelltisch
//#		-	Konfigurierbarkeit aller Loconet Befehle:
//#			-	Switch => bit ist '0'
//#			-	Sensor => bit ist '1'
//#			In LNCV #3 werden die empfangenen Nachrichten konfiguriert
//#			In den LNCVs #4 und #5 werden die zu sendenden Nachrichten
//#			konfiguriert.
//#
//#-------------------------------------------------------------------------
//#	Version: 1.06	vom: 27.07.2021
//#
//#	Umsetzung:
//#		-	Konfigurierbarkeit der empfangenen Loconet Befehle, ob
//#			Switch (Bit ist '0') oder Sensor (Bit ist '1')
//#
//#-------------------------------------------------------------------------
//#	Version: 1.05	vom: 24.07.2021
//#
//#	Fehlerbeseitigung:
//#		-	Bei der Umstellung der LNCV Adressen auf lückenlose Reihe
//#			ist die Adressüberprüfung und -berechnung nicht umgestellt
//#			worden.
//#			Dieser Fehler wurde behoben.
//#
//#-------------------------------------------------------------------------
//#	Version: 1.04	vom: 23.07.2021
//#
//#	Umsetzung:
//#		-	Umstellung auf lückenlose LNCVs
//#
//#	Fehlerbeseitigung:
//#		-	Das Flag für die Betätigung der Rückblocktasten blieb ständig
//#			aktiv. Das führte beim Endfeld im Zustand 'geräumt' zum
//#			direkten Rückblocken, wenn das Einfahrsignal wieder auf Hp0
//#			gestellt wurde.
//#			Das Flag wird nach der Auswertung nun automatisch
//#			zurückgesetzt.
//#		-	Manche Textausgaben werden nicht angezeigt.
//#			Positionierung der Textausgaben bei Anfangs- und Endfeld
//#			wieder direkt bei den Print-Befehlen plaziert.
//#
//#-------------------------------------------------------------------------
//#	Version: 1.03	vom: 10.07.2021
//#
//#	Umsetzung:
//#		- Integration des internen Gleiskontaktes
//#
//#-------------------------------------------------------------------------
//#	Version: 1.02	vom: 10.07.2021
//#
//#	Umsetzung:
//#		- Integration des Schlüsselinterfaces.
//#		- Einführung eines Konfigurations-Wortes
//#		- Über das oberste Bit der Adresse kann entschieden werden,
//#			ob eine Sensor- oder eine Switch-Message geschickt wird.
//#			(Sensor-Message => Bit gesetzt)
//#
//#-------------------------------------------------------------------------
//#	Version: 1.01	vom: 01.07.2021
//#
//#	Anpassung an die Block-Platine Version 4, d.h.: andere I/O-Belegung.
//#
//#-------------------------------------------------------------------------
//#	Version: 1.0	vom: 07.06.2021
//#
//#	Umgesetzt sind in dieser Version
//#		- der TF71-Streckenblock
//#		- der Relaisblock
//#	Der Schlüsselschalter für eine Awanst ist noch nicht berücksichtigt.
//#
//##########################################################################


//==========================================================================
//
//		I N C L U D E S
//
//==========================================================================

#include "compile_options.h"

#ifdef DEBUGGING_PRINTOUT
#include "debugging.h"
#endif

#include "io_control.h"
#include "lncv_storage.h"
#include "data_pool.h"
#include "my_loconet.h"
#include "erlaubnis.h"
#include "anfangsfeld.h"
#include "endfeld.h"
#include "block_msg.h"


//==========================================================================
//
//		D E F I N I T I O N S
//
//==========================================================================

#define BUFFER_LEN		30


typedef enum slip_state
{
	SS_Receive = 0,
	SS_Escape,
	SS_End
	
}	slip_state_t;


//==========================================================================
//
//		G L O B A L   V A R I A B L E S
//
//==========================================================================

uint32_t	g_ulMillisRepeat	= 0;
uint8_t		g_iOffCounter		= 3;
bool		g_bIsProgMode		= false;

//----------------------------------------------------------------------
//	Variable für das Block Interface
//
uint8_t g_uiBlockMessageCodes[] =
{
	BLOCK_MSG_VORBLOCK,
	BLOCK_MSG_RUECKBLOCK,
	BLOCK_MSG_ERLAUBNIS_ABGABE,
	BLOCK_MSG_ERLAUBNIS_ANFRAGE,
	BLOCK_MSG_VORBLOCK_ACK,
	BLOCK_MSG_RUECKBLOCK_ACK,
	BLOCK_MSG_ERLAUBNIS_ABGABE_ACK,
	BLOCK_MSG_ERLAUBNIS_ANFRAGE_ACK
};

uint8_t	g_usSendBuffer[] = { 0xC0, 0x2A, 0xC0, 0xC0 };
uint8_t	g_usRecvBuffer[ BUFFER_LEN ];

slip_state_t	g_SlipState		= SS_Receive;
uint8_t			g_usReceived	= 0;
uint8_t			g_usInByte;


//==========================================================================
//
//		G L O B A L   F U N C T I O N S
//
//==========================================================================

void (*resetFunc)( void ) = 0;


//**********************************************************************
//	HandleBlockMessage
//
void HandleBlockMessage( void )
{
#ifdef DEBUGGING_PRINTOUT
	g_clDebugging.PrintReceiveBlockMsg( g_usRecvBuffer[ 0 ] );
#endif

	switch( g_usRecvBuffer[ 0 ] )
	{
		case BLOCK_MSG_VORBLOCK:
			g_clDataPool.SetBlockMessageState( 1 << DP_BLOCK_MESSAGE_VORBLOCK );
			SendBlockMessage( DP_BLOCK_MESSAGE_VORBLOCK_ACK );
			break;

		case BLOCK_MSG_RUECKBLOCK:
			g_clDataPool.SetBlockMessageState( 1 << DP_BLOCK_MESSAGE_RUECKBLOCK );
			SendBlockMessage( DP_BLOCK_MESSAGE_RUECKBLOCK_ACK );
			break;

		case BLOCK_MSG_ERLAUBNIS_ABGABE:
			SendBlockMessage( DP_BLOCK_MESSAGE_ERLAUBNIS_ABGABE_ACK );

			if( !g_clLncvStorage.IsConfigSet( RICHTUNGSBETRIEB ) )
			{
				g_clDataPool.SetBlockMessageState( 1 << DP_BLOCK_MESSAGE_ERLAUBNIS_ABGABE );
			}
			break;

		case BLOCK_MSG_ERLAUBNIS_ANFRAGE:
			g_clDataPool.SetBlockMessageState( 1 << DP_BLOCK_MESSAGE_ERLAUBNIS_ANFRAGE );
			SendBlockMessage( DP_BLOCK_MESSAGE_ERLAUBNIS_ANFRAGE_ACK );
			break;

		case BLOCK_MSG_BROADCAST:
			if(		g_clDataPool.IsTrainNoEnabled()
				&&	g_clLncvStorage.IsConfigSet( TRAIN_NUMBERS ) )
			{
				g_clDataPool.ReceiveTrainNoFromBlock( g_usRecvBuffer );
			}
			break;

		default:
			break;
	}
}


//**********************************************************************
//	SendBlockMessage
//
bool CheckForBlockMessage( void )
{
	while( Serial.available() )
	{
		g_usInByte = (uint8_t)Serial.read();

		switch( g_SlipState )
		{
			case SS_Receive:
				switch( g_usInByte )
				{
					case SLIP_END:
						if( 0 < g_usReceived )
						{
							return( true );
						}
						break;

					case SLIP_ESC:
						g_SlipState = SS_Escape;
						break;

					default:
						if( g_usReceived < BUFFER_LEN )
						{
							g_usRecvBuffer[ g_usReceived ] = g_usInByte;
							g_usReceived++;
						}
						break;
				}
				break;

			case SS_Escape:
				if( g_usReceived < BUFFER_LEN )
				{
					if( SLIP_ESC_END == g_usInByte )
					{
						g_usInByte = SLIP_END;
					}
					else	//	SLIP_ESC_ESC == g_usInByte
					{
						g_usInByte = SLIP_ESC;
					}

					g_usRecvBuffer[ g_usReceived ] = g_usInByte;
					g_usReceived++;
				}

				g_SlipState = SS_Receive;
				break;
		}
	}
	
	return( false );
}


//**********************************************************************
//	SendBlockMessage
//
void SendBlockMessage( uint8_t msgIdx )
{
	g_usSendBuffer[ 1 ] = g_uiBlockMessageCodes[ msgIdx ];

	Serial.write( g_usSendBuffer, 3 );

#ifdef DEBUGGING_PRINTOUT
	g_clDebugging.PrintSendBlockMsg( g_usSendBuffer[ 1 ] );
#endif
}


//-----------------------------------------------------------------
//	CheckForBlockOutMessages
//
void CheckForBlockOutMessages( void )
{
	uint8_t uiMask	= 0x01;
	uint8_t idx		= 0;
	
	while( 0 < g_clDataPool.GetSendBlockMessage() )
	{
		if( g_clDataPool.IsSendBlockMessage( uiMask ) )
		{
			SendBlockMessage( idx );
		}

		idx++;
		uiMask <<= 1;
	}
}


//**********************************************************************
//	setup
//
void setup()
{
	// put your setup code here, to run once:
#ifdef DEBUGGING_PRINTOUT
	g_clDebugging.Init();
#endif

	g_clControl.Init();
	Serial.begin( 9600 );
	g_clMyLoconet.Init();
	g_clDataPool.Init();

	//----	some setup tests  --------------------------------------
#ifdef DEBUGGING_PRINTOUT
	g_clDebugging.PrintTitle( PLATINE_VERSION, VERSION_MINOR, VERSION_BUGFIX, false );
	g_clDebugging.PrintInfoLine( infoLineInit );
#endif

	delay( 200 );

#ifdef DEBUGGING_PRINTOUT
	g_clDebugging.PrintInfoLine( infoLineLedTest );
#endif

	g_clControl.Test( 300 );

	//----	LNCV: Check and Init  ----------------------------------
	g_clLncvStorage.CheckEEPROM();

	delay( 1000 );

	g_clLncvStorage.Init();

	delay( 200 );

	//----	Prepare Display  ---------------------------------------
#ifdef DEBUGGING_PRINTOUT
	bool flipDisplay = g_clLncvStorage.IsConfigSet( DISPLAY_FLIP );
	
	g_clDebugging.PrintTitle( PLATINE_VERSION, VERSION_MINOR, VERSION_BUGFIX, flipDisplay );
	g_clDebugging.PrintInfoLine( infoLineFields );
#endif

	//----	Prepare Block  -----------------------------------------
	if( g_clLncvStorage.IsBlockOn() )
	{
		g_clControl.BlockEnable();

		if( g_clLncvStorage.IsConfigSet( PRUEFSCHLEIFE_OK ) )
		{
			g_clDataPool.SetInState(	((uint16_t)1 << DP_E_SIG_SEND)
									|	((uint16_t)1 << DP_A_SIG_SEND) );
		}
		else
		{
			g_clMyLoconet.AskForSignalState();
		}

		g_clDataPool.SetOutStatePrevious(	OUT_MASK_FAHRT_MOEGLICH
										|	OUT_MASK_NICHT_ZWANGSHALT );
	}
	else
	{
		g_clDataPool.SwitchBlockOff();
	}

	//----	Repeat Timer starten  ----------------------------------
	g_ulMillisRepeat = millis() + cg_ulInterval_2_s;
}


//**********************************************************************
//	loop
//
void loop()
{
	//==================================================================
	//	Read Inputs
	//	-	Loconet messages
	//	-	Block messages
	//	-	Input signals
	//	all inputs will be stored in the 'data pool'
	//
	g_clMyLoconet.CheckForMessageAndStoreInDataPool();

	if( CheckForBlockMessage() )
	{
		HandleBlockMessage();

		//----------------------------------------------------------
		//	!!	ATTENTION  !!
		//	don't forget to 'reset' the receive buffer after
		//	handling the message !
		//
		g_usReceived = 0;
	}


	//==================================================================
	//	Auswertung der Daten und logische Verknüpfungen abarbeiten.
	//	Die Ergebnisse landen wieder im 'data_pool'.
	//
	if( g_clDataPool.InterpretData() )
	{
		resetFunc();
	}

	//--------------------------------------------------------------
	//	Wurde der Programmier-Modus beendet ?
	//
	if( g_clDataPool.IsProgMode() != g_bIsProgMode )
	{
		if( g_bIsProgMode )
		{
			resetFunc();
		}
		else
		{
			g_bIsProgMode = true;
		}
	}


	//==================================================================
	//	Prüfen, ob wiederkehrende Aktionen ausgeführt werden sollen.
	//	Z.B.:	im Richtungsbetrieb die Erlaubnis abgeben oder
	//			im BLOCK OFF Zustand die Ausfahrsignale freigeben.
	//
	if( millis() > g_ulMillisRepeat )
	{
		if( g_clLncvStorage.IsBlockOn() )
		{
			//----------------------------------------------------------
			//	Im Richtungsbetrieb wird alle zwei Sekunden
			//	die Erlaubnis abgegeben.
			//
			if( g_clLncvStorage.IsConfigSet( RICHTUNGSBETRIEB ) )
			{
				g_clDataPool.SetInState( IN_MASK_BEDIENUNG_ERLAUBNISABGABE );
				g_clDataPool.ClearOutStatePrevious( OUT_MASK_MELDER_ERLAUBNIS_ABGEGEBEN );
				g_clDataPool.SetOutStatePrevious(	OUT_MASK_MELDER_ERLAUBNIS_ERHALTEN );
			}
		}
		else
		{
			//----------------------------------------------------------
			//	Im BLOCK OFF Zustand die Ausfahrsignale freigeben und
			//	Ausfahrten ermöglichen und ein paar mal alle Melder
			//	und Anzeigen ausschalten.
			//
			g_clDataPool.ClearOutStatePrevious(		OUT_MASK_FAHRT_MOEGLICH
												|	OUT_MASK_NICHT_ZWANGSHALT
												|	OUT_MASK_SCHLUESSELENTNAHME_MOEGLICH );

			if( 0 < g_iOffCounter )
			{
				g_iOffCounter--;
				
				g_clDataPool.SetOutStatePrevious(	OUT_MASK_AUSFAHRSPERRMELDER_TF71
												|	OUT_MASK_BLOCKMELDER_TF71
												|	OUT_MASK_WIEDERHOLSPERRMELDER_RELAISBLOCK
												|	OUT_MASK_VORBLOCKMELDER_RELAISBLOCK
												|	OUT_MASK_RUECKBLOCKMELDER_RELAISBLOCK
												|	OUT_MASK_MELDER_ANSCHALTER
												|	OUT_MASK_MELDER_ERSTE_ACHSE
												|	OUT_MASK_MELDER_GERAEUMT
												|	OUT_MASK_MELDER_GERAEUMT_BLINKEN
												|	OUT_MASK_UEBERTRAGUNGSSTOERUNG
												|	OUT_MASK_MELDER_ERLAUBNIS_ERHALTEN
												|	OUT_MASK_MELDER_ERLAUBNIS_ABGEGEBEN );
			}
		}

		//--------------------------------------------------------------
		//	Timer neu starten
		//	Im "Block Off" Zustand werden Nachrichten alle 5 Sekunden,
		//	im "Block ON" Zustand alle 2 Sekunden gesendet.
		//
		if( 0 == g_iOffCounter )
		{
			g_ulMillisRepeat = millis() + cg_ulInterval_5_s;
		}
		else
		{
			g_ulMillisRepeat = millis() + cg_ulInterval_2_s;
		}
	}

	//==================================================================
	//	Die State-Maschinen abarbeiten
	//
	erlaubnis_state_t	erlaubnisState	= g_clErlaubnis.CheckState();

	if( ERLAUBNIS_STATE_ERHALTEN == erlaubnisState )
	{
		g_clAnfangsfeld.CheckState();
	}

	if( ERLAUBNIS_STATE_ABGEGEBEN == erlaubnisState )
	{
		g_clEndfeld.CheckState();
	}
	
	//==================================================================
	//	Auswerten, ob Melder oder sonstiges über das Loconet
	//	gesendet werden sollen und/oder ob Blocknachrichten
	//	über das Kabel gesendet werden sollen.
	//
	if(		g_clDataPool.IsTrainNoEnabled()
		&&	g_clLncvStorage.IsConfigSet( TRAIN_NUMBERS ) )
	{
		if( g_clDataPool.IsNewMsgStation2Block() )
		{
			uint8_t	*pBuffer = g_clDataPool.GetStation2Block();

			Serial.write( pBuffer, g_clDataPool.GetTrainNoStation2BlockLen() );
			
#ifdef DEBUGGING_PRINTOUT
			g_clDebugging.PrintSendBlockMsg( *(pBuffer + 1) );
#endif

			*pBuffer = 0x00;
		}
	}

	CheckForBlockOutMessages();
	g_clDataPool.CheckForOutMessages();
}
