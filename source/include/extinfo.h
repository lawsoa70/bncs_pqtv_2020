/*++

Copyright (c) 2000-2017 Atos

Module Name:

    extinfo.h

Abstract:

    Class definitions for external infodriver hook

Author:

    Tim Alden (tim.alden@bbc.co.uk)   8-Jan-2001

Revision History:

  v1.00			9th January 2001
  v1.01			1st February 2001
  v1.02			5th July 2001		Incorporated multi host functionality
  v1.03			16th July 2001		Now using BNCSDEF.H common definitions header
									hInstEX is now class global private member
  v1.04			22nd October 2001	Made counters unsigned, and their support functions

  v1.07			15th July 2002		Added CSI database get/set functions. No event-based
									database change notifications are possible due to this class not 
									registering as CSI client!

  v1.08			28th November 2002	changed the update slot from a void return to bool.
									returns whether or not an update was done

  v1.09			8th April 2003		TX/RX behaviour check
  V4.1			09/09/2004			Rebuild/reversion for Synergy
  v4.1.2		23/10/2008			Minor rework for compatibiliity with VS9. RK.
  v4.1.3		6/12/2010			Put Debug in as in interface. RK.
  v4.1.4		8/3/2013			adds in closeInfodriver function and option
  v4.2.0		16/2/2015			makes get mode return internal status value rather than going to infodriver (we should already know this information)
									makes BBC_SLOTMSG asynchronous (defers for later processing)
									adds static version string
  v4.3.0		21/07/16			reworks static version string, adds build url
  v4.4.0		07/11/16			uses handleshare DLL to do away with HWND_BROADCAST
*/

#if !defined(AFX_EXTINFO_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_EXTINFO_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#define __EXTINFO_VERSION__ "4.4"

#include <bncsdef.h>

#ifndef GTHUNKS_EXPORTS
#undef GTHUNKS_API
#define GTHUNKS_API
#endif

// Would have called this "interface" but although it compiles here it
// doesn't for users.  Probably needs <windows> or similar.
// So use "class".
class IBncsDebug
{
	public:
		virtual void Debug( const char * szFmt, ...) =0;
};

class GTHUNKS_API extinfo
{
public:
	extinfo(UINT i=420);
	~extinfo();

	int connect(UINT iExtDev,HINSTANCE hProgInst=NULL,UINT iThisDev=0,UINT iOffset=0,UINT iMin=0,UINT iMax=0);
	void notify(void(*)(extinfo*,UINT,LPCSTR));

	void setslot(int, LPCSTR, ...);
	bool updateslot(int, LPCSTR, ...);
	void getslot(int, LPSTR );
	void sendslots(int, int);

	void setcounters(PULONG lpTX,PULONG lpRX);
	void incrx();

	int setmode(int iMode);
	int getmode( bool notCached = false );

	void getdbname(WORD wDevice, WORD wDatabase, WORD wIndex, LPSTR szName, int iMaxSize);
	int getdbindex(WORD wDevice, WORD wDatabase, LPCSTR szName);
	void setdbname(WORD device, WORD database, WORD index, LPCSTR name, BOOL fPoll);

	void closeInfodriver( void );

	void closeInfodriverOnShutdown( bool b );

	void (*func)(extinfo*,UINT,LPCSTR);

	void setDebug( IBncsDebug * pIBncsDebug );
	
	UINT iDevice;
	HWND hWndInfo;
	char netmsg[300];
	int iStatus;
	int requestmode;
	UINT iMinRange;
	UINT iMaxRange;
	UINT iOffset;
	BOOL fCalledEx;
	UINT iThisDev;
	HWND hWndCSI_Cl;

	bool m_closeInfodriverOnShutdown;

private:
	HWND hWndSpawn;
	PULONG txcount;
	PULONG rxcount;
	ULONG deftx,defrx;
	HINSTANCE hInstEx;
	int m_txrxState;

	IBncsDebug * m_pIBncsDebug;
	
	friend long far PASCAL InfoWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

#endif