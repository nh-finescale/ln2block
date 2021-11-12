
#pragma once

//##########################################################################
//#
//#		LncvStorageClass
//#
//#	This class manages the LNCVs (L oco N et C onfiguration V ariables)
//#	These variables will be saved in the EEPROM.
//#
//##########################################################################


//==========================================================================
//
//		D E F I N I T I O N S
//
//==========================================================================

//----------------------------------------------------------------------
//	my artikle number
#define ARTIKEL_NUMMER	1510


//----------------------------------------------------------------------
//	definitions for optional features
//
#define	KEY_INTERFACE					0x0001
#define	KEY_BOX_DIRECT					0x0002
#define	CONTACT_INTERN					0x0004
#define ANRUECKMELDER_FROM_LN2BLOCK		0x0008
#define	PRUEFSCHLEIFE_OK				0x0010
#define	GERAEUMT_NACH_5_SEC				0x0020
#define DISPLAY_FLIP					0x0040


//----------------------------------------------------------------------
//	address definitions for config informations
//
#define LNCV_ADR_MODULE_ADRESS						0
#define LNCV_ADR_ARTIKEL_NUMMER						1
#define LNCV_ADR_CONFIGURATION						2
#define LNCV_ADR_CONFIG_RECEIVE						3
#define LNCV_ADR_CONFIG_SEND_LOW					4
#define LNCV_ADR_CONFIG_SEND_HIGH					5
#define LNCV_ADR_INVERT_RECEIVE						6
#define LNCV_ADR_INVERT_SEND_LOW					7
#define LNCV_ADR_INVERT_SEND_HIGH					8
#define LNCV_ADR_SEND_DELAY							9

//----------------------------------------------------------------------
//	address definitions for IN messages
//
#define	LNCV_ADR_EINFAHR_SIGNAL						10
#define	LNCV_ADR_AUSFAHR_SIGNAL						11
#define LNCV_ADR_EINFAHR_KONTAKT					12
#define LNCV_ADR_AUSFAHR_KONTAKT					13
#define LNCV_ADR_BEDIENUNG_RUECKBLOCK				14
#define LNCV_ADR_BEDIENUNG_HILFSVORBLOCK			15
#define LNCV_ADR_BEDIENUNG_ERLAUBNISABGABE			16
#define LNCV_ADR_BEDIENUNG_ANSCHALTER_EIN			17
#define LNCV_ADR_BEDIENUNG_ANSCHALTER_AUS			18
#define LNCV_ADR_KEY_RELEASED						19

//----------------------------------------------------------------------
//	address definitions for OUT messages
//
#define LNCV_ADR_FAHRT_MOEGLICH						20
#define LNCV_ADR_NICHT_ZWANGSHALT					21
#define LNCV_ADR_ERLAUBNISWECHSELSPERRE				22
#define LNCV_ADR_PRUEFSCHLEIFE						23
#define LNCV_ADR_SCHLUESSELENTNAHME_MOEGLICH		24
#define LNCV_ADR_AUSFAHRSPERRMELDER_TF71			25
#define	LNCV_ADR_BLOCKMELDER_TF71					26
#define LNCV_ADR_WIEDERHOLSPERRMELDER_RELAISBLOCK	27
#define LNCV_ADR_VORBLOCKMELDER_RELAISBLOCK			28
#define LNCV_ADR_RUECKBLOCKMELDER_RELAISBLOCK		29
#define LNCV_ADR_MELDER_ANSCHALTER					30
#define LNCV_ADR_MELDER_ERSTE_ACHSE					31
#define LNCV_ADR_MELDER_GERAEUMT					32
#define LNCV_ADR_MELDER_GERAEUMT_BLINKEN			33
#define LNCV_ADR_UEBERTRAGUNGSSTOERUNG				34
#define LNCV_ADR_MELDER_ERLAUBNIS_ERHALTEN			35
#define LNCV_ADR_MELDER_ERLAUBNIS_ABGEGEBEN			36
#define LNCV_ADR_HUPE								37


//----------------------------------------------------------------------
//	index definitions related to IN messages
//
#define	IN_IDX_EINFAHR_SIGNAL						0
#define	IN_IDX_AUSFAHR_SIGNAL						1
#define IN_IDX_EINFAHR_KONTAKT						2
#define IN_IDX_AUSFAHR_KONTAKT						3
#define IN_IDX_BEDIENUNG_RUECKBLOCK					4
#define IN_IDX_BEDIENUNG_HILFSVORBLOCK				5
#define IN_IDX_BEDIENUNG_ERLAUBNISABGABE			6
#define IN_IDX_BEDIENUNG_ANSCHALTER_EIN				7
#define IN_IDX_BEDIENUNG_ANSCHALTER_AUS				8
#define IN_IDX_KEY_RELEASED							9

