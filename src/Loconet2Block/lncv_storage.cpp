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

		WriteLNCV( LNCV_ADR_MODULE_ADRESS,	1 );									//	default Module Adress 0x0001
		WriteLNCV( LNCV_ADR_ARTIKEL_NUMMER,	ARTIKEL_NUMMER );						//	Artikel-Nummer
		WriteLNCV( LNCV_ADR_CONFIGURATION,  ANRUECKMELDER_FROM_LN2BLOCK );			//	TF71 Setting, Loconet2Block steuert Anrückmelder
		WriteLNCV( LNCV_ADR_CONFIG_RECEIVE, (	(1 << IN_IDX_EINFAHR_KONTAKT)
											|	(1 << IN_IDX_AUSFAHR_KONTAKT)) );	//	Empfang: Gleiskontakte als Sensor-Message
		WriteLNCV( LNCV_ADR_CONFIG_SEND_LOW,  0x0000 );								//	Senden: alle Nachrichten
		WriteLNCV( LNCV_ADR_CONFIG_SEND_HIGH, 0x0000 );								//			als Switch-Message
		WriteLNCV( LNCV_ADR_INVERT_RECEIVE,   0x0000 );								//	Empfang: nicht invertieren
		WriteLNCV( LNCV_ADR_INVERT_SEND_LOW,  0x0000 );								//	Senden: nicht invertieren
		WriteLNCV( LNCV_ADR_INVERT_SEND_HIGH, 0x0000 );
		
		//----------------------------------------------------------
		//	... and default address values into EEPROM
		//
		for( uint8_t idx = LNCV_ADR_INVERT_SEND_HIGH + 1 ; idx <= LNCV_ADR_HUPE ; idx++ )
		{
			WriteLNCV( idx, 0 );
		}
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
#ifdef DEBUGGING_PRINTOUT
	g_clDebugging.PrintStorageRead();
#endif

	//--------------------------------------------------------------
	//	read config information
	//
	m_uiConfiguration	  = ReadLNCV( LNCV_ADR_CONFIGURATION );
	m_uiConfigReceive	  = ReadLNCV( LNCV_ADR_CONFIG_RECEIVE );
	m_uiConfigSend		  = ReadLNCV( LNCV_ADR_CONFIG_SEND_HIGH );
	m_uiConfigSend		<<= 16;
	m_uiConfigSend		 |= ReadLNCV( LNCV_ADR_CONFIG_SEND_LOW );
	m_uiInvertReceive	  = ReadLNCV( LNCV_ADR_INVERT_RECEIVE );
	m_ulInvertSend		  = ReadLNCV( LNCV_ADR_INVERT_SEND_HIGH );
	m_ulInvertSend		<<= 16;
	m_ulInvertSend		 |= ReadLNCV( LNCV_ADR_INVERT_SEND_LOW );

	//--------------------------------------------------------------
	//	read addresses for IN messages
	//
	for( uint8_t idx = IN_IDX_EINFAHR_SIGNAL ; idx <= IN_IDX_KEY_RELEASED ; idx++ )
	{
		m_auiAdresseIn[ idx ] = ReadLNCV( LNCV_ADR_EINFAHR_SIGNAL + idx );
	}

	//--------------------------------------------------------------
	//	read addresses for OUT messages
	//
	for( uint8_t idx = OUT_IDX_FAHRT_MOEGLICH ; idx <= OUT_IDX_HUPE ; idx++ )
	{
		m_auiAdresseOut[ idx ] = ReadLNCV( LNCV_ADR_FAHRT_MOEGLICH + idx );
	}

	delay( 200 );
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
