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

#include "ccCheck.h"
#include "cc.h"

/** \class IccExtControlCallback
\brief Callback interface class for ccExtControl class

For clarity because this class must be inherited, the function names are prepended with cci (for Colledia Control Infodriver)
\sa ccExtControl
*/
class IccExtControlCallback
{
	friend class ccExtControl;
protected:
	/** Connected event */
	virtual void cceConnected( int device ){};
	/** Disconnected event */
	virtual void cceDisconnected( int device ){};

	/** Called when a value changes on the external 
	\param device BNCS device number
	\param index slot/dest on that device
	\param info integer value (or conversion of string value)
	\param sInfo string value 
	*/
	virtual void cceExtHostRevertive(int device, int index, int info, std::string sInfo){};

	/** Called when an incoming command is received. You can choose whether to let that command carry on, or stop it.
	\param device BNCS device number
	\param index slot/dest on that device
	\param info integer value (or conversion of string value)
	\param sInfo string value
	\param workstation number of the workstation generating the command
	\returns true to intercept the command, false for normal processing
	*/
	virtual bool cceCommand(int device, int index, int info, std::string sInfo, int workstation){ return true; };

	//	redundancy
	virtual void cceRedundancyState(int device, enum cc::redundancyState state){};
};

class ccExtControlPost
{
public:
	// lParam is string
	ccExtControlPost( UINT msg, WPARAM wParam, const string & contents )
	{
		m_msg = msg;
		m_wParam = wParam;
		m_lParam = 0;
		m_contents = contents;
	};
	// lParam is numeric
	ccExtControlPost(UINT msg, WPARAM wParam, LPARAM lParam)
	{
		m_msg = msg;
		m_wParam = wParam;
		m_lParam = lParam;
	};
	UINT m_msg;
	int m_wParam;
	int m_lParam;
	string m_contents;
};

class ccExtControl : public ccCheck
{
public:
	ccExtControl( IccExtControlCallback * callback = 0, const string & debugStringHeader = "ccExtControl"  );
	virtual ~ccExtControl();

	long version( void );

	bool connect( char type, int device, enum cc::ipcType ipc = cc::bbc, long ipcOptions=0 );
	bool disconnect();
	bool isConnected( void );
	void resetCounters(void);
	bool counters(unsigned int & tx, unsigned int & rx);


	void routerCrosspoint(int device, int index, int info, const string & mask, bool now, int workstationOverride);
	void infoWrite(int device, int index, const string & value, bool now, int workstationOverride);
	void gpiSwitch(int device, int index, bool state, bool now, int workstationOverride);
	
	void getData(int index, int & info, std::string & sInfo);

	enum cc::redundancyState redundancyState(void);
	void setRedundancyState(enum cc::redundancyState state, bool force = true);
private:
	void sendRaw(const char * cmd);

	IccExtControlCallback * m_callback;
	HWND m_hExternal;
	HWND m_hWndMessage;
	enum cc::ipcType m_ipc;
	long m_ipcOptions;
	int m_workstation;
	int m_device;

	unsigned int m_txCounter;
	unsigned int m_rxCounter;
	unsigned int m_lastTxCounter;
	unsigned int m_lastRxCounter;

	enum cc::redundancyState m_redundancyState;
	std::queue< ccExtControlPost > postedQueue;
	static LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	LRESULT BNCSSendMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, int dataLen, int expectedReturnDataLen, int iDataType );
};

#endif // !defined(AFX_CCINFODRIVER_H__0EF3B407_6B86_49D2_92FF_9737960379E3__INCLUDED_)


/*
XC	eXternal Crosspoint	Similar to 'Router Crosspoint'
XS	eXternal Switch	Similar to ‘GPI Switch’
XW	eXternal Write	Similar to 'Infodriver Write'
XR	eXternal Register	Similar to the three commands 'Router Register', 'GPI Register' and 'Infodriver Register'
XU	eXternal Unregister	Similar to the three commands 'Router Unregister', 'GPI Unregister' and 'Infodriver Unregister'

XD	eXternal Data	Retrieves data as a result of an intercept being performed
XG	eXternal Get	Direct retrieval of source, IO or slot information
XI	eXternal Intercept	Used to specify a stringtable offset where the panel will be notified that it has intercepted a command to the driver. Retrieve the data with the XD command.
XN	eXternal redundancy Notify	Used to specify a stringtable offset where the panel will be notified that the redundancy status of a driver has changed. Use DM command to retrieve status.
XP	eXternal Poll	Similar to the three commands 'Router Poll', 'GPI Poll' and 'Infodriver Poll'
*/