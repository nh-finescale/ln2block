//##########################################################################
//#
//#		IO_ControlClass
//#
//#-------------------------------------------------------------------------
//#	This class operates the Inputs and Outputs, like LEDs, relais etc.
//#	There will be functions to set Outputs and read Inputs.
//#
//#-------------------------------------------------------------------------
//#
//#	File Version:	4		from: 20.06.2023
//#	
//#	Bug Fix:
//#		-	wrong basis address for the port extender used for the
//#			old style station interface
//#
//#-------------------------------------------------------------------------
//#
//#	File Version:	4		from: 20.06.2023
//#	
//#	Implementation:
//#		-	add functionality for the old style station interface
//#			changes in functions
//#				Init()
//#				ReadInputs()
//#			new functions
//#				IsStationInterfaceConnectd()
//#				IsErlaubnisAbgabeEnabled()
//#				IsStationConnected()
//#				GetSignalTrackInfo()
//#				SetAusfahrtMoeglich()
//#				SetNotZwangshalt()
//#				ReadStationInterfaceInputs()
//#				SetStationInterfaceOutputs()
//#
//#-------------------------------------------------------------------------
//#
//#	File Version:	3		from: 07.01.2022
//#	
//#	Implementation:
//#		-	Anpassung an Platine Version 6
//#			Neue Inputs zur Konfiguration über DIP-Switches
//#		-	Neue Funktionen zur Abfrage der DIP-Switches
//#
//#-------------------------------------------------------------------------
//#
//#	File Version:	2		from: 14.09.2021
//#
//#	Bug Fix:
//#		-	KEY_LED an das endgültige Layout der Platine angepasst.
//#
//#-------------------------------------------------------------------------
//#
//#	File Version:	1		from: 12.08.2021
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

#include <Arduino.h>
#include <Wire.h>

#include "io_control.h"
#include "entprellung.h"


//==========================================================================
//
//		D E F I N I T I O N S
//
//==========================================================================

#define	READ_INPUT_COUNT	6


//----	station interface  ---------------------------------------------
//
#define STATION_INTERFACE_ADR		0x38

#define SI_ERLAUBNISABGABE_ENABLE	0x40
#define SI_PRUEFSCHLEIFE_CLOSED		0x80

#define SI_AUSFAHRT_MOEGLICH		0x10
#define SI_KEIN_ZWANGSHALT			0x20

#define SI_INPUTS	(SI_SIGNAL_TRACK_INFO | SI_ERLAUBNISABGABE_ENABLE | SI_PRUEFSCHLEIFE_CLOSED)



#if PLATINE_VERSION == 7


	//----	Port B  ----------------------------------------------------
	//		PB0		KEY_REL
	//		PB1		unused
	//		PB2		unused
	//		PB3		BLOCK_ON_OFF
	//		PB4		CONTACT_1
	//		PB5		Sound TX
	//		PB6		Sound RX
	//		PB7		BLOCK_DETECT
	#define KEY_REL				(1 << PB0)
	#define BLOCK_ON_OFF		(1 << PB3)
	#define CONTACT_1			(1 << PB4)
	#define SOUND_TX			(1 << PB5)
	#define SOUND_RX			(1 << PB6)
	#define BLOCK_DETECT		(1 << PB7)