//----------------------------------------------------------------------
//	index definitions related to OUT messages
//
#define OUT_IDX_FAHRT_MOEGLICH						0
#define OUT_IDX_NICHT_ZWANGSHALT					1
#define OUT_IDX_ERLAUBNISWECHSELSPERRE				2
#define OUT_IDX_PRUEFSCHLEIFE						3
#define OUT_IDX_SCHLUESSELENTNAHME_MOEGLICH			4
#define OUT_IDX_AUSFAHRSPERRMELDER_TF71				5
#define	OUT_IDX_BLOCKMELDER_TF71					6
#define OUT_IDX_WIEDERHOLSPERRMELDER_RELAISBLOCK	7
#define OUT_IDX_VORBLOCKMELDER_RELAISBLOCK			8
#define OUT_IDX_RUECKBLOCKMELDER_RELAISBLOCK		9
#define OUT_IDX_MELDER_ANSCHALTER					10
#define OUT_IDX_MELDER_ERSTE_ACHSE					11
#define OUT_IDX_MELDER_GERAEUMT						12
#define OUT_IDX_MELDER_GERAEUMT_BLINKEN				13
#define OUT_IDX_UEBERTRAGUNGSSTOERUNG				14
#define OUT_IDX_MELDER_ERLAUBNIS_ERHALTEN			15
#define OUT_IDX_MELDER_ERLAUBNIS_ABGEGEBEN			16
#define OUT_IDX_HUPE								17


//----------------------------------------------------------------------
//	number of input and output address fields
//
#define	LOCONET_IN_COUNT		( IN_IDX_KEY_RELEASED + 1)
#define LOCONET_OUT_COUNT		(OUT_IDX_HUPE + 1)


////////////////////////////////////////////////////////////////////////
//	CLASS:	LncvStorageClass
//
class LncvStorageClass
{
	private:
		uint16_t	m_uiConfiguration;
		uint16_t	m_uiConfigReceive;
		uint32_t	m_uiConfigSend;
		uint16_t	m_uiInvertReceive;
		uint32_t	m_ulInvertSend;
		uint16_t	m_uiSendDelay;
		uint16_t	m_auiAdresseIn[  LOCONET_IN_COUNT ];
		uint16_t	m_auiAdresseOut[ LOCONET_OUT_COUNT ];

	public:
		//----------------------------------------------------------
		//	Constructor
		//
		LncvStorageClass();

		//----------------------------------------------------------
		//
		inline uint32_t GetConfigSend( void )
		{
			return( m_uiConfigSend );
		}

		//----------------------------------------------------------
		//
		inline uint16_t GetConfigReceive( void )
		{
			return( m_uiConfigReceive );
		}

		//----------------------------------------------------------
		//
		inline uint32_t GetInvertSend( void )
		{
			return( m_ulInvertSend );
		}

		//----------------------------------------------------------
		//
		inline uint16_t GetInvertReceive( void )
		{
			return( m_uiInvertReceive );
		}

		//----------------------------------------------------------
		//
		inline uint16_t GetSendDelayTime( void )
		{
			return( m_uiSendDelay );
		}

		//----------------------------------------------------------
		//
		inline uint16_t GetInAddress( uint8_t idx )
		{
			return( m_auiAdresseIn[ idx ] );
		}

		//----------------------------------------------------------
		//
		inline uint16_t GetOutAddress( uint8_t idx )
		{
			return( m_auiAdresseOut[ idx ] );
		}

		//----------------------------------------------------------
		//
		inline bool IsConfigSet( uint16_t mask )
		{
			return( 0 != (m_uiConfiguration & mask) );
		}

		//----------------------------------------------------------
		//
		inline bool IsConfigSetAll( uint16_t mask )
		{
			return( 0 == ~(m_uiConfiguration | ~mask) );
		}

		void CheckEEPROM( void );
		void Init( void );
		bool IsValidLNCVAddress( uint16_t Adresse );
		uint16_t ReadLNCV( uint16_t Adresse );
		void WriteLNCV( uint16_t Adresse, uint16_t Value );
};


//==========================================================================
//
//		E X T E R N   G L O B A L   V A R I A B L E S
//
//==========================================================================

extern LncvStorageClass		g_clLncvStorage;
