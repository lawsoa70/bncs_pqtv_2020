/*++

Copyright (c) 2000-2001  BBC Technology Ltd

Module Name:

    cxinfo.h

Abstract:

    C++ Class definitions for infodriver connection

Author:

    Tim Alden (tim.alden@bbc.co.uk)   8-Nov-2003

Revision History:

  v1.00			7 Nov 2003
  
*/

#if !defined(AFX_CXEXTINFO_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_CXEXTINFO_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_


#include <bncsdef.h>

#ifndef GTHUNKS_EXPORTS
#undef GTHUNKS_API
#define GTHUNKS_API
#endif

class GTHUNKS_API cXInfo
{
	HWND hWndSpawn;
	PULONG txcount;
	PULONG rxcount;
	ULONG deftx,defrx;
	HWND hWndInfo;
	UINT iDevice;
	LPSTR netmsg;
	UINT iMinRange;
	UINT iMaxRange;
	UINT iOffset;
	BOOL fCalledEx;
	UINT iThisDev;
	HWND hWndCSI_Cl;
	static LRESULT WINAPI InfoWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
protected:
	virtual BOOL notify(cXInfo* cX,UINT iSlot, LPCSTR szNewContents)=0;
public:
	int iStatus;
	cXInfo(UINT i=3546);
	~cXInfo();
	int connect(UINT iExtDev,UINT iThisDev=0,UINT iOffset=0,UINT iMin=0,UINT iMax=0);
	void setcounters(PULONG lpTX,PULONG lpRX);
	void setslot(int, LPCSTR, ...);
	bool updateslot(int, LPCSTR, ...);
	void getslot(int, LPSTR );
	void sendslots(int, int);
	void incrx();
	int setmode(int iMode);
	int getmode();
	void getdbname(WORD wDevice, WORD wDatabase, WORD wIndex, LPSTR szName, int iMaxSize);
	int getdbindex(WORD wDevice, WORD wDatabase, LPCSTR szName);
	void setdbname(WORD device, WORD database, WORD index, LPCSTR name, BOOL fPoll);
};

#endif