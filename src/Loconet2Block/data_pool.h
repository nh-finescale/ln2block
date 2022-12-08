
#pragma once

//##########################################################################
//#
//#		DataPoolClass
//#
//#	This class administers and processes all data, especially the guards
//#	for the state machines.
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	5		from: 08.12.2022
//#
//#	Implementation:
//#		-	add check to re-send 'Erlaubnisabgabe'
//#			changed function
//#				InterpretData() will deliver different return values now
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	4		from: 09.09.2022
//#
//#	Implementation:
//#		-	the flag for train numbers moved to 'lncv_storage',
//#			because it will be stored permanent now.
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	3		from: 27.08.2022
//#
//#	Implementation:
//#		-	add a function to send the state of all OUT-LocoNet-Devices
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	2		from: 04.03.2022
//#
//#	Implementation:
//#		-	add flag that shows if handling of train number messages
//#			is enabled
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	1		from: 25.02.2022
//#
//#	Implementation:
//#		-	add new message handling for FREMO train numbers
//#
//##########################################################################


//==========================================================================
//
//		I N C L U D E S
//
//==========================================================================

#include <stdint.h>

#include "lncv_storage.h"


//==========================================================================
//
//		D E F I N I T I O N S
//
//==========================================================================

//----	Definitionen für 'm_uiLocoNetIn'  --------------------------
//
#define DP_E_SIG_SEND		(IN_IDX_KEY_RELEASED + 1)
#define DP_A_SIG_SEND		(DP_E_SIG_SEND + 1)

//----	Definitionen für 'm_uiBlockIn'  ----------------------------
//
#define DP_BLOCK_MESSAGE_VORBLOCK				0
#define DP_BLOCK_MESSAGE_RUECKBLOCK				1
#define DP_BLOCK_MESSAGE_ERLAUBNIS_ABGABE		2
#define DP_BLOCK_MESSAGE_ERLAUBNIS_ANFRAGE		3

#define DP_BLOCK_MESSAGE_VORBLOCK_ACK			4
#define DP_BLOCK_MESSAGE_RUECKBLOCK_ACK			5
#define DP_BLOCK_MESSAGE_ERLAUBNIS_ABGABE_ACK	6
#define DP_BLOCK_MESSAGE_ERLAUBNIS_ANFRAGE_ACK	7

//----	Rückmeldungen für Funktion InterpretData()  ----------------
//
#define DO_NOTHING			0
#define DO_RESET			1
#define DO_DISCONNECTED		2
#define DO_RECONNECTED		3


//=====================================================================
//
//		K O N S T A N T E
//
//=====================================================================

extern const uint32_t	cg_ulInterval_500_ms;
extern const uint32_t	cg_ulInterval_1_s;
extern const uint32_t	cg_ulInterval_2_s;
extern const uint32_t	cg_ulInterval_5_s;
extern const uint32_t	cg_ulInterval_10_s;


//==========================================================================
//
//		C L A S S   D E F I N I T I O N S
//
//==========================================================================


///////////////////////////////////////////////////////////////////////
//	CLASS:	DataPoolClass
//
//	This class administers and processes all data, especially the guards
//	for the state machines.
//
class DataPoolClass
{
	private:
		uint8_t		m_arusTrainNoStation2Block[ 30 ];
		uint8_t		m_arusTrainNoBlock2Station[ 22 ];
		uint8_t		m_usTrainNoStation2BlockLen;
		uint16_t	m_uiConfig;
		uint16_t	m_uiLocoNetIn;
		uint32_t	m_ulLocoNetOut;
		uint32_t	m_ulLocoNetOutPrevious;
		uint8_t		m_uiBlockIn;
		uint8_t		m_uiBlockOut;
		uint32_t	m_ulMillisProgMode;
		uint32_t	m_ulMillisBlockDetect;
		uint32_t	m_ulMillisReadInputs;
		uint32_t	m_ulMillisMelder;
		uint32_t	m_ulMillisContact;
		uint8_t		m_uiMelderCount;
		bool		m_bInternalContactSet;

