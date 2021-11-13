//##########################################################################
//#
//#		IO_ControlClass
//#
//#	Diese Klasse verwaltet alle Ein- und Ausgänge, z.B.: die LEDs.
//#	Es werden Funktionen zum Schreiben und Lesen der Ein- und Ausgänge
//#	zur Verfügung gestellt.
//#
//#-------------------------------------------------------------------------
//#	Version: 1.0	vom: 12.08.2021
//#
//#	Umsetzung:
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

#include <Arduino.h>

#include "io_control.h"
#include "entprellung.h"


//==========================================================================
//
//		D E F I N I T I O N S
//
//==========================================================================

#if PLATINE_VERSION == 5

	//----	PORT B  ----------------------------------------------------
	//		PB0		Loconet Receive (LN_RX)
	//		PB1		I2C Interrupt 1
	//		PB2		I2C Interrupt 2
	//		PB3		KEY_LED
	//		PB4		not used
	//		PB5		not used
	//		PB6		Oscillator 1
	//		PB7		Oscillator 2
	//
	#define KEY_LED				PB3

	#define PORT_B_FREE_BITS	((1 << PB2) | (1 << PB3) | (1 << PB4) | (1 << PB5))
	#define PORT_B_OUTPUTS		 (1 << KEY_LED)
	
	//----	PORT C  ----------------------------------------------------
	//		PC0		BLOCK_ENABLE (BL_DRV_EN / LED_BLUE)
	//		PC1		LED_YELLOW
	//		PC2		LED_RED
	//		PC3		LED_GREEN
	//		PC4		I2C:SDA
	//		PC5		I2C:SCL
	//		PC6		not implemented
	//		PC7		not implemented

	//----	PORT D  ----------------------------------------------------
	//		PD0		Block Rx (BL_RX)
	//		PD1		Block Tx (BL_TX)
	//		PD2		BLK_ON_OFF
	//		PD3		KEY_REL
	//		PD4		KEY_IN
	//		PD5		CONTACT
	//		PD6		BLOCK_DETECT
	//		PD7		Loconet Send (LN_TX)
	//
	#define	BLOCK_ON_OFF	PD2
	#define	KEY_REL			PD3
	#define	KEY_IN			PD4
	#define CONTACT			PD5
	#define	BLOCK_DETECT	PD6

	#define	PORT_D_INPUTS	((1 << BLOCK_ON_OFF) | (1 << KEY_IN) | (1 << CONTACT))
	#define	PORT_D_OUTPUTS	(1 << KEY_REL)


#elif PLATINE_VERSION == 4

	//----	PORT B  ----------------------------------------------------
	//		PB0		Loconet Receive (LN_RX, !!  von Hand neu verdrahtet  !! )
	//		PB1		KEY_IN
	//		PB2		CONTACT
	//		PB3		not used
	//		PB4		not used
	//		PB5		not used
	//		PB6		Oscillator 1
	//		PB7		Oscillator 2
	//
	#define	KEY_IN				PB1
	#define	CONTACT				PB2

	#define PORT_B_FREE_BITS	((1 << PB3) | (1 << PB4) | (1 << PB5))
	#define PORT_B_INPUTS		((1 << KEY_IN) | (1 << CONTACT))

	//----	PORT C  ----------------------------------------------------
	//		PC0		BLOCK_ENABLE (BL_DRV_EN / LED_BLUE)
	//		PC1		LED_YELLOW
	//		PC2		LED_RED
	//		PC3		LED_GREEN
	//		PC4		I2C:SDA
	//		PC5		I2C:SCL
	//		PC6		not implemented
	//		PC7		not implemented
	//

	//----	PORT D  ----------------------------------------------------
	//		PD0		Block Rx (BL_RX)
	//		PD1		Block Tx (BL_TX)
	//		PD2		BLK_ON_OFF
	//		PD3		KEY_LED
	//		PD4		Loconet Send (LN_TX)
	//		PD5		BLOCK_DETECT
	//		PD6		not used
	//		PD7		KEY_REL ( !! von Hand neu verdrahtet !! )
	//
	#define BLOCK_ON_OFF		PD2
	#define KEY_LED				PD3
	#define	BLOCK_DETECT		PD5
	#define	KEY_REL				PD7

	#define PORT_D_FREE_BITS	 (1 << PD6)
	#define PORT_D_INPUTS		 (1 << BLOCK_ON_OFF)
	#define	PORT_D_OUTPUTS		((1 << KEY_LED) | (1 << KEY_REL))