//	#define PORT_B_FREE_BITS	((1 << PB1) | (1 << PB2))
	#define PORT_B_OUTPUTS		 KEY_REL
	#define PORT_B_INPUTS		(BLOCK_ON_OFF | CONTACT_1 | BLOCK_DETECT)


	//----	Port C  ----------------------------------------------------
	//		PC0		not implemented
	//		PC1		not implemented
	//		PC2		not implemented
	//		PC3		not implemented
	//		PC4		not implemented
	//		PC5		not implemented
	//		PC6		DIP-Switches DIP-4	CONFIG_4
	//		PC7		DIP-Switches DIP-3	CONFIG_3
	#define CONFIG_4			(1 << PC6)
	#define CONFIG_3			(1 << PC7)

	#define PORT_C_INPUTS		(CONFIG_3 | CONFIG_4)


	//----	Port D  ----------------------------------------------------
	//		PD0		I2C:SCL
	//		PD1		I2C:SDA
	//		PD2		BLOCK_RX
	//		PD3		BLOCK_TX
	//		PD4		Loconet Receive (LN_RX)
	//		PD5		KEY_IN
	//		PD6		DIP-Switches DIP-2	CONFIG_2
	//		PD7		DIP-Switches DIP-1	CONFIG_1
	#define I2C_SCL				(1 << PD0)
	#define I2C_SDA				(1 << PD1)
	#define BLOCK_RX			(1 << PD2)
	#define BLOCK_TX			(1 << PD3)
	#define LN_RX				(1 << PD4)
	#define KEY_IN				(1 << PD5)
	#define CONFIG_2			(1 << PD6)
	#define CONFIG_1			(1 << PD7)

	#define PORT_D_INPUTS		(KEY_IN | CONFIG_1 | CONFIG_2)


	//----	Port E  ----------------------------------------------------
	//		PE0		not implemented
	//		PE1		not implemented
	//		PE2		CONTACT_2
	//		PE3		not implemented
	//		PE4		not implemented
	//		PE5		not implemented
	//		PE6		LN_TX
	//		PE7		not implemented
	#define CONTACT_2			(1 << PE2)
	#define LN_TX				(1 << PE6)

	#define PORT_E_INPUTS		CONTACT_2


	//----	Port F  ----------------------------------------------------
	//		PF0		LED_BLUE
	//		PF1		LED_YELLOW
	//		PF2		not implemented
	//		PF3		not implemented
	//		PF4		LED_RED
	//		PF5		LED_GREEN
	//		PF6		LED_WHITE
	//		PF7		unused

//	#define PORT_F_FREE_BITS	(1 << PF7)
	#define PORT_F_OUTPUTS		ALL_LEDS


#elif PLATINE_VERSION == 6


	//----	PORT B  ----------------------------------------------------
	//		PB0		Loconet Receive (LN_RX)
	//		PB1		CONFIG_1
	//		PB2		CONFIG_2
	//		PB3		KEY_LED
	//		PB4		CONFIG_3
	//		PB5		not used
	//		PB6		Oscillator 1
	//		PB7		Oscillator 2
	//
	#define	CONFIG_1			(1 << PB1)
	#define	CONFIG_2			(1 << PB2)
	#define KEY_LED				(1 << PB3)
	#define	CONFIG_3			(1 << PB4)

	#define PORT_B_FREE_BITS	(1 << PB5)
	#define PORT_B_OUTPUTS		KEY_LED
	#define PORT_B_INPUTS		(CONFIG_1 | CONFIG_2 | CONFIG_3)
	#define PORT_B_PULLUP		PORT_B_INPUTS
	
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
	#define	BLOCK_ON_OFF	(1 << PD2)
	#define	KEY_REL			(1 << PD3)
	#define	KEY_IN			(1 << PD4)
	#define CONTACT			(1 << PD5)
	#define	BLOCK_DETECT	(1 << PD6)

	#define	PORT_D_OUTPUTS	KEY_REL
	#define	PORT_D_INPUTS	(BLOCK_ON_OFF | KEY_IN | CONTACT)
	#define PORT_D_PULLUP	PORT_D_INPUTS



#elif PLATINE_VERSION == 5


	//----	PORT B  ----------------------------------------------------
	//		PB0		Loconet Receive (LN_RX)
	//		PB1		I2C_INTA
	//		PB2		I2C_INTB
	//		PB3		KEY_LED
	//		PB4		not used
	//		PB5		not used
	//		PB6		Oscillator 1
	//		PB7		Oscillator 2
	//
	#define KEY_LED				(1 << PB3)

	#define PORT_B_FREE_BITS	((1 << PB1) | (1 << PB2) | (1 << PB4) | (1 << PB5))
	#define PORT_B_OUTPUTS		KEY_LED
	
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
	#define	BLOCK_ON_OFF		(1 << PD2)
	#define	KEY_REL				(1 << PD3)
	#define	KEY_IN				(1 << PD4)
	#define CONTACT				(1 << PD5)
	#define	BLOCK_DETECT		(1 << PD6)

	#define	PORT_D_OUTPUTS		KEY_REL
	#define	PORT_D_INPUTS		(BLOCK_ON_OFF | KEY_IN | CONTACT)
	#define PORT_D_PULLUP		PORT_D_INPUTS



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
	#define	KEY_IN				(1 << PB1)
	#define	CONTACT				(1 << PB2)

	#define PORT_B_FREE_BITS	((1 << PB3) | (1 << PB4) | (1 << PB5))
	#define PORT_B_INPUTS		(KEY_IN | CONTACT)
	#define PORT_B_PULLUP		PORT_B_INPUTS

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
	#define BLOCK_ON_OFF		(1 << PD2)
	#define KEY_LED				(1 << PD3)
	#define	BLOCK_DETECT		(1 << PD5)
	#define	KEY_REL				(1 << PD7)

	#define PORT_D_FREE_BITS	(1 << PD6)
	#define	PORT_D_OUTPUTS		(KEY_LED | KEY_REL)
	#define PORT_D_INPUTS		BLOCK_ON_OFF
	#define PORT_D_PULLUP		PORT_D_INPUTS