	public:
		DataPoolClass();

		void	Init( void );
		uint8_t	InterpretData( void );
		void	StartMelder( void );
		void	SetProgMode( bool on );
		void	SwitchBlockOff( void );
		void	CheckForOutMessages( void );
		bool	CheckIfConfigChanged( void );
		void	ReceiveTrainNoFromBlock( uint8_t *pusData );
		void	ReceiveTrainNoFromStation( uint8_t *pusData );

		inline bool IsProgMode( void )
		{
			return( 0 < m_ulMillisProgMode );
		};

		inline uint8_t *GetStation2Block( void )
		{
			return( m_arusTrainNoStation2Block );
		};

		inline uint8_t GetTrainNoStation2BlockLen( void )
		{
			return( m_usTrainNoStation2BlockLen );
		};

		inline bool IsNewMsgStation2Block( void )
		{
			return( 0x00 != m_arusTrainNoStation2Block[ 0 ] );
		};

		inline bool IsNewMsgBlock2Station( void )
		{
			return( 0x00 != m_arusTrainNoBlock2Station[ 0 ] );
		};


	//=================================================================
	//	LocoNet IN Nachrichten
	//=================================================================

		//---------------------------------------------------------
		//	Die Funktion setzt das/die angegebene(n) Flag(s) um
		//	den Zustand der entsprechenden IN-LocoNet-Devices
		//	zu speichern
		//
		inline void SetInState( uint16_t flags )
		{
			m_uiLocoNetIn |= flags;
		}

		//---------------------------------------------------------
		//	Die Funktion löscht das/die angegebene(n) Flag(s) um
		//	den Zustand der entsprechenden IN-LocoNet-Devices
		//	zu speichern
		//
		inline void ClearInState( uint16_t flags )
		{
			m_uiLocoNetIn &= ~flags;
		}

		//---------------------------------------------------------
		//	Die Funktion liefert 'true' zurück,
		//	wenn eines der angegebenen Flags gesetzt ist.
		//
		inline bool IsOneInStateSet( uint16_t flags )
		{
			return( 0 != (m_uiLocoNetIn & flags) );
		}

		//---------------------------------------------------------
		//	Die Funktion liefert 'true' zurück,
		//	wenn alle angegebenen Flaggen gesetzt sind.
		//
		inline bool AreAllInStateSet( uint16_t flags )
		{
			return( 0 == ~(m_uiLocoNetIn | ~flags) );
		}

		//---------------------------------------------------------
		//	Die Funktion liefert 'true' zurück,
		//	wenn das angegebene Flag gesetzt ist.
		//	Anschließend wird das Flag gelöscht.
		//
		bool IsInStateSetAndClear( uint16_t flag );


	//=================================================================
	//	LocoNet OUT Nachrichten
	//=================================================================

		//---------------------------------------------------------
		//	Die Funktion liefert den Zustand der
		//	OUT-LocoNet-Devices
		//
		inline uint32_t GetLocoNetOut( void )
		{
			return( m_ulLocoNetOut );
		}

		//---------------------------------------------------------
		//	Die Funktion setzt das/die angegebene(n) Flag(s) um
		//	den Zustand der entsprechenden OUT-LocoNet-Devices
		//	zu speichern
		//
		inline void SetOutState( uint32_t flags )
		{
			m_ulLocoNetOut |= flags;
		}

		//---------------------------------------------------------
		//	Die Funktion löscht das/die angegebene(n) Flag(s) um
		//	den Zustand der entsprechenden OUT-LocoNet-Devices
		//	zu speichern
		//
		inline void ClearOutState( uint32_t flags )
		{
			m_ulLocoNetOut &= ~flags;
		}

