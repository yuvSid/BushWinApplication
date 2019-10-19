#pragma once
#include "SerialPortBush.h"
#include "BushData.h"

const WORD M_WAIT_TIME_DEFAULT = 1000;
const WORD M_WAIT_TIME_UPDATE = 5000;

enum SCRIPT_STEP
{
	FIRST_STEP,
	SECOND_STEP,
	THIRD_STEP
};

class BushInOutInterpretator : public SerialPortBush
{
private:
	enum EVENT_ARR {
		COMMAND,
		BUSH_INPUT,
		EV_COUNT
	};
		
	BushData* m_pDataITC;// pointer to class with inter-thread communication
	HANDLE m_haEvHandler[EV_COUNT];
	BUSH_SCRIPT m_script;// stores name of operation
	BYTE m_waitForOpcode;// stores current wait for operation, need in timeout error processing
	DWORD m_dwWaitTime;// timer before timeout error start processing
	BOOL m_bRepeatErr;// flag of repeatable timeout error

	

private:
	VOID fnSecureClassMemory() {
		m_pDataITC = nullptr;
		SecureZeroMemory( m_haEvHandler, sizeof( HANDLE )*EV_COUNT );
		m_waitForOpcode = OPCODE::NOT_VALUE;
		m_script = BUSH_SCRIPT::NO_SCRIPT;
		m_dwWaitTime = INFINITE;
		m_bRepeatErr = FALSE;

	}

public:
	BushInOutInterpretator( BushData* pDataToITC, const TCHAR* pcBushPortName ) : SerialPortBush( pcBushPortName ) {
		fnSecureClassMemory();
		m_pDataITC = pDataToITC;
		m_haEvHandler[EVENT_ARR::COMMAND] = m_pDataITC->GetCommandEvent();
	}
	~BushInOutInterpretator() { 
		for ( int i = 0; i < EV_COUNT; i++ )
			CloseHandle( m_haEvHandler[i] );
		m_pDataITC = nullptr;
	}

	DWORD fnStart();

	DWORD fnFinish();

	BOOL fnWaitForNextIO();


private:
	DWORD fnDefaultWait();
	DWORD fnConnectCheck();
	DWORD fnAskStateInit();
	DWORD fnAskState();
	DWORD fnGetHeatSens();
	DWORD fnLockRelayScript( SCRIPT_STEP wStep, BOOL bOnMech, BOOL bIsLockScript );

	DWORD fnInputBushHandle();
	DWORD fnTimerWaitHandle();
};



DWORD WINAPI fnFromBushThread( LPVOID lpParam );
typedef struct InThreadData
{
	const TCHAR* pPortName;
	BushData* pBushData;
} INTHREADDATA, *LPINTHREADDATA;

DWORD WINAPI fnMainIOBushThread( LPVOID lpParam );