#else	//	Version 3 und vorherige Versionen


	//----	PORT B  ----------------------------------------------------
	//		PB0		Loconet Receive (LN_RX)
	//		PB2		Block Send mirror (BL_TX)
	//		PB6		Oscillator 1
	//		PB7		Oscillator 2
	//
	#define PORT_B_FREE_BITS	((1 << PB1) | (1 << PB3) | (1 << PB4) | (1 << PB5))
	#define	BLOCK_TX_MIRROR		(1 << PB2)

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
	#define	BLOCK_DETECT		(1 << PD5)


#endif


//---------------------------------------------------------------------
//	The following definitions are identical to all board versions,
//	except of board version 7
//
#if PLATINE_VERSION == 7


#define ALL_LEDS	(LED_BLUE | LED_YELLOW | LED_RED | LED_GREEN | LED_WHITE)


#else

//----	Port B  ---------------------------------------------------


//----	Port C  ---------------------------------------------------
//	LED definitions are located in the header file 'io_control.h'
//
#define BLOCK_ENABLE		(1 << PC0)

#define ALL_LEDS			(LED_YELLOW | LED_RED | LED_GREEN)

#define PORT_C_OUTPUTS		(BLOCK_ENABLE | ALL_LEDS)


//----	Port D  ---------------------------------------------------


#endif



//==========================================================================
//
//		G L O B A L   V A R I A B L E S
//
//==========================================================================

IO_ControlClass	g_clControl	= IO_ControlClass();



#if PLATINE_VERSION == 7

//----------------------------------------------------------------------
//	for board version 7 Inputs are connected to Port B, Port C, Port D
//	and Port E
//
EntprellungClass	g_clPortB( BLOCK_ON_OFF, BLOCK_DETECT );
EntprellungClass	g_clPortC( 0x00, CONFIG_3 | CONFIG_4 );
EntprellungClass	g_clPortD( 0x00, CONFIG_1 | CONFIG_2 );
EntprellungClass	g_clPortE( 0x00 );



#elif PLATINE_VERSION == 6

//----------------------------------------------------------------------
//	for board version 6 Inputs are connected to Port B and Port D
//
EntprellungClass	g_clPortB( 0x00, 0xFF );
EntprellungClass	g_clPortD( BLOCK_ON_OFF, 0xFF );



#elif PLATINE_VERSION == 5

//----------------------------------------------------------------------
//	for board version 5 all Inputs are connected to Port D
//
EntprellungClass	g_clPortD( BLOCK_ON_OFF, 0xFF );



#elif PLATINE_VERSION == 4

//----------------------------------------------------------------------
//	for board version 4 Inputs are connected to Port B and Port D
//
EntprellungClass	g_clPortB( 0x00, 0xFF );
EntprellungClass	g_clPortD( BLOCK_ON_OFF, 0xFF );



#else	//	Version 3 und vorherige Versionen

//----------------------------------------------------------------------
//	for older board versions there is only one Input relevant and
//	this Input is connected to Port D
//
EntprellungClass	g_clPortD( 0x00, 0xFF );

#endif


//==========================================================================
//
//		C L A S S   F U N C T I O N S
//
//==========================================================================


////////////////////////////////////////////////////////////////////////
//	CLASS: IO_ControlClass
//

//******************************************************************
//	Constructor
//
IO_ControlClass::IO_ControlClass()
{
	m_bIsStationInterface	= false;
	m_bOutputChanged		= false;
	m_usInputs				= 0;
	m_usOutputs				= SI_INPUTS;
}