#else	//	Version 3 und vorherige Versionen

	//----	PORT B  ----------------------------------------------------
	//		PB0		Loconet Receive (LN_RX)
	//		PB2		Block Send mirror (BL_TX)
	//		PB6		Oscillator 1
	//		PB7		Oscillator 2
	//
	#define PORT_B_FREE_BITS	((1 << PB1) | (1 << PB3) | (1 << PB4) | (1 << PB5))
	#define	BLOCK_TX_MIRROR		PB2

	//----	PORT C  ----------------------------------------------------
	//		PC0		BLOCK_ENABLE (BL_DRV_EN / LED_BLUE)
	//		PC1		LED_YELLOW
	//		PC2		LED_RED
	//		PC3		LED_GREEN
	//		PC4		I2C:SDA
	//		PC5		I2C:SCL
	//

	//----	PORT D  ----------------------------------------------------
	//		PD0		Block Rx (BL_RX)
	//		PD1		Block Tx (BL_TX)
	//		PD5		BLOCK_DETECT
	//		PD7		Loconet Send (LN_TX)
	//
	#define PORT_D_FREE_BITS	((1 << PB2) | (1 << PB3) | (1 << PB4) | (1 << PD6))
	#define	BLOCK_DETECT		PD5


#endif


//---------------------------------------------------------------------
//	Die folgenden Definitionen sind für alle Platinen Versionen
//	identisch
//

//----	Port B  ---------------------------------------------------


//----	Port C  ---------------------------------------------------
//	Die Definitionen für die LEDs befinden sich im Header
//
#define BLOCK_ENABLE	PC0

#define ALL_LEDS	((1 << LED_YELLOW) | (1 << LED_RED) | (1 << LED_GREEN))


//----	Port D  ---------------------------------------------------


//==========================================================================
//
//		G L O B A L   V A R I A B L E S
//
//==========================================================================

IO_ControlClass	g_clControl	= IO_ControlClass();

#if PLATINE_VERSION == 5

//----------------------------------------------------------------------
//	Bei Platinen Version 5 sind alle Eingänge an Port D angeschlossen,
//	daher gibt es nur eine Port Variable.
//
EntprellungClass	g_clPortD( (1 << BLOCK_ON_OFF) );

#elif PLATINE_VERSION == 4

//----------------------------------------------------------------------
//	Bei Platine Version 4 sind die Eingänge auf Port B und Port D
//	verteilt, daher zwei Port Variable.
//
EntprellungClass	g_clPortB( 0x00 );
EntprellungClass	g_clPortD( (1 << BLOCK_ON_OFF) );

#else

//----------------------------------------------------------------------
//	Bei älteren Platinen Versionen ist nur ein Eingang interessant,
//	daher auch nur eine Port Variabla.
//
EntprellungClass	g_clPortD( 0x00 );

#endif


//==========================================================================
//
//		C L A S S   F U N C T I O N S
//
//==========================================================================


///////////////////////////////////////////////////////////////////////
//	CLASS: IO_ControlClass
//

//-----------------------------------------------------------------
//	Constructor
//
IO_ControlClass::IO_ControlClass()
{
}


//-----------------------------------------------------------------
//	Init
//
//	Hier werden die I/O Pins der einzelnen Ports konfiguriert.
//	Nicht benutzte I/O Pins werden auf Eingang mit PullUp gesetzt.
//	Alle Componenten (z.B.: LEDs) an Ausgängen werden "OFF"
//	geschaltet.
//
void IO_ControlClass::Init( void )
{
	//----	set unused pins to input with pullup  ------------------
	//
	DDRB	&= ~PORT_B_FREE_BITS;
	PORTB	|=  PORT_B_FREE_BITS;

#ifdef PORT_D_FREE_BITS
	DDRD	&= ~PORT_D_FREE_BITS;
	PORTD	|=  PORT_D_FREE_BITS;
#endif


	//----	Port B  ------------------------------------------------
	//
#ifdef PORT_B_INPUTS
	DDRB	&= ~PORT_B_INPUTS;		//	auf Eingang setzen
	PORTB	|=  PORT_B_INPUTS;		//	Pull-Up Widerstand ein
#endif

#ifdef PORT_B_OUTPUTS
	DDRB	|= PORT_B_OUTPUTS;		//	auf Ausgang setzen
	PORTB	|= PORT_B_OUTPUTS;		//	auf 'Aus' schalten
#endif

	//----	Port C  ------------------------------------------------
	//
	//----	Pins auf Ausgang setzen  -------------------------------
	DDRC |= ((1 << BLOCK_ENABLE) | ALL_LEDS);
	
	LedOff( ALL_LEDS );
	BlockDisable();

	//----	Port D  ------------------------------------------------
	//
	DDRD	&= ~(1 << BLOCK_DETECT);	//	auf Eingang setzen
	
#ifdef PORT_D_INPUTS
	DDRD	&= ~PORT_D_INPUTS;			//	auf Eingang setzen
	PORTD	|=  PORT_D_INPUTS;			//	Pull-Up Widerstand ein
#endif

#ifdef PORT_D_OUTPUTS
	DDRD	|= PORT_D_OUTPUTS;			//	auf Ausgang setzen
	PORTD	|= PORT_D_OUTPUTS;			//	auf 'Aus' schalten
#endif
}


