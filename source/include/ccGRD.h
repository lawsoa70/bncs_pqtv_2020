/*************************************/
/* Written by David Yates            */
/* Copyright Siemens IT Systems 2007 */
/*************************************/
#if !defined(AFX_ccGrd_H__959C69A9_22CC_4486_8F52_4687AE0071A8__INCLUDED_)
#define AFX_ccGrd_H__959C69A9_22CC_4486_8F52_4687AE0071A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include "ccCheck.h"
#ifdef USEINDEXMAPPING
#include "indexMapping.h"
#endif
#include "cc.h"

#pragma warning( disable : 4786) 

#include <queue>
using namespace std;

/** \class IccGrdCallback
\brief Callback interface class for ccGrd class

For clarity because this class must be inherited, the function names are prepended with ccg (for Colledia Control Grd)
\sa ccGrd
*/
class IccGrdCallback
{
public:
	/** Notification that we've connected to this device (also available as return value from connect()*/
	virtual void ccgConnected( int )=0;
	/** Notification that we've disconnected - this may be received as a resuilt of the host application closing */
	virtual void ccgDisconnected( int )=0;
	/** Simple notification of crosspoint request - note that the mask parameter is not available 
	\param[in] dest The destination number
	\param[in] source The source number
	*/
	virtual void ccgCrosspointRequest( int device, int dest, int source ){};
	/** Full notification of crosspoint request
	\param[in] dest The destination number
	\param[in] source The source number
	\param[in] mask The "metadata" of how to make this route
	\param[in] workstation The workstation requesting the route
	*/
	virtual void ccgCrosspointRequest( int device, int dest, int source, const string & mask, int workstation ){};
	/** Direct message from a client requesting workstation specific information on this device
	\param[in] workstation The workstation requesting the information
	\param[in] device The device number
	\param[in] message The message string from the client
	\param[in] reference An arbitary message reference that should be returned with the reply to this command
	\sa ccInfodriver::clientMessage();
	\note Implementation of this messaging mechanism is patchy so beware....
	*/
	virtual void ccgClientMessage( int workstation, int device, const string & message, const string & clientHandle, const string & reference ){};
	/** Notification of redundancy mode
	\param[in] mode The current mode
	*/
	virtual void ccgRedundancyState( enum cc::redundancyState state ){};
};

class ccGrdPost
{
public:
	ccGrdPost( int type, int index, int info, const string & str )
	{
		m_type = type;
		m_index = index;
		m_info = info;
		m_str = str;
	};
	int m_type;
	int m_index;
	int m_info;
	string m_str;
};

class ccGrd : ccCheck
{
public:
	ccGrd( IccGrdCallback * callback, const string & debugHeader = "ccGrd" );
	virtual ~ccGrd();

	void setTally( int dest, int source, bool send=true );
	bool updateTally( int dest, int source );
	bool connect( int device );
	bool disconnect( void );
	bool isConnected( void );
	int tally( int dest );
	int size( int database );

	void setRedundancyState( enum cc::redundancyState state );
	void setDriverState( enum cc::driverState state, bool forceNow=false );
	enum cc::redundancyState redundancyState( void );
	void clientMessage( int workstation, int device, const string & message, const string & client, const string & reference );

	void setCloseHostOnExit( bool close = false );
	bool counters( unsigned int & tx, unsigned int & rx );
	void resetCounters( void );
	void loadIndexMapping( void );
private:
	IccGrdCallback *m_callback;
	HWND m_hWndMessage;
	HWND m_hGRD;
	int m_device;
	queue< ccGrdPost > postedQueue;
	int m_workstation;
	static LRESULT CALLBACK WndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	enum cc::redundancyState m_redundancyState;
	enum cc::driverState m_driverState;

	bool m_closeHostOnExit;
	unsigned int m_txCounter;
	unsigned int m_rxCounter;
	unsigned int m_lastTxCounter;
	unsigned int m_lastRxCounter;
	LRESULT BNCSSendMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, int dataLen, int expectedReturnDataLen, int iDataType );
#ifdef USEINDEXMAPPING
	indexMapping *m_indexMapping;
#endif
};

#endif // !defined(AFX_ccGrd_H__959C69A9_22CC_4486_8F52_4687AE0071A8__INCLUDED_)
