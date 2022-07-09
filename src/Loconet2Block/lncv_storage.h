
#pragma once

//##########################################################################
//#
//#		LncvStorageClass
//#
//#	This class manages the LNCVs (L oco N et C onfiguration V ariables)
//#	These variables will be saved in the EEPROM.
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	1.06	vom: 09.07.2022
//#
//#	Implementation:
//#		-	add address to reset the box per loconet messages
//#			add function GetResetAddress()
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	1.05	vom: 08.07.2022
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
//#	File version:	1.04	vom: 04.03.2022
//#
//#	Implementation:
//#		-	add address to enable handling of train number messages
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	1.03	vom: 26.02.2022
//#
//#	Implementation:
//#		-	add address for annunciator field use for train numbers
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	1.02	vom: 25.02.2022
//#
//#	Implementation:
//#		-	integration of train numbers
//#				-	new config bit TRAIN_NUMBERS
//#				-	new functions to get the addresses for train no
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	1.01	vom: 25.02.2022
//#
//#	Umsetzung:
//#		-	New configuration bit (SPLIT_PERMIT_INDICATOR_MSG) to send
//#			individual messages for 'Erlaubnis abgegeben' and
//#			'Erlaubnis erhalten'.
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
#define	KEY_INTERFACE					0x0001		//	dez.:     1
#define	KEY_BOX_DIRECT					0x0002		//	dez.:     2
#define RICHTUNGSBETRIEB				0x0004		//	dez.:     4
//#define FREE_CONFIG_BIT_3				0x0008		//	dieses Bit ist frei
#define FELDERBLOCK						0x0010		//	dez.:    16
#define	PRUEFSCHLEIFE_OK				0x0020		//	dez.:    32
#define ANRUECKMELDER_FROM_LN2BLOCK		0x0040		//	dez.:    64
#define	CONTACT_INTERN					0x0080		//	dez.:   128
#define DISPLAY_FLIP					0x0100		//	dez.:   256
#define TIMER_ENTRY_RETRIGGER			0x0200		//	dez.:   512
#define TIMER_EXIT_ACTIVE				0x0400		//	dez.:  1024
#define TIMER_EXIT_RETRIGGER			0x0800		//	dez.:  2048
#define SPLIT_PERMIT_INDICATOR_MSG		0x1000		//	dez.:  4096
#define TRAIN_NUMBERS					0x2000		//	dez.:  8192


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
#define LNCV_ADR_TIMER_ENTRY_TIME					10
#define LNCV_ADR_TIMER_EXIT_TIME					11
#define LNCV_ADR_TIMER_CONTACT_TIME					12

#define LNCV_ADR_RESET								13

#define LNCV_ADR_TRAIN_NO_ENABLE					16
#define LNCV_ADR_TRAIN_NO_OFFER						17
#define LNCV_ADR_TRAIN_NO_ANNUNCIATOR				18
#define LNCV_ADR_TRAIN_NO_TRACK						19

//----------------------------------------------------------------------
//	address definitions for IN messages
//
#define	LNCV_ADR_EINFAHR_SIGNAL						20
#define	LNCV_ADR_AUSFAHR_SIGNAL						21
#define LNCV_ADR_EINFAHR_KONTAKT					22
#define LNCV_ADR_AUSFAHR_KONTAKT					23
#define LNCV_ADR_BEDIENUNG_RUECKBLOCK				24
#define LNCV_ADR_BEDIENUNG_HILFSVORBLOCK			25
#define LNCV_ADR_BEDIENUNG_ERLAUBNISABGABE			26
#define LNCV_ADR_BEDIENUNG_ANSCHALTER_EIN			27
#define LNCV_ADR_BEDIENUNG_ANSCHALTER_AUS			28
#define LNCV_ADR_KEY_RELEASED						29