		//---------------------------------------------------------
		//	Die Funktion liefert 'true' zurück,
		//	wenn eines der angegebenen Flags gesetzt ist.
		//
		inline bool IsOneOutStateSet( uint32_t flags )
		{
			return( 0 != (m_ulLocoNetOut & flags) );
		}

		
		//---------------------------------------------------------
		//	Die Funktion setzt das/die angegebene(n) Flag(s) um
		//	den aktuellen Zustand der entsprechenden OUT-LocoNet-
		//	Devices erneut zu senden.
		//
		inline void SetOutStatePrevious( uint32_t flags )
		{
			m_ulLocoNetOutPrevious |= flags;
		}

		//---------------------------------------------------------
		//	Die Funktion löscht das/die angegebene(n) Flag(s) um
		//	den aktuellen Zustand der entsprechenden OUT-LocoNet-
		//	Devices erneut zu senden.
		//
		inline void ClearOutStatePrevious( uint32_t flags )
		{
			m_ulLocoNetOutPrevious &= ~flags;
		}

		//---------------------------------------------------------
		//	Die Funktion sorgt dafür, dass der aktuelle Zustand
		//	aller OUT-LocoNet-Devices gesendet wird
		//
		void SendOutState( void );


	//=================================================================
	//	Block-Nachrichten
	//=================================================================

		//---------------------------------------------------------
		//	Die Funktion setzt das angegebene Flag um den Empfang
		//	einer Block-Nachricht zu melden.
		//
		inline void SetBlockMessageState( uint8_t flags )
		{
			m_uiBlockIn |= flags;
		}

		//---------------------------------------------------------
		//	Die Funktion löscht das angegebene Flag einer
		//	Block-Nachricht
		//
		inline void ClearBlockMessageState( uint8_t flags )
		{
			m_uiBlockIn &= ~flags;
		}

		//---------------------------------------------------------
		//	Die Funktion liefert 'true' zurück, wenn das
		//	angegebene Flag für eine Block-Nachricht gesetzt ist.
		//	Nach der Abfrage gilt die Nachricht als 'verbraucht'
		//	und wird 'gelöscht', d.h.: das Flag wird gelöscht.
		//
		bool IsBlockMessageEmpfangen( uint8_t flag );


		//---------------------------------------------------------
		//	Die Funktion liefert den Zustand der zu sendenden
		//	Block-Nachrichten.
		//
		inline uint8_t GetSendBlockMessage( void )
		{
			return( m_uiBlockOut );
		}
		
		//---------------------------------------------------------
		//	Die Funktion setzt das angegebene Flag um das Senden
		//	einer Block-Nachricht anzumelden.
		//
		inline void SetSendBlockMessage( uint8_t flags )
		{
			m_uiBlockOut |= flags;
		}

		//---------------------------------------------------------
		//	Die Funktion löscht das angegebene Flag um das Senden
		//	einer Block-Nachricht zu verhindern.
		//
		inline void ClearSendBlockMessage( uint8_t flags )
		{
			m_uiBlockOut &= ~flags;
		}

		//---------------------------------------------------------
		//	Die Funktion liefert 'true' zurück, wenn das
		//	angegebene Flag für eine Block-Nachricht gesetzt ist.
		//	Nach der Abfrage gilt die Nachricht als gesendet,
		//	d.h.: das Flag wird gelöscht.
		//
		bool IsSendBlockMessage( uint8_t flag );


	//=================================================================
	//	Wächter
	//=================================================================

		//---------------------------------------------------------
		//	Der Wächter, ob die Erlaubnis abgegeben werden darf.
		//
		bool DarfErlaubnisAbgeben( void );

		//---------------------------------------------------------
		//	Der Wächter, ob der Hilfsvorblock bedient werden darf.
		//
		bool DarfHilfsvorblockSetzen( void );
};


//==========================================================================
//
//		E X T E R N   G L O B A L   V A R I A B L E S
//
//==========================================================================

extern DataPoolClass	g_clDataPool;
