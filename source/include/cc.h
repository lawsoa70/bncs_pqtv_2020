/*************************************/
/* Written by David Yates            */
/* Copyright Atos 2007 */
/*************************************/
#if !defined(AFX_CC_H__2F6043C9_824C_427B_A99E_BEB42E154EE6__INCLUDED_)
#define AFX_CC_H__2F6043C9_824C_427B_A99E_BEB42E154EE6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bncs.h"

namespace cc
{
	enum ipcType
	{
		none=0,
		wm,
		bbc
	};

	enum redundancyState
	{
		unknown = IFMODE_NONE,
		rx = IFMODE_RXONLY, 
		tx = IFMODE_TXRX,
		broken = IFMODE_RXONLYBROKEN,
		forceTx = IFMODE_FORCETXRX
	};

	enum redundancyMode
	{
		singleControllerCapable = 1,
		multipleControllerCapable
	};

	enum driverState
	{
		ok = 1,
		fail
	};

	enum pollType
	{
		UnknownType = 0,
		IP,
		RP,
		GP
	};
};

#ifndef BBC_IDSLOTMSGPOSTED
	#define BBC_IDSLOTMSGPOSTED	WM_APP + 5001
#endif

#ifndef BBC_DEVCOMMANDMSGPOSTED
	#define BBC_DEVCOMMANDMSGPOSTED	WM_APP + 5002
#endif

#ifndef BBC_DRIVERMSGPOSTED
	#define BBC_DRIVERMSGPOSTED	WM_APP + 5003
#endif

#endif // !defined(AFX_CC_H__2F6043C9_824C_427B_A99E_BEB42E154EE6__INCLUDED_)
