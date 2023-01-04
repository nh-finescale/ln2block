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
//#	File version:	16		from: 04.01.2023
//#
//#	Bug Fix:
//#		-	forget to read address to switch block ON/OFF
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	15		from: 09.12.2022
//#
//#	Implementation:
//#		-	add selection of different debug informations (LNCV #7)
//#			new variables
//#				m_bShowTrainNumbers
//#				m_uiAddressShowTrainNumbers
//#			new functions
//#				SetShowTrainNumbers()
//#				IsShowTrainNumbers()
//#				GetShowTrainNumbersAddress()
//#			changes in functions
//#				CheckEEPROM()
//#				Init()
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	14		from: 16.11.2022
//#
//#	Bug Fix:
//#		-	flags for "Block on" and "train numbers on" were not set
//#			correctly during 'Init()'.
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	13		from: 02.10.2022
//#
//#	Implementation:
//#		-	add version number into LNCV #5
//#		-	add new config bit 'CONTACT_REMAINS_ACTIVE'
//#			if this bit is set in the endfield state model the transition
//#			from 'signal_gezogen' to 'belegt' is disabled.
//#
//#	Bug Fix:
//#		-	only write to EEPROM if the new value is different
//#			than the old one
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	12		from: 09.09.2022
//#
//#	Implementation:
//#		-	add address to switch Block ON/OFF per loconet message
//#			new functions:
//#				GetBlockOnOffAddress()
//#				SetTrainNumbersOn()
//#				IsTrainNumbersOn()
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	11		from: 28.08.2022
//#
//#	Implementation:
//#		-	add address to send the state of all OUT-Loconet-Devices
//#			add function GetSendDeviceStateAddress()
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	10		from: 09.07.2022
//#
//#	Implementation:
//#		-	add address to reset the box per loconet messages
//#			add function GetResetAddress()
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	9		from: 08.07.2022
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
//#	File version:	8		from: 04.03.2022
//#
//#	Implementation:
//#		-	add address to enable handling of train number messages
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	7		from: 26.02.2022
//#
//#	Implementation:
//#		-	add address for annunciator field use for train numbers
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	6		from: 25.02.2022
//#
//#	Umsetzung:
//#		-	integration of train numbers
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	5		from: 07.01.2022
//#
//#	Umsetzung:
//#		-	Anpassung an Platine Version 6
//#			Einbindung der DIP-Switches für die Konfiguration
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	4		from: 29.12.2021
//#
//#	Umsetzung:
//#		-	Neue LNCV #50 eingebaut für Block On / Off.
//#		-	Neue Funktionen für Block On / Off eingebaut:
//#				SetBlockOn( bool )
//#				IsBlockOn()
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	3		from: 01.12.2021
//#
//#	Umsetzung:
//#		-	Platz geschaffen für neue Konfigurations-LNCVs.
//#		-	3 LNCVs für Timer (Entry, Exit, Contact) und die entsprechenden
//#			Zugriffsfunktionen hinzugefügt.
//#			(Default-Zeiten der Timer = 1s bzw. 1000ms)
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	2		from: 28.10.2021
//#
//#	Implementation:
//#		-	Neue LNCV #9 für eine Wartezeit zwischen zwei zu sendenden
//#			Loconet-Nachrichten eingebaut.
//#			Beim Einlesen der Konfigurationswerte wird darauf geachtet,
//#			dass die Wartezeit nicht kürzer als 5 ms ist.
//#			Die Default Wartezeit ist 10 ms
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	1		from: 14.09.2021
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
void LncvStorageClass::CheckEEPROM( uint16_t uiVersionNumber )
{
	uint16_t	uiAddress	= ReadLNCV( LNCV_ADR_MODULE_ADDRESS );
	uint16_t	uiArticle	= ReadLNCV( LNCV_ADR_ARTIKEL_NUMMER );


#ifdef DEBUGGING_PRINTOUT
	g_clDebugging.PrintStorageCheck( uiAddress, uiArticle );
#endif

	if( (0xFFFF == uiAddress) || (0x0000 == uiAddress) )
	{
		//----------------------------------------------------------
		//	the EEPROM is empty, so write default config info ...
		//

#ifdef DEBUGGING_PRINTOUT
		g_clDebugging.PrintStorageDefault();
#endif

		WriteLNCV( LNCV_ADR_MODULE_ADDRESS,	1 );							//	default Module Adress 0x0001
		WriteLNCV( LNCV_ADR_ARTIKEL_NUMMER,	ARTIKEL_NUMMER );				//	Artikel-Nummer
		WriteLNCV( LNCV_ADR_VERSION_NUMBER, uiVersionNumber );

		WriteLNCV( LNCV_ADR_CONFIGURATION, ANRUECKMELDER_FROM_LN2BLOCK );	//	TF71 Setting, Loconet2Block steuert Anrückmelder
		WriteLNCV( LNCV_ADR_SEND_STATE_OF_DEVICES,	0x0000 );				//	Zustand aller Devices senden
		WriteLNCV( LNCV_ADR_RESET,					0x0000 );				//	Adr for Reset over LN
		WriteLNCV( LNCV_ADR_BLOCK_ON_OFF,			0x0000 );				//	Block ON/OFF senden
		WriteLNCV( LNCV_ADR_SHOW_ZN_ON_DISPLAY,		0x0000 );				//	Adr to select display info

		WriteLNCV( LNCV_ADR_FREE_1, 0x0000 );

		WriteLNCV( LNCV_ADR_SEND_DELAY, DEFAULT_SEND_DELAY_TIME );			//	Send Delay Timer
		WriteLNCV( LNCV_ADR_TIMER_ENTRY_TIME,   DEFAULT_TIMER_TIME );		//	Entry Timer
		WriteLNCV( LNCV_ADR_TIMER_EXIT_TIME,    DEFAULT_TIMER_TIME );		//	Exit Timer
		WriteLNCV( LNCV_ADR_TIMER_CONTACT_TIME, DEFAULT_TIMER_TIME );		//	Contact Timer

		//----------------------------------------------------------
		//	... and default address values into EEPROM
		//
		for( uint8_t idx = LNCV_ADR_FREE_2 ; idx <= LNCV_ADR_HUPE ; idx++ )
		{
			WriteLNCV( idx, 0 );
		}

		//----------------------------------------------------------
		//	default states for
		//		Block			=	ON
		//		Train Numbers	=	OFF
		//
		WriteLNCV( LNCV_ADR_STATE_STORAGE, STATE_STORAGE_BLOCK_ON );		//	default Block = ON
	}
	else
	{
		WriteLNCV( LNCV_ADR_VERSION_NUMBER, uiVersionNumber );
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
	//	read states for
	//		-	block on
	//		-	train numbers on
	//		-	show train numbers
	//
	m_bBlockOn			= false;
	m_bTrainNumbersOn	= false;
	m_bShowTrainNumbers	= false;

	helper16	= ReadLNCV( LNCV_ADR_STATE_STORAGE );

	if( 0 != (helper16 & STATE_STORAGE_BLOCK_ON) )
	{
		m_bBlockOn = true;
	}

	if( 0 != (helper16 & STATE_STORAGE_TRAIN_NUMBERS_ON) )
	{
		m_bTrainNumbersOn = true;
	}

	if( 0 != (helper16 & STATE_STORAGE_SHOW_TRAIN_NUMBERS) )
	{
		m_bShowTrainNumbers = true;
	}

	//--------------------------------------------------------------
	//	read addresses for train number messages
	//
	m_uiTrainNoEnable		= ReadLNCV( LNCV_ADR_TRAIN_NO_ENABLE );
	m_uiTrainNoOffer		= ReadLNCV( LNCV_ADR_TRAIN_NO_OFFER );
	m_uiTrainNoAnnunciator	= ReadLNCV( LNCV_ADR_TRAIN_NO_ANNUNCIATOR );
	m_uiTrainNoTrack		= ReadLNCV( LNCV_ADR_TRAIN_NO_TRACK );

	//--------------------------------------------------------------
	//	read address for the reset function
	//
	m_uiAddressReset	= ReadLNCV( LNCV_ADR_RESET );

	//--------------------------------------------------------------
	//	read address to switch the block ON/OFF
	//
	m_uiAddressBlockOnOff = ReadLNCV( LNCV_ADR_BLOCK_ON_OFF );

	//--------------------------------------------------------------
	//	read address to send the states of all OUT-Loconet-Devices
	//
	m_uiAddressSendStates = ReadLNCV( LNCV_ADR_SEND_STATE_OF_DEVICES );

	//--------------------------------------------------------------
	//	read address to switch between showing
	//		-	Loconet and block messages
	//		-	train numbers
	//
	m_uiAddressShowTrainNumbers = ReadLNCV( LNCV_ADR_SHOW_ZN_ON_DISPLAY );

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
//******************************************************************
//	avoid compiler warning
//
//	if( (LNCV_ADR_MODDULE_ADRESS <= Adresse) && (LNCV_ADR_HUPE >= Adresse) )

	if( LNCV_ADR_HUPE >= Adresse )
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
void LncvStorageClass::WriteLNCV( uint16_t Address, uint16_t Value )
{
	//--------------------------------------------------------------
	//	because of uint16 values the address has to be shifted
	//	by '1' (this will double the address).
	//
	uint16_t *	puiAdr	= (uint16_t *)(Address << 1);
	uint16_t	uiValue	= eeprom_read_word( puiAdr );
	
	if( uiValue != Value )
	{
		eeprom_write_word( puiAdr, Value );
	}
}


//**********************************************************************
//	SetBlockOn
//
void LncvStorageClass::SetBlockOn( bool state )
{
	uint16_t	uiStorageState = ReadLNCV( LNCV_ADR_STATE_STORAGE );


	m_bBlockOn = state;

	if( m_bBlockOn )
	{
		uiStorageState |= STATE_STORAGE_BLOCK_ON;
	}
	else
	{
		uiStorageState &= ~STATE_STORAGE_BLOCK_ON;
	}
	
	WriteLNCV( LNCV_ADR_STATE_STORAGE, uiStorageState );
}


//**********************************************************************
//	SetTrainNumbersOn
//
void LncvStorageClass::SetTrainNumbersOn( bool state )
{
	uint16_t	uiStorageState = ReadLNCV( LNCV_ADR_STATE_STORAGE );


	m_bTrainNumbersOn = state;

	if( m_bTrainNumbersOn )
	{
		uiStorageState |= STATE_STORAGE_TRAIN_NUMBERS_ON;
	}
	else
	{
		uiStorageState &= ~STATE_STORAGE_TRAIN_NUMBERS_ON;
	}
	
	WriteLNCV( LNCV_ADR_STATE_STORAGE, uiStorageState );
}


//**********************************************************************
//	SetShowTrainNumbers
//
void LncvStorageClass::SetShowTrainNumbers( bool state )
{
	uint16_t	uiStorageState = ReadLNCV( LNCV_ADR_STATE_STORAGE );


	m_bShowTrainNumbers = state;

	if( m_bShowTrainNumbers )
	{
		uiStorageState |= STATE_STORAGE_SHOW_TRAIN_NUMBERS;
	}
	else
	{
		uiStorageState &= ~STATE_STORAGE_SHOW_TRAIN_NUMBERS;
	}
	
	WriteLNCV( LNCV_ADR_STATE_STORAGE, uiStorageState );
}