//----------------------------------------------------------------------
//	LedTest
//
void IO_ControlClass::Test( uint16_t delayTime )
{
	//----	Blau EIN, alle anderen aus  ----------------------------
	BlockEnable();
	delay( delayTime );

	//----	Blau AUS, Gelb EIN  ------------------------------------
	BlockDisable();
	LedOn( 1 << LED_YELLOW );
	delay( delayTime );
	
	//----	Gelb AUS, Rot EIN  -------------------------------------
	LedOff( 1 << LED_YELLOW );
	LedOn( 1 << LED_RED );
	delay( delayTime );
	
	//----	Rot AUS, Grün EIN  -------------------------------------
	LedOff( 1 << LED_RED );
	LedOn( 1 << LED_GREEN );
	delay( delayTime );
	
	//----	Grün AUS, Test beendet  --------------------------------
	LedOff( 1 << LED_GREEN );
}


//----------------------------------------------------------------------
//	ReadInputs
//
void IO_ControlClass::ReadInputs( void )
{
	g_clPortD.Work( PIND );
	
#if PLATINE_VERSION == 4
	g_clPortB.Work( PINB );
#endif
}


//----------------------------------------------------------------------
//	LedOn
//
void IO_ControlClass::LedOn( uint8_t leds )
{
	PORTC &= ~leds; 
}


//----------------------------------------------------------------------
//	LedOff
//
void IO_ControlClass::LedOff( uint8_t leds )
{
	PORTC |= leds;
}


//----------------------------------------------------------------------
//	BlockEnable
//
void IO_ControlClass::BlockEnable( void )
{
	PORTC |= (1 << BLOCK_ENABLE);
}


//----------------------------------------------------------------------
//	BlockDisable
//
void IO_ControlClass::BlockDisable( void )
{
	PORTC &= ~(1 << BLOCK_ENABLE);
}


//----------------------------------------------------------------------
//	IsLedOn
//
bool IO_ControlClass::IsLedOn( uint8_t leds )
{
	return( 0 == (PINC & leds) );
}


//----------------------------------------------------------------------
//	IsBlockDetect
//
bool IO_ControlClass::IsBlockDetect( void )
{
	return( 0 != g_clPortD.GetKeyState( 1 << BLOCK_DETECT ) );
}


#if PLATINE_VERSION > 3

//----------------------------------------------------------------------
//	KeyLedOn
//
void IO_ControlClass::KeyLedOn( void )
{
#if PLATINE_VERSION == 5
	PORTB &= ~(1 << KEY_LED);
#else
	PORTD &= ~(1 << KEY_LED);
#endif 
}


//----------------------------------------------------------------------
//	KeyLedOff
//
void IO_ControlClass::KeyLedOff( void )
{
#if PLATINE_VERSION == 5
	PORTB |= (1 << KEY_LED);
#else
	PORTD |= (1 << KEY_LED);
#endif 
}


//----------------------------------------------------------------------
//	IsKeyLedOn
//
bool IO_ControlClass::IsKeyLedOn( void )
{
#if PLATINE_VERSION == 5
	return( 0 == (PINB & (1 << KEY_LED)) );
#else
	return( 0 == (PIND & (1 << KEY_LED)) );
#endif 
}


//----------------------------------------------------------------------
//	KeyRelaisOn
//
void IO_ControlClass::KeyRelaisOn( void )
{
	PORTD &= ~(1 << KEY_REL);
}


//----------------------------------------------------------------------
//	KeyRelaisOff
//
void IO_ControlClass::KeyRelaisOff( void )
{
	PORTD |= (1 << KEY_REL);
}


//----------------------------------------------------------------------
//	IsReset
//
bool IO_ControlClass::IsReset( void )
{
	return( 0 != g_clPortD.GetKeyLong( 1 << BLOCK_ON_OFF ) );
}


//----------------------------------------------------------------------
//	IsBlockOnOff
//
bool IO_ControlClass::IsBlockOnOff( void )
{
	return( 0 != g_clPortD.GetKeyShort( 1 << BLOCK_ON_OFF ) );
}


//----------------------------------------------------------------------
//	IsBlockOnOff
//
bool IO_ControlClass::IsKeyIn( void )
{
#if PLATINE_VERSION == 5
	return( 0 != g_clPortD.GetKeyState( 1 << KEY_IN ) );
#else
	return( 0 != g_clPortB.GetKeyState( 1 << KEY_IN ) );
#endif
}


//----------------------------------------------------------------------
//	IsBlockOnOff
//
bool IO_ControlClass::IsContact( void )
{
#if PLATINE_VERSION == 5
	return( 0 != g_clPortD.GetKeyState( 1 << CONTACT ) );
#else
	return( 0 != g_clPortB.GetKeyState( 1 << CONTACT ) );
#endif
}

#endif