//******************************************************************
//	Init
//------------------------------------------------------------------
//	here for all Ports the relevant I/O pins will be configured.
//	Not used I/O's will be set to Input with PullUp.
//	all connected devices (like LED's) will set to 'off'.
//
void IO_ControlClass::Init( void )
{
	uint8_t	usCheck;


	//----	set unused pins to input with pullup  ------------------
	//
#ifdef PORT_B_FREE_BITS
	DDRB	&= ~PORT_B_FREE_BITS;
	PORTB	|=  PORT_B_FREE_BITS;
#endif

#ifdef PORT_C_FREE_BITS
	DDRC	&= ~PORT_C_FREE_BITS;
	PORTC	|=  PORT_C_FREE_BITS;
#endif

#ifdef PORT_D_FREE_BITS
	DDRD	&= ~PORT_D_FREE_BITS;
	PORTD	|=  PORT_D_FREE_BITS;
#endif

#ifdef PORT_E_FREE_BITS
	DDRE	&= ~PORT_E_FREE_BITS;
	PORTE	|=  PORT_E_FREE_BITS;
#endif

#ifdef PORT_F_FREE_BITS
	DDRF	&= ~PORT_F_FREE_BITS;
	PORTF	|=  PORT_F_FREE_BITS;
#endif


	//----	Port B  ------------------------------------------------
	//
#ifdef PORT_B_INPUTS
	DDRB		&= ~PORT_B_INPUTS;		//	configure as Input

	#ifdef PORT_B_PULLUP
		PORTB	|=  PORT_B_PULLUP;		//	Pull-Up on
	#endif
#endif

#ifdef PORT_B_OUTPUTS
	DDRB	|= PORT_B_OUTPUTS;			//	configure as Output

	#if PLATINE_VERSION == 7
		PORTB	&= ~PORT_B_OUTPUTS;
	#else
		PORTB	|= PORT_B_OUTPUTS;		//	switch off
	#endif
#endif


	//----	Port C  ------------------------------------------------
	//
#ifdef PORT_C_INPUTS
	DDRC	&= ~PORT_C_INPUTS;		//	configure as Input

	#ifdef PORT_C_PULLUP
		PORTC	|=  PORT_C_PULLUP;		//	Pull-Up on
	#endif
#endif

#ifdef PORT_C_OUTPUTS
	DDRC	|= PORT_C_OUTPUTS;			//	configure as Output

	#if PLATINE_VERSION == 7
		PORTC	&= ~PORT_C_OUTPUTS;
	#else
		PORTC	|= PORT_C_OUTPUTS;		//	switch off
	#endif
#endif


	//----	Port D  ------------------------------------------------
	//
#if PLATINE_VERSION != 7
	DDRD	&= ~BLOCK_DETECT;			//	configure as Input
#endif

#ifdef PORT_D_INPUTS
	DDRD		&= ~PORT_D_INPUTS;		//	configure as Input

	#ifdef PORT_D_PULLUP
		PORTD	|=  PORT_D_PULLUP;		//	Pull-Up on
	#endif
#endif

#ifdef PORT_D_OUTPUTS
	DDRD	|= PORT_D_OUTPUTS;			//	configure as Output

	#if PLATINE_VERSION == 7
		PORTD	&= ~PORT_D_OUTPUTS;
	#else
		PORTD	|= PORT_D_OUTPUTS;		//	switch off
	#endif
#endif


	//----	Port E  ------------------------------------------------
	//
#ifdef PORT_E_INPUTS
	DDRE		&= ~PORT_E_INPUTS;		//	configure as Input

	#ifdef PORT_E_PULLUP
		PORTE	|=  PORT_E_PULLUP;		//	Pull-Up on
	#endif
#endif

#ifdef PORT_E_OUTPUTS
	DDRE	|= PORT_E_OUTPUTS;			//	configure as Output

	#if PLATINE_VERSION == 7
		PORTE	&= ~PORT_E_OUTPUTS;
	#else
		PORTE	|= PORT_E_OUTPUTS;		//	switch off
	#endif
#endif


	//----	Port F  ------------------------------------------------
	//
#ifdef PORT_F_INPUTS
	DDRF		&= ~PORT_F_INPUTS;		//	configure as Input

	#ifdef PORT_F_PULLUP
		PORTF	|=  PORT_F_PULLUP;		//	Pull-Up on
	#endif
#endif

#ifdef PORT_F_OUTPUTS
	DDRF	|= PORT_F_OUTPUTS;			//	configure as Output

	#if PLATINE_VERSION == 7
		PORTF	&= ~PORT_F_OUTPUTS;
	#else
		PORTF	|= PORT_F_OUTPUTS;		//	switch off
	#endif
#endif


	LedOff( ALL_LEDS );
	BlockDisable();


	//------------------------------------------------------------------
	//	check if the station interface is connected
	//	first check if Wire was already initialized
	//	if not then do it
	//
	usCheck = TWCR;

	if( ~(usCheck | ~(_BV(TWEN) | _BV(TWIE) | _BV(TWEA))) )
	{
		Wire.begin();
	}

	//------------------------------------------------------------------
	//	Check if the station interface can be connected
	//
	Wire.beginTransmission( STATION_INTERFACE_ADR );

	if( 0 == Wire.endTransmission() )
	{
		delay( 20 );

		//----------------------------------------------------------
		//	YES the station interface can be connected
		//	so initialize the interface
		//
		m_bIsStationInterface = true;

		Wire.beginTransmission( STATION_INTERFACE_ADR );
		Wire.write( SI_INPUTS );
		Wire.endTransmission();

		delay( 20 );

		m_usOutputs |= (SI_AUSFAHRT_MOEGLICH | SI_KEIN_ZWANGSHALT);
	}


	//----	Read actual Inputs  ------------------------------------
	//
	for( uint8_t idx = 0 ; idx < READ_INPUT_COUNT ; idx++ )
	{
		delay( 20 );

		ReadInputs();
	}

	delay( 20 );
}