//----------------------------------------------------------------------
//	address definitions for OUT messages
//
#define LNCV_ADR_FAHRT_MOEGLICH						30
#define LNCV_ADR_NICHT_ZWANGSHALT					31
#define LNCV_ADR_ERLAUBNISWECHSELSPERRE				32
#define LNCV_ADR_PRUEFSCHLEIFE						33
#define LNCV_ADR_SCHLUESSELENTNAHME_MOEGLICH		34
#define LNCV_ADR_AUSFAHRSPERRMELDER_TF71			35
#define	LNCV_ADR_BLOCKMELDER_TF71					36
#define LNCV_ADR_WIEDERHOLSPERRMELDER_RELAISBLOCK	37
#define LNCV_ADR_VORBLOCKMELDER_RELAISBLOCK			38
#define LNCV_ADR_RUECKBLOCKMELDER_RELAISBLOCK		39
#define LNCV_ADR_MELDER_ANSCHALTER					40
#define LNCV_ADR_MELDER_ERSTE_ACHSE					41
#define LNCV_ADR_MELDER_GERAEUMT					42
#define LNCV_ADR_MELDER_GERAEUMT_BLINKEN			43
#define LNCV_ADR_UEBERTRAGUNGSSTOERUNG				44
#define LNCV_ADR_MELDER_ERLAUBNIS_ERHALTEN			45
#define LNCV_ADR_MELDER_ERLAUBNIS_ABGEGEBEN			46
#define LNCV_ADR_HUPE								47


//----------------------------------------------------------------------
//	addresses for internal use
//
#define LNCV_ADR_BLOCK_ON_OFF						50


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
//	mask definitions related to IN messages
//
#define	IN_MASK_EINFAHR_SIGNAL				((uint16_t)1 << IN_IDX_EINFAHR_SIGNAL)
#define	IN_MASK_AUSFAHR_SIGNAL				((uint16_t)1 << IN_IDX_AUSFAHR_SIGNAL)
#define IN_MASK_EINFAHR_KONTAKT				((uint16_t)1 << IN_IDX_EINFAHR_KONTAKT)
#define IN_MASK_AUSFAHR_KONTAKT				((uint16_t)1 << IN_IDX_AUSFAHR_KONTAKT)
#define IN_MASK_BEDIENUNG_RUECKBLOCK		((uint16_t)1 << IN_IDX_BEDIENUNG_RUECKBLOCK)
#define IN_MASK_BEDIENUNG_HILFSVORBLOCK		((uint16_t)1 << IN_IDX_BEDIENUNG_HILFSVORBLOCK)
#define IN_MASK_BEDIENUNG_ERLAUBNISABGABE	((uint16_t)1 << IN_IDX_BEDIENUNG_ERLAUBNISABGABE)
#define IN_MASK_BEDIENUNG_ANSCHALTER_EIN	((uint16_t)1 << IN_IDX_BEDIENUNG_ANSCHALTER_EIN)
#define IN_MASK_BEDIENUNG_ANSCHALTER_AUS	((uint16_t)1 << IN_IDX_BEDIENUNG_ANSCHALTER_AUS)
#define IN_MASK_KEY_RELEASED				((uint16_t)1 << IN_IDX_KEY_RELEASED)

