//##########################################################################
//#	BlockPlatine		copyright: Michael Pfeil
//#
//#	Dieses Programm ist für die Hardware Loconet2Block geschrieben.
//# Hiermit wird die Hardware getestet, also die LEDs und Eingänge.
//#
//#-------------------------------------------------------------------------
//#
//#	Der Ablauf für den Hardware-Test ist wie folgt:
//#
//#	SETUP:
//#		Alle LEDs werden ein- und wieder ausgeschaltet.
//#
//#	LOOP:
//#		Übertragungsstörung:
//#			Die rote LED dient zur Anzeige der Übertragungsstörung.
//#			Wenn kein Sub-D-Kabel zu einer Blockkiste eingesteckt ist,
//#			dann leuchtet die rote LED.
//#			Ist ein Kabel zu einer eingeschalteten Blockkiste eingesteckt,
//#			dann erlischt die rote LED.
//#
//#		Taster "BLK_ON_OFF":
//#			Bei einem kurzen Druck auf den Taster wird zwischen
//#			2 Zuständen umgeschaltet:
//#				Block On:	die gelbe LED ist aus und die grüne LED an.
//#							der Text "BlkOnOff: ON" wird ausgegeben.
//#				Block Off:	die gelbe LED ist an und die grüne LED aus.
//#							der Text "BlkOnOff: OFF" wird ausgegeben.
//#			Bei einem langen Druck wird ein "Reset" ausgelöst.
//#			In diesem Fall werden die gelbe und grüne LED ausgeschaltet
//#			und der Text "BlkOnOff: Reset" wird ausgegeben.
//#
//#		KEY_IN:
//#			Solange der KEY_IN-Eingang auf Masse gezogen ist,
//#			wird die KEY_LED und das KEY_RELAIS eingeschaltet und
//#			der Text "KeyIn: IN" wird ausgegeben.
//#			Im anderen Fall werden KEY_LED und KEY_RELAIS ausgeschaltet
//#			und der Text "KeyIn: OUT" ausgegeben.
//#
//#		CONTACT:
//#			Solange der CONTACT-Eingang auf Masse gezogen wird,
//#			wird der Text "Contact: Close" ausgegeben.
//#			Im anderen Fall wird "Contact: Free" ausgegeben.
//#
//#-------------------------------------------------------------------------
//#	Version: 1.00	vom: 09.08.2021
//#
//#	Umsetzung:
//#		-	siehe Text oben
//#
//##########################################################################

#define VERSION_MAIN	1
#define	VERSION_MINOR	0


//==========================================================================
//
//		I N C L U D E S
//
//==========================================================================

#include "compile_options.h"

#include <LocoNet.h>

#ifdef DEBUGGING_PRINTOUT
#include "debugging.h"
#endif

#include "io_control.h"


//==========================================================================
//
//		K O N S T A N T E
//
//==========================================================================

const uint32_t	cg_ulInterval_500_ms	= 500;		//	 0,5 sec
const uint32_t	cg_ulInterval_20_ms		= 20;


//==========================================================================
//
//		G L O B A L   V A R I A B L E S
//
//==========================================================================

uint32_t	g_ulMillisReadInputs	= 0;
uint32_t	g_ulMillisBlockDetect	= 0;


//==========================================================================
//
//		G L O B A L   F U N C T I O N S
//
//==========================================================================


//**********************************************************************
//	setup
//
//	hier werden die Klassen initialisiert.
//
void setup()
{
	// put your setup code here, to run once:
	//
#ifdef DEBUGGING_PRINTOUT
	g_clDebugging.Init();
#endif

	g_clControl.Init();

	//----	some setup tests  --------------------------------------
#ifdef DEBUGGING_PRINTOUT
	g_clDebugging.PrintTitle( VERSION_MAIN, VERSION_MINOR );
	g_clDebugging.PrintInfoLine( infoLineInit );
#endif

	delay( 200 );

#ifdef DEBUGGING_PRINTOUT
	g_clDebugging.PrintInfoLine( infoLineLedTest );
#endif

	g_clControl.Test( 300 );

	g_ulMillisReadInputs = millis() + cg_ulInterval_20_ms;

	//----	setup start state  -------------------------------------
	g_clControl.LedOn(  1 << LED_YELLOW );
	g_clControl.BlockEnable();
	
#ifdef DEBUGGING_PRINTOUT
	g_clDebugging.PrintTitle( VERSION_MAIN, VERSION_MINOR );
	g_clDebugging.PrintInfoLine( infoLineFields );

#if PLATINE_VERSION > 3
	g_clDebugging.PrintBlockOnOffState( blkOnOffIsOff );
#endif

#endif
}