//******************************************************************
//	LedTest
//
void IO_ControlClass::Test( uint16_t delayTime )
{
#if PLATINE_VERSION == 7

	//----	Blau EIN, alle anderen aus  ------------------------
	LedOn( LED_BLUE );
	delay( delayTime );

	//----	Blau AUS, Gelb EIN  --------------------------------
	LedOff( LED_BLUE );

#else
	
	//----	Blau EIN, alle anderen aus  ------------------------
	BlockEnable();
	delay( delayTime );

	//----	Blau AUS, Gelb EIN  --------------------------------
	BlockDisable();

#endif

	LedOn( LED_YELLOW );
	delay( delayTime );
	
	//----	Gelb AUS, Rot EIN  ---------------------------------
	LedOff( LED_YELLOW );
	LedOn(  LED_RED );
	delay( delayTime );
	
	//----	Rot AUS, Grün EIN  ---------------------------------
	LedOff( LED_RED );
	LedOn(  LED_GREEN );
	delay( delayTime );
	
	//----	Grün AUS, Test beendet  ----------------------------
	LedOff( LED_GREEN );

#if PLATINE_VERSION == 7

	LedOn( LED_WHITE );
	delay( delayTime );
	
	LedOff( LED_WHITE );

#endif
}


//******************************************************************
//	ReadInputs
//------------------------------------------------------------------
//	This function should be called at regular intervals,
//	for example: every 20 ms
//
void IO_ControlClass::ReadInputs( void )
{
	if( m_bIsStationInterface )
	{
		ReadStationInterfaceInputs();
	}

	g_clPortD.Work( PIND );

#if PLATINE_VERSION == 7

	g_clPortB.Work( PINB );
	g_clPortC.Work( PINC );
	g_clPortE.Work( PINE );

#elif PLATINE_VERSION > 5

	g_clPortB.Work( PINB );

#elif PLATINE_VERSION == 4

	g_clPortB.Work( PINB );

#endif

	if( m_bIsStationInterface )
	{
		SetStationInterfaceOutputs();
	}
}


//******************************************************************
//	LedOn
//
void IO_ControlClass::LedOn( uint8_t leds )
{
#if PLATINE_VERSION == 7

	PORTF |= leds;

#else

	PORTC &= ~leds; 

#endif
}


//******************************************************************
//	LedOff
//
void IO_ControlClass::LedOff( uint8_t leds )
{
#if PLATINE_VERSION == 7

	PORTF &= ~leds; 

#else

	PORTC |= leds;

#endif
}


