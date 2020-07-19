/*************************************/
/* Written by David Yates            */
/* Copyright Atos 2007 */
/*************************************/
#if !defined(AFX_CCINFODRIVER_H__0EF3B407_6B86_49D2_92FF_9737960379E3__INCLUDED_)
#define AFX_CCINFODRIVER_H__0EF3B407_6B86_49D2_92FF_9737960379E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <string>
#include <queue>
using namespace std;

#include "ccCheck.h"
#include "cc.h"

/** \class IccInfodriverCallback
\brief Callback interface class for ccInfodriver class

For clarity because this class must be inherited, the function names are prepended with cci (for Colledia Control Infodriver)
\sa ccInfodriver
*/
class IccInfodriverCallback
{
	friend class ccInfodriver;
protected:
	/** Request from client to change a slot
	\param[in] device The device number
	\param[in] index The slot number
	\param[in] slot The requested contents to use
	\note This command does not automatically update the contents of the slot
	*/
	virtual void cciSlotRequest( int device, int index, const string & slot ){};
	/** Direct message from a client requesting workstation specific information on this device
	\param[in] workstation The workstation requesting the information
	\param[in] device The device number
	\param[in] message The message string from the client
	\param[in] reference An arbitary message reference that should be returned with the reply to this command
	\sa ccInfodriver::clientMessage();
	\note Implementation of this messaging mechanism is patchy so beware....
	*/
	virtual void cciClientMessage( int workstation, int device, const string & message, const string & clientHandle, const string & reference ){};
	/** Notification of redundancy state
	\param[in] state The current state
	*/
	virtual void cciRedundancyState( int device, enum cc::redundancyState state ){};
	/** Connected event */
	virtual void cciConnected( int ){};
	/** Disconnected event */
	virtual void cciDisconnected( int ){};
	
};

class ccInfodriverPost
{
public:
	ccInfodriverPost( int type, int slot, const string & contents )
	{
		m_type = type;
		m_slot = slot;
		m_contents = contents;
	};
	int m_type;
	int m_slot;
	string m_contents;
};

class ccInfodriver : public ccCheck
{
public:
	ccInfodriver( IccInfodriverCallback * callback = 0, const string & debugStringHeader = "ccInfodriver"  );
	virtual ~ccInfodriver();

	long version( void );

	bool connect( int device, enum cc::ipcType ipc = cc::bbc, long ipcOptions=0 );
	bool disconnect();
	bool isConnected( void );

	bool setSlotOnly( int index, const string & value );

	bool setSlot( int index, const string & value, bool sendNow = false );
	bool setSlot( int index, int value, const string & format = "%d", bool sendNow = false );
	bool setSlot( int index, double value, const string & format = "%f", bool sendNow = false );

	bool updateSlot( int index, const string & value, bool & updated );
	bool updateSlot( int index, int value, const string & format = "%d" );
	bool updateSlot( int index, double value, const string & format = "%f" );
	bool sendSlotRange( int start, int end );
	bool slot( int index, string & value );

	void clientMessage( int workstation, int device, const string & message, const string & client, const string & reference );

	void setRedundancyState( enum cc::redundancyState state, bool force = true );
	void setDriverState( enum cc::driverState state, bool doNow=false, bool force = true );
	enum cc::redundancyState redundancyState( void );
	int setRedundancyMaster(int device);
	void setCloseHostOnExit( bool close = false );
	bool counters( unsigned int & tx, unsigned int & rx );
	void resetCounters( void );
	void sendBufferedRevertives( void );

	unsigned int maxDevices(void);
	unsigned int maxIndexes(void);
	unsigned int maxSources(void);
	unsigned int maxDatabases(void);
	unsigned int maxString(void);

private:
	IccInfodriverCallback * m_callback;
	HWND m_hInfo;
	HWND m_hWndMessage;
	enum cc::ipcType m_ipc;
	long m_ipcOptions;
	int m_workstation;
	int m_device;
	bool m_closeHostOnExit;
	unsigned int m_txCounter;
	unsigned int m_rxCounter;
	unsigned int m_lastTxCounter;
	unsigned int m_lastRxCounter;

	enum cc::redundancyState m_redundancyState;
	enum cc::driverState m_driverState;

	queue< ccInfodriverPost > postedQueue;
	static LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	LRESULT BNCSSendMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, int dataLen, int expectedReturnDataLen, int iDataType );
};

#endif // !defined(AFX_CCINFODRIVER_H__0EF3B407_6B86_49D2_92FF_9737960379E3__INCLUDED_)