//**********************************************************************
//	loop
//
//	Die Hauptschleife des Programms
//
void loop()
{
	bool	isBlockOnOffPressed	= false;
	bool	isKeyIn				= false;


	//----	Uebertragungsstoerung ?  -------------------------
	//
	if(	0 < g_ulMillisBlockDetect )
	{
		if( millis() > g_ulMillisBlockDetect )
		{
			g_ulMillisBlockDetect = 0;
	
			if( g_clControl.IsLedOn( 1 << LED_RED ) == g_clControl.IsBlockDetect() )
			{
				if( g_clControl.IsLedOn( 1 << LED_RED) )
				{
					g_clControl.LedOff( 1 << LED_UEBERTRAGRUNGSSTOERUNG );
				}
				else
				{
					g_clControl.LedOn( 1 << LED_UEBERTRAGRUNGSSTOERUNG );
				}
			}
		}
	}
	else if( g_clControl.IsLedOn( 1 << LED_RED ) == g_clControl.IsBlockDetect() )
	{
		g_ulMillisBlockDetect = millis() + cg_ulInterval_500_ms;
	}

	//----	Eingänge lesen  ----------------------------------
	//
	if( millis() > g_ulMillisReadInputs )
	{
		g_ulMillisReadInputs = millis() + cg_ulInterval_20_ms;

		g_clControl.ReadInputs();


#if PLATINE_VERSION > 3

		//----	Block On Off  --------------------------------
		//
		if( g_clControl.IsReset() )
		{
#ifdef DEBUGGING_PRINTOUT
			g_clDebugging.PrintBlockOnOffState( blkOnOffReset );
#endif

			g_clControl.LedOff( 1 << LED_YELLOW );
			g_clControl.LedOff( 1 << LED_GREEN  );
		}
		else if( (isBlockOnOffPressed = g_clControl.IsBlockOnOff()) )
		{
			if( g_clControl.IsLedOn( 1 << LED_YELLOW ) )
			{
#ifdef DEBUGGING_PRINTOUT
				g_clDebugging.PrintBlockOnOffState( blkOnOffIsOn );
#endif

				g_clControl.LedOff( 1 << LED_YELLOW );
				g_clControl.LedOn(  1 << LED_GREEN  );
			}
			else
			{
#ifdef DEBUGGING_PRINTOUT
				g_clDebugging.PrintBlockOnOffState( blkOnOffIsOff );
#endif

				g_clControl.LedOn(  1 << LED_YELLOW );
				g_clControl.LedOff( 1 << LED_GREEN  );
			}
		}


		//----	Key In  --------------------------------------
		//
		isKeyIn = g_clControl.IsKeyIn();

		if( g_clControl.IsKeyLedOn() != isKeyIn )
		{
#ifdef DEBUGGING_PRINTOUT
			g_clDebugging.PrintKeyInState( isKeyIn );
#endif

			if( g_clControl.IsKeyLedOn() )
			{
				g_clControl.KeyLedOff();
				g_clControl.KeyRelaisOff();
			}
			else
			{
				g_clControl.KeyLedOn();
				g_clControl.KeyRelaisOn();
			}
		}


		//----	Contact  -------------------------------------
		//
#ifdef DEBUGGING_PRINTOUT
		g_clDebugging.PrintContactState( g_clControl.IsContact() );
#endif


#endif	//	PLATINE_VERSION > 3

	}
}
