//##########################################################################
//#
//#		LncvStorageClass
//#
//#	This class manages the LNCVs (L oco N et C onfiguration V ariables)
//#	These variables will be saved in the EEPROM.
//#
//#	To simplify access, the variables were arranged one after the other
//#	without gaps. This makes it very easy to read and write the variables
//#	with a tool.
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	1.08	vom: 08.07.2022
//#
//#	Implementation:
//#		-	add address to reset the box per loconet messages
//#		-	new configuration method:
//#			address, switch, sensor, green and red is configured in
//#			one word. The word has the following format:
//#			xxxx m	-	xxxx	address
//#						m		mode
//#								0	-	switch msg RED   (0) active
//#								1	-	switch msg GREEN (1) active
//#								2	-	sensor LOW  (0) active
//#								3	-	sensor HIGH (1) active
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	1.07	vom: 04.03.2022
//#
//#	Implementation:
//#		-	add address to enable handling of train number messages
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	1.06	vom: 26.02.2022
//#
//#	Implementation:
//#		-	add address for annunciator field use for train numbers
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	1.05	vom: 25.02.2022
//#
//#	Umsetzung:
//#		-	integration of train numbers
//#
//#-------------------------------------------------------------------------
//#
//#	Version: 1.04	vom: 07.01.2022
//#
//#	Umsetzung:
//#		-	Anpassung an Platine Version 6
//#			Einbindung der DIP-Switches für die Konfiguration
//#
//#-------------------------------------------------------------------------
//#	Version: 1.03	vom: 29.12.2021
//#
//#	Umsetzung:
//#		-	Neue LNCV #50 eingebaut für Block On / Off.
//#		-	Neue Funktionen für Block On / Off eingebaut:
//#				SetBlockOn( bool )
//#				IsBlockOn()
//#
//#-------------------------------------------------------------------------
//#	Version: 1.02	vom: 01.12.2021
//#
//#	Umsetzung:
//#		-	Platz geschaffen für neue Konfigurations-LNCVs.
//#		-	3 LNCVs für Timer (Entry, Exit, Contact) und die entsprechenden
//#			Zugriffsfunktionen hinzugefügt.
//#			(Default-Zeiten der Timer = 1s bzw. 1000ms)
//#
//#-------------------------------------------------------------------------
//#	Version: 1.01	vom: 28.10.2021
//#
//#	Umsetzung:
//#		-	Neue LNCV #9 für eine Wartezeit zwischen zwei zu sendenden
//#			Loconet-Nachrichten eingebaut.
//#			Beim Einlesen der Konfigurationswerte wird darauf geachtet,
//#			dass die Wartezeit nicht kürzer als 5 ms ist.
//#			Die Default Wartezeit ist 10 ms
//#
//#-------------------------------------------------------------------------
//#	Version: 1.0	Date: 14.09.2021
//#
//#	Implementation:
//#		-	first version
//#
//##########################################################################


//==========================================================================
//
//		I N C L U D E S
//
//==========================================================================

#include "compile_options.h"

#include <Arduino.h>

#ifdef DEBUGGING_PRINTOUT
#include "debugging.h"
#endif

#include "lncv_storage.h"
#include "io_control.h"


//==========================================================================
//
//		G L O B A L   V A R I A B L E S
//
//==========================================================================

LncvStorageClass	g_clLncvStorage = LncvStorageClass();


//==========================================================================
//
//		D E F I N I T I O N S
//
//==========================================================================

#define	MIN_SEND_DELAY_TIME			   5
#define DEFAULT_SEND_DELAY_TIME		  10
#define	DEFAULT_TIMER_TIME			1000

#define SWITCH_RED		0
#define SWITCH_GREEN	1
#define SENSOR_LOW		2
#define SENSOR_HIGH		3


////////////////////////////////////////////////////////////////////////
//	CLASS: LncvStorageClass
//

//**********************************************************************
//	Constructor
//----------------------------------------------------------------------
//
LncvStorageClass::LncvStorageClass()
{
}