//******************************************************************
//	BlockEnable
//
void IO_ControlClass::BlockEnable( void )
{
#if PLATINE_VERSION != 7
	PORTC |= BLOCK_ENABLE;
#endif
}


//******************************************************************
//	BlockDisable
//
void IO_ControlClass::BlockDisable( void )
{
#if PLATINE_VERSION != 7
	PORTC &= ~BLOCK_ENABLE;
#endif
}


//******************************************************************
//	IsLedOn
//
bool IO_ControlClass::IsLedOn( uint8_t leds )
{
#if PLATINE_VERSION == 7

	return( 1 == (PINF & leds) );

#else

	return( 0 == (PINC & leds) );

#endif
}


//******************************************************************
//	IsBlockDetect
//
bool IO_ControlClass::IsBlockDetect( void )
{
#if PLATINE_VERSION == 7

	return( 0 != g_clPortB.GetKeyState( BLOCK_DETECT ) );

#else

	return( 0 != g_clPortD.GetKeyState( BLOCK_DETECT ) );

#endif
}


//******************************************************************
//	KeyLedOn
//
void IO_ControlClass::KeyLedOn( void )
{
#if PLATINE_VERSION == 7
	return;
#elif PLATINE_VERSION > 4
	PORTB &= ~KEY_LED;
#elif PLATINE_VERSION == 4
	PORTD &= ~KEY_LED;
#else
	return;
#endif 
}


//******************************************************************
//	KeyLedOff
//
void IO_ControlClass::KeyLedOff( void )
{
#if PLATINE_VERSION == 7
	return;
#elif PLATINE_VERSION > 4
	PORTB |= KEY_LED;
#elif PLATINE_VERSION == 4
	PORTD |= KEY_LED;
#else
	return;
#endif 
}


//******************************************************************
//	IsKeyLedOn
//
bool IO_ControlClass::IsKeyLedOn( void )
{
#if PLATINE_VERSION == 7
	return( false );
#elif PLATINE_VERSION > 4
	return( 0 == (PINB & KEY_LED) );
#elif PLATINE_VERSION == 4
	return( 0 == (PIND & KEY_LED) );
#else
	return( false );
#endif
}


//******************************************************************
//	KeyRelaisOn
//
void IO_ControlClass::KeyRelaisOn( void )
{
#if PLATINE_VERSION == 7
	PORTB &= ~KEY_REL;
#elif PLATINE_VERSION > 3
	PORTD &= ~KEY_REL;
#endif
}


//******************************************************************
//	KeyRelaisOff
//
void IO_ControlClass::KeyRelaisOff( void )
{
#if PLATINE_VERSION == 7
	PORTB |= KEY_REL;
#elif PLATINE_VERSION > 3
	PORTD |= KEY_REL;
#endif
}


//******************************************************************
//	IsReset
//
bool IO_ControlClass::IsReset( void )
{
#if PLATINE_VERSION == 7
	return( 0 != g_clPortB.GetKeyShort( BLOCK_ON_OFF ) );
#elif PLATINE_VERSION > 3
	return( 0 != g_clPortD.GetKeyLong( BLOCK_ON_OFF ) );
#else
	return( false );
#endif
}


//******************************************************************
//	IsBlockOnOff
//
bool IO_ControlClass::IsBlockOnOff( void )
{
#if PLATINE_VERSION == 7
	return( 0 != g_clPortB.GetKeyLong( BLOCK_ON_OFF ) );
#elif PLATINE_VERSION > 3
	return( 0 != g_clPortD.GetKeyShort( BLOCK_ON_OFF ) );
#else
	return( false );
#endif
}


//******************************************************************
//	IsKeyIn
//
bool IO_ControlClass::IsKeyIn( void )
{
#if PLATINE_VERSION > 4
	return( 0 != g_clPortD.GetKeyState( KEY_IN ) );
#elif PLATINE_VERSION == 4
	return( 0 != g_clPortB.GetKeyState( KEY_IN ) );
#else
	return( false );
#endif
}


