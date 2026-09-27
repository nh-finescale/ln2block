
#pragma once

//##########################################################################
//#
//#		IO_ControlClass
//#
//#	Diese Klasse verwaltet die Ports des Atmel Chips.
//#	Es werden Funktionen zur Verfügung gestellt, um
//#		-	diverse Ausgänge zu schalten
//#		-	diverse Eingänge auszuwerten
//#
//#-------------------------------------------------------------------------
//#
//#	File Version:	5		from: 30.05.2026
//#
//#	Implementation:
//#		-	add second track contact
//#			new function
//#				IsContactAusfahrt()
//#
//#-------------------------------------------------------------------------
//#
//#	File Version:	4		from: 20.10.2025
//#
//#	Implementation:
//#		-	add new platine version 7 with ATmega32U4 chip
//#			changed:
//#				LED definitions depending on platine version
//#
//#-------------------------------------------------------------------------
//#
//#	File Version:	3		from: 20.06.2023
//#
//#	Implementation:
//#		-	add functionality for the old style station interface
//#
//#-------------------------------------------------------------------------
//#
//#	File Version:	2		from: 07.01.2022
//#
//#	Implementation:
//#		-	Anpassung an Platine Version 6
//#			Einbindung der DIP-Switches für die Konfiguration
//#		-	Neue Funktionen zur Abfrage der DIP-Switches
//#
//#-------------------------------------------------------------------------
//#
//#	File Version:	1		from: 01.08.2021
//#
//#	Implementation:
//#		-	Initialisierung
//#		-	Test
//#		-	Ein- und Ausschalten
//#
//##########################################################################


//==========================================================================
//
//		I N C L U D E S
//
//==========================================================================

#include "compile_options.h"

#include <stdint.h>


//==========================================================================
//
//		D E F I N I T I O N S
//
//==========================================================================

#if PLATINE_VERSION == 7

	//----	LEDs  ------------------------------------------------------
	#define LED_BLUE		0x01
	#define LED_YELLOW		0x02
	#define LED_RED			0x10
	#define LED_GREEN		0x20
	#define LED_WHITE		0x40

	#define ALL_LEDS		(LED_BLUE | LED_YELLOW | LED_RED | LED_GREEN | LED_WHITE)

#else

	//----	LEDs  ------------------------------------------------------
	#define LED_YELLOW		0x02
	#define LED_RED			0x04
	#define LED_GREEN		0x08

	#define ALL_LEDS		(LED_YELLOW | LED_RED | LED_GREEN)

#endif


//----	Pseudonym (Alias)  ---------------------------------------------
#define LED_PROG_MODE				LED_YELLOW
#define LED_UEBERTRAGRUNGSSTOERUNG	LED_RED
#define LED_ANRUECKMELDER			LED_GREEN

//----	station interface  ---------------------------------------------
//
#define SI_SIGNAL_TRACK_INFO		0x0F


///////////////////////////////////////////////////////////////////////
//	CLASS:	LedControlClass
//
class IO_ControlClass
{
	public:
		IO_ControlClass();

		void Init( void );
		void Test( uint16_t delayTime );
		void ReadInputs( void );

		void LedOn( uint8_t leds );
		void LedOff( uint8_t leds );

		bool IsLedOn( uint8_t led );

		void BlockEnable( void );
		void BlockDisable( void );

		bool IsBlockDetect( void );

		void KeyLedOn( void );
		void KeyLedOff( void );

		bool IsKeyLedOn( void );

		void KeyRelaisOn( void );
		void KeyRelaisOff( void );

		bool IsReset( void );
		bool IsBlockOnOff( void );
		bool IsKeyIn( void );
		bool IsContact( void );


#if PLATINE_VERSION == 7

		bool IsContactAusfahrt( void );

#endif


		bool IsConfigKey( void );
		bool IsConfigKeyByBox( void );
		bool IsConfigRichtungsbetrieb( void );


		inline bool IsStationInterfaceConnectd( void )
		{
			return( m_bIsStationInterface );
		};

		uint8_t GetSignalTrackInfo( void );
		bool	IsErlaubnisAbgabeEnabled( void );
		bool	IsStationConnected( void );

		void SetAusfahrtMoeglich( bool bEnable );
		void SetNotZwangshalt( bool bEnable );


	private:
		bool	m_bIsStationInterface;
		bool	m_bOutputChanged;
		uint8_t	m_usInputs;
		uint8_t m_usOutputs;

		void ReadStationInterfaceInputs();
		void SetStationInterfaceOutputs();
};


//==========================================================================
//
//		E X T E R N   G L O B A L   V A R I A B L E S
//
//==========================================================================

extern IO_ControlClass	g_clControl;