//**********************************************************************
//	CheckEEPROM
//----------------------------------------------------------------------
//	This function checks if the EEPROM is empty (0xFF in the cells).
//	If so, then the EEPROM will be filled with default config infos
//	and all addresses will be set to zero.
//
void LncvStorageClass::CheckEEPROM( void )
{
	uint8_t	byte0 = eeprom_read_byte( (uint8_t *)0 );
	uint8_t	byte1 = eeprom_read_byte( (uint8_t *)1 );

#ifdef DEBUGGING_PRINTOUT
	g_clDebugging.PrintStorageCheck( byte0, byte1 );
#endif

	if( (0xFF == byte0) && (0xFF == byte1) )
	{
		//----------------------------------------------------------
		//	the EEPROM is empty, so write default config info ...
		//

#ifdef DEBUGGING_PRINTOUT
		g_clDebugging.PrintStorageDefault();
#endif

		WriteLNCV( LNCV_ADR_MODULE_ADRESS,	1 );							//	default Module Adress 0x0001
		WriteLNCV( LNCV_ADR_ARTIKEL_NUMMER,	ARTIKEL_NUMMER );				//	Artikel-Nummer
		WriteLNCV( LNCV_ADR_CONFIGURATION,  ANRUECKMELDER_FROM_LN2BLOCK );	//	TF71 Setting, Loconet2Block steuert Anrückmelder
		WriteLNCV( LNCV_ADR_CONFIG_RECEIVE,   0x0000 );						//	Empfang: alle Nachrichten als Switch-Message
		WriteLNCV( LNCV_ADR_CONFIG_SEND_LOW,  0x0000 );						//	Senden:  alle Nachrichten
		WriteLNCV( LNCV_ADR_CONFIG_SEND_HIGH, 0x0000 );						//			 als Switch-Message
		WriteLNCV( LNCV_ADR_INVERT_RECEIVE,   0x0000 );						//	Empfang: nicht invertieren
		WriteLNCV( LNCV_ADR_INVERT_SEND_LOW,  0x0000 );						//	Senden:  nicht invertieren
		WriteLNCV( LNCV_ADR_INVERT_SEND_HIGH, 0x0000 );
		WriteLNCV( LNCV_ADR_SEND_DELAY, DEFAULT_SEND_DELAY_TIME );			//	Send Delay Timer
		WriteLNCV( LNCV_ADR_TIMER_ENTRY_TIME,   DEFAULT_TIMER_TIME );		//	Entry Timer
		WriteLNCV( LNCV_ADR_TIMER_EXIT_TIME,    DEFAULT_TIMER_TIME );		//	Exit Timer
		WriteLNCV( LNCV_ADR_TIMER_CONTACT_TIME, DEFAULT_TIMER_TIME );		//	Contact Timer
		WriteLNCV( LNCV_ADR_RESET, 0 );

		//----------------------------------------------------------
		//	... and default address values into EEPROM
		//
		for( uint8_t idx = LNCV_ADR_RESET + 1 ; idx <= LNCV_ADR_HUPE ; idx++ )
		{
			WriteLNCV( idx, 0 );
		}

		//----------------------------------------------------------
		//	default for Block is ON
		//
		WriteLNCV( LNCV_ADR_BLOCK_ON_OFF, 0x0001 );		//	default Block = ON
	}
	
	delay( 1000 );
}