//******************************************************************
//	IsContact
//
bool IO_ControlClass::IsContact( void )
{
#if PLATINE_VERSION == 7
	return( 0 != g_clPortB.GetKeyState( CONTACT_1 ) );
#elif PLATINE_VERSION > 4
	return( 0 != g_clPortD.GetKeyState( CONTACT ) );
#elif PLATINE_VERSION == 4
	return( 0 != g_clPortB.GetKeyState( CONTACT ) );
#else
	return( false );
#endif
}


//******************************************************************
//	IsConfigKey
//
bool IO_ControlClass::IsConfigKey( void )
{
#if PLATINE_VERSION == 7
	return( 0 != g_clPortD.GetKeyState( CONFIG_1 ) );
#elif PLATINE_VERSION == 6
	return( 0 != g_clPortB.GetKeyState( CONFIG_1 ) );
#else
	return( false );
#endif
}


//******************************************************************
//	IsConfigKeyByBox
//
bool IO_ControlClass::IsConfigKeyByBox( void )
{
#if PLATINE_VERSION == 7
	return( 0 != g_clPortD.GetKeyState( CONFIG_2 ) );
#elif PLATINE_VERSION > 5
	return( 0 != g_clPortB.GetKeyState( CONFIG_2 ) );
#else
	return( false );
#endif
}


//******************************************************************
//	IsConfigRichtungsbetrieb
//
bool IO_ControlClass::IsConfigRichtungsbetrieb( void )
{
#if PLATINE_VERSION == 7
	return( 0 != g_clPortC.GetKeyState( CONFIG_3 ) );
#elif PLATINE_VERSION > 5
	return( 0 != g_clPortB.GetKeyState( CONFIG_3 ) );
#else
	return( false );
#endif
}


//******************************************************************
//	GetSignalTrackInfo
//
uint8_t IO_ControlClass::GetSignalTrackInfo( void )
{
	return( m_usInputs & SI_SIGNAL_TRACK_INFO );
}


//******************************************************************
//	IsErlaubnisAbgabeEnabled
//
bool IO_ControlClass::IsErlaubnisAbgabeEnabled( void )
{
	return( m_usInputs & SI_ERLAUBNISABGABE_ENABLE );
}


//******************************************************************
//	IsStationConnected
//
bool IO_ControlClass::IsStationConnected( void )
{
	return( m_usInputs & SI_PRUEFSCHLEIFE_CLOSED );
}


//******************************************************************
//	SetAusfahrtMoeglich
//
void IO_ControlClass::SetAusfahrtMoeglich( bool bEnable )
{
	if( bEnable )
	{
		if( !(m_usOutputs & SI_AUSFAHRT_MOEGLICH) )
		{
			m_usOutputs |= SI_AUSFAHRT_MOEGLICH;

			m_bOutputChanged = true;
		}
	}
	else
	{
		if( m_usOutputs & SI_AUSFAHRT_MOEGLICH )
		{
			m_usOutputs &= ~SI_AUSFAHRT_MOEGLICH;

			m_bOutputChanged = true;
		}
	}
}


//******************************************************************
//	SetNotZwangshalt
//
void IO_ControlClass::SetNotZwangshalt( bool bEnable )
{
	if( bEnable )
	{
		if( !(m_usOutputs & SI_KEIN_ZWANGSHALT) )
		{
			m_usOutputs |= SI_KEIN_ZWANGSHALT;

			m_bOutputChanged = true;
		}
	}
	else
	{
		if( m_usOutputs & SI_KEIN_ZWANGSHALT )
		{
			m_usOutputs &= ~SI_KEIN_ZWANGSHALT;

			m_bOutputChanged = true;
		}
	}
}


//******************************************************************
//	ReadStationInterfaceInputs
//
void IO_ControlClass::ReadStationInterfaceInputs( void )
{
	uint8_t	usHelper;

	if( m_bIsStationInterface )
	{
		Wire.requestFrom( STATION_INTERFACE_ADR, 1 );

		while( Wire.available() )
		{
			usHelper	= Wire.read();
			m_usInputs	= ~usHelper;
		}
	}
}


//******************************************************************
//	ReadStationInterfaceInputs
//
void IO_ControlClass::SetStationInterfaceOutputs( void )
{
	if( m_bIsStationInterface && m_bOutputChanged )
	{
		Wire.beginTransmission( STATION_INTERFACE_ADR );
		Wire.write( m_usOutputs );
		Wire.endTransmission();

		m_bOutputChanged = false;
	}
}