//----------------------------------------------------------------------
//	mask definitions related to OUT messages
//
#define OUT_MASK_FAHRT_MOEGLICH						((uint32_t)1 << OUT_IDX_FAHRT_MOEGLICH)
#define OUT_MASK_NICHT_ZWANGSHALT					((uint32_t)1 << OUT_IDX_NICHT_ZWANGSHALT)
#define OUT_MASK_ERLAUBNISWECHSELSPERRE				((uint32_t)1 << OUT_IDX_ERLAUBNISWECHSELSPERRE)
#define OUT_MASK_PRUEFSCHLEIFE						((uint32_t)1 << OUT_IDX_PRUEFSCHLEIFE)
#define OUT_MASK_SCHLUESSELENTNAHME_MOEGLICH		((uint32_t)1 << OUT_IDX_SCHLUESSELENTNAHME_MOEGLICH)
#define OUT_MASK_AUSFAHRSPERRMELDER_TF71			((uint32_t)1 << OUT_IDX_AUSFAHRSPERRMELDER_TF71)
#define	OUT_MASK_BLOCKMELDER_TF71					((uint32_t)1 << OUT_IDX_BLOCKMELDER_TF71)
#define OUT_MASK_WIEDERHOLSPERRMELDER_RELAISBLOCK	((uint32_t)1 << OUT_IDX_WIEDERHOLSPERRMELDER_RELAISBLOCK)
#define OUT_MASK_VORBLOCKMELDER_RELAISBLOCK			((uint32_t)1 << OUT_IDX_VORBLOCKMELDER_RELAISBLOCK)
#define OUT_MASK_RUECKBLOCKMELDER_RELAISBLOCK		((uint32_t)1 << OUT_IDX_RUECKBLOCKMELDER_RELAISBLOCK)
#define OUT_MASK_MELDER_ANSCHALTER					((uint32_t)1 << OUT_IDX_MELDER_ANSCHALTER)
#define OUT_MASK_MELDER_ERSTE_ACHSE					((uint32_t)1 << OUT_IDX_MELDER_ERSTE_ACHSE)
#define OUT_MASK_MELDER_GERAEUMT					((uint32_t)1 << OUT_IDX_MELDER_GERAEUMT)
#define OUT_MASK_MELDER_GERAEUMT_BLINKEN			((uint32_t)1 << OUT_IDX_MELDER_GERAEUMT_BLINKEN)
#define OUT_MASK_UEBERTRAGUNGSSTOERUNG				((uint32_t)1 << OUT_IDX_UEBERTRAGUNGSSTOERUNG)
#define OUT_MASK_MELDER_ERLAUBNIS_ERHALTEN			((uint32_t)1 << OUT_IDX_MELDER_ERLAUBNIS_ERHALTEN)
#define OUT_MASK_MELDER_ERLAUBNIS_ABGEGEBEN			((uint32_t)1 << OUT_IDX_MELDER_ERLAUBNIS_ABGEGEBEN)
#define OUT_MASK_HUPE								((uint32_t)1 << OUT_IDX_HUPE)


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
		uint32_t	m_ulConfigSend;
		uint16_t	m_uiInvertReceive;
		uint32_t	m_ulInvertSend;
		uint16_t	m_uiSendDelay;
		uint16_t	m_uiTimerEntryTime;
		uint16_t	m_uiTimerExitTime;
		uint16_t	m_uiTimerContactTime;
		uint16_t	m_uiTrainNoEnable;
		uint16_t	m_uiTrainNoOffer;
		uint16_t	m_uiTrainNoAnnunciator;
		uint16_t	m_uiTrainNoTrack;
		uint16_t	m_uiAddressReset;
		uint16_t	m_auiAdresseIn[  LOCONET_IN_COUNT ];
		uint16_t	m_auiAdresseOut[ LOCONET_OUT_COUNT ];
		bool		m_BlockOn;

	public:
		//----------------------------------------------------------
		//	Constructor
		//
		LncvStorageClass();

		//----------------------------------------------------------
		//
		inline uint32_t GetConfigSend( void )
		{
			return( m_ulConfigSend );
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
		inline uint16_t GetTimerEntryTime( void )
		{
			return( m_uiTimerEntryTime );
		}

		//----------------------------------------------------------
		//
		inline uint16_t GetTimerExitTime( void )
		{
			return( m_uiTimerExitTime );
		}

		//----------------------------------------------------------
		//
		inline uint16_t GetTimerContactTime( void )
		{
			return( m_uiTimerContactTime );
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
		inline uint16_t GetTrainNoAddressEnable( void )
		{
			return( m_uiTrainNoEnable );
		}

		//----------------------------------------------------------
		//
		inline uint16_t GetTrainNoAddressOffer( void )
		{
			return( m_uiTrainNoOffer );
		}

		//----------------------------------------------------------
		//
		inline uint16_t GetTrainNoAddressAnnunciator( void )
		{
			return( m_uiTrainNoAnnunciator );
		}

		//----------------------------------------------------------
		//
		inline uint16_t GetTrainNoAddressTrack( void )
		{
			return( m_uiTrainNoTrack );
		}

		//----------------------------------------------------------
		//
		inline uint16_t GetResetAddress( void )
		{
			return( m_uiAddressReset );
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

		//----------------------------------------------------------
		//
		inline bool IsBlockOn( void )
		{
			return( m_BlockOn );
		}

		void CheckEEPROM( void );
		void Init( void );
		bool IsValidLNCVAddress( uint16_t Adresse );
		uint16_t ReadLNCV( uint16_t Adresse );
		void WriteLNCV( uint16_t Adresse, uint16_t Value );
		void SetBlockOn( bool state );
};


//==========================================================================
//
//		E X T E R N   G L O B A L   V A R I A B L E S
//
//==========================================================================

extern LncvStorageClass		g_clLncvStorage;