//**********************************************************************
//	Init
//----------------------------------------------------------------------
//	This function will read the saved
//		-	configuration information
//		-	addresses for IN messages
//		-	addresses for OUT messages
//	from the EEPROM.
//	This makes it easyer to interpret IN messages and send OUT messages.
//
void LncvStorageClass::Init( void )
{
	uint16_t	mask16;
	uint32_t	mask32;
	uint16_t	helper16;


#ifdef DEBUGGING_PRINTOUT
	g_clDebugging.PrintStorageRead();
#endif

	//--------------------------------------------------------------
	//	read config information
	//
	m_uiConfiguration	  = ReadLNCV( LNCV_ADR_CONFIGURATION );
	m_uiTimerEntryTime	  = ReadLNCV( LNCV_ADR_TIMER_ENTRY_TIME );
	m_uiTimerExitTime	  = ReadLNCV( LNCV_ADR_TIMER_EXIT_TIME );
	m_uiTimerContactTime  = ReadLNCV( LNCV_ADR_TIMER_CONTACT_TIME );

	//--------------------------------------------------------------
	//	read send delay time
	//	and make sure it is not shorter than MIN_SEND_DELAY_TIME ms
	//
	m_uiSendDelay = ReadLNCV( LNCV_ADR_SEND_DELAY );

	if( MIN_SEND_DELAY_TIME > m_uiSendDelay )
	{
		m_uiSendDelay = MIN_SEND_DELAY_TIME;
	}

	//--------------------------------------------------------------
	//	read "block on" info
	//
	if( 0 == ReadLNCV( LNCV_ADR_BLOCK_ON_OFF ) )
	{
		m_BlockOn = false;
	}
	else
	{
		m_BlockOn = true;
	}

	//--------------------------------------------------------------
	//	read addresses for train number messages
	//
	m_uiTrainNoEnable		= ReadLNCV( LNCV_ADR_TRAIN_NO_ENABLE );
	m_uiTrainNoOffer		= ReadLNCV( LNCV_ADR_TRAIN_NO_OFFER );
	m_uiTrainNoAnnunciator	= ReadLNCV( LNCV_ADR_TRAIN_NO_ANNUNCIATOR );
	m_uiTrainNoTrack		= ReadLNCV( LNCV_ADR_TRAIN_NO_TRACK );

	//--------------------------------------------------------------
	//	read addresses for IN messages
	//
	m_uiConfigReceive	= 0x0000;
	m_uiInvertReceive	= 0x0000;
	mask16				= 0x0001;

	for( uint8_t idx = IN_IDX_EINFAHR_SIGNAL ; idx <= IN_IDX_KEY_RELEASED ; idx++ )
	{
		helper16				 = ReadLNCV( LNCV_ADR_EINFAHR_SIGNAL + idx );
		m_auiAdresseIn[ idx ]	 = helper16 / 10;
		helper16				-= (m_auiAdresseIn[ idx ] * 10);

		switch( helper16 )
		{
			case SWITCH_RED:
				m_uiInvertReceive |= mask16;
				break;

			case SENSOR_LOW:
				m_uiInvertReceive |= mask16;
				//
				//	no break is correct here

			case SENSOR_HIGH:
				m_uiConfigReceive |= mask16;
				break;

			case SWITCH_GREEN:
			default:
				break;
		}

		mask16 <<= 1;
	}

	//--------------------------------------------------------------
	//	read addresses for OUT messages
	//
	m_ulConfigSend	= 0x00000000;
	m_ulInvertSend	= 0x00000000;
	mask32			= 0x00000001;

	for( uint8_t idx = OUT_IDX_FAHRT_MOEGLICH ; idx <= OUT_IDX_HUPE ; idx++ )
	{
		helper16				 = ReadLNCV( LNCV_ADR_FAHRT_MOEGLICH + idx );
		m_auiAdresseOut[ idx ]	 = helper16 / 10;
		helper16				-= (m_auiAdresseOut[ idx ] * 10);

		switch( helper16 )
		{
			case SWITCH_RED:
				m_ulInvertSend |= mask32;
				break;

			case SENSOR_LOW:
				m_ulInvertSend |= mask32;
				//
				//	no break is correct here

			case SENSOR_HIGH:
				m_ulConfigSend |= mask32;
				break;

			case SWITCH_GREEN:
			default:
				break;
		}

		mask32 <<= 1;
	}

	delay( 100 );


	//--------------------------------------------------------------
	//	check configuration from DIP switches
	//
	if( g_clControl.IsConfigKey() )
	{
		m_uiConfiguration |= KEY_INTERFACE;
	}

	if( g_clControl.IsConfigKeyByBox() )
	{
		m_uiConfiguration |= KEY_BOX_DIRECT;
	}

	if( g_clControl.IsConfigRichtungsbetrieb() )
	{
		m_uiConfiguration |= RICHTUNGSBETRIEB;
	}

	delay( 100 );
}


//**********************************************************************
//	IsValidLNCVAdress
//
bool LncvStorageClass::IsValidLNCVAddress( uint16_t Adresse )
{
	if( (LNCV_ADR_MODULE_ADRESS <= Adresse) && (LNCV_ADR_HUPE >= Adresse) )
	{
		return( true );
	}

	return( false );
}


//**********************************************************************
//	ReadLNCV
//
uint16_t LncvStorageClass::ReadLNCV( uint16_t Adresse )
{
	uint16_t	value;

	//--------------------------------------------------------------
	//	because of uint16 values the address has to be shifted
	//	by '1' (this will double the address).
	//
	value = eeprom_read_word( (uint16_t *)(Adresse << 1) );

	return( value );
}


//**********************************************************************
//	WriteLNCV
//
void LncvStorageClass::WriteLNCV( uint16_t Adresse, uint16_t Value )
{
	//--------------------------------------------------------------
	//	because of uint16 values the address has to be shifted
	//	by '1' (this will double the address).
	//
	eeprom_write_word( (uint16_t *)(Adresse << 1), Value );
}


//**********************************************************************
//	SetBlockOn
//
void LncvStorageClass::SetBlockOn( bool state )
{
	m_BlockOn = state;

	if( m_BlockOn )
	{
		WriteLNCV( LNCV_ADR_BLOCK_ON_OFF, 0x0001 );
	}
	else
	{
		WriteLNCV( LNCV_ADR_BLOCK_ON_OFF, 0x0000 );
	}
}
