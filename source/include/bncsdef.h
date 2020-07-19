///////////////////////////////////////////////////////////
//
//	BNCSDEF.H		CONSTANTS FOR BNCS CLASS LIBRARIES
//
//	Written by Tim Alden
//
// (C) 2001 BBC Technology Ltd
//	v4.1	09/09/2004	first versioned release
//
///////////////////////////////////////////////////////////

#if !defined(AFX_BNCSDEF_H__A9DBDB833_L6FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_BNCSDEF_H__A9DBDB833_L6FD_11D0_BFD1_444553540000__INCLUDED_


///////////////////////////////////////////////////////////
// serial driver specific
#define	COMM_NORMAL					0
#define COMM_ROCKET_DYNAMIC			1
#define COMM_ROCKET_FORCE			2

///////////////////////////////////////////////////////////
// external infodriver class specific

#define CANT_FIND_INFODRIVER	5
#define	SLOT_CHANGE				15

///////////////////////////////////////////////////////////
// external grd class specific

#define CANT_FIND_GRD			5
#define XPT_CHANGE				15
#define XPT_LOCK				16

///////////////////////////////////////////////////////////
// general class defines - used by CSIdrv CSIclient etc

#define ERROR_GENERAL			0xFFFF
#define ERROR_WRONG_TYPE		0xFFFE
#define ERROR_BAD_PARAMLIST		0xFFFD


#define COMMAND					0
#define STATUS					1
#define RXONLY					2
#define DISCONNECTED			3
#define INVALID_DRIVERNUM		4
#define CANT_FIND_CSI			5
#define TO_TXRX					6
#define TO_RXONLY				7
#define QUERY_TXRX				8
#define CANT_REGISTER_CLASSWND	9
#define DRIVERNUM_ALREADY		10
#define CSI_NOT_ENABLED			11
#define DATABASECHANGE			12
#define BAD_WS					13
#define CSI_WRONG_TYPE			14
#define CONNECTED				15

#define DRVTYPE_NONE			0xF000
#define REVTYPE_R				0x1000
#define DRVTYPE_R				0x1100
#define REVTYPE_G				0x2000
#define DRVTYPE_G				0x2200
#define REVTYPE_I				0x4000
#define DRVTYPE_I				0x4400
#define REVTYPE_D				0x8000
#define DRVTYPE_D				0x8800
#define NOW						true
#define LATER					false
#define INSERT					1
#define	OVERWRITE				0

/********************************************
	BNCS user defined structure declarations
	*****************************************/
#define MAXNAMESIZE 16
#ifdef __CDS
#define __CDS
typedef struct tagCOPYDATASTRUCT {
	 DWORD dwData;
	 DWORD cbData;
	 LPVOID lpData;
} COPYDATASTRUCT, far * LPCOPYDATASTRUCT;
#endif

#ifndef LOCALSTRUCTURES
typedef struct tagREGTYPE
{
WORD wDevice;
WORD wMin;
WORD wMax;
char wDevType[2];
}REGTYPE, far * LPREGTYPE;

typedef struct tagDBASE
{
WORD wRouter;
WORD wSwitch;
WORD wIndex;
}DBASE, far* LPDBASE;

typedef struct tagREVERTIVE
{
char revType[3];
unsigned driver;
unsigned srcIdx;
unsigned destIdx;
char srcName[300];
char destName[MAXNAMESIZE];
char srcName2[MAXNAMESIZE];
char destName2[MAXNAMESIZE];
}REV, far * LPREV;
#endif

   /* CSI Parser return values */
#define ERR_NONE					0
#define ERR_NO_DRIVERTYPE		1
#define ERR_NO_DRIVERNUM		2
#define ERR_NO_PARAM1			4
#define ERR_NO_PARAM2			8
#define ERR_NO_CONTENTS			16
#define ERR_NO_SLOTNUM			32
#define ERR_NO_WS					64
#define ERR_NO_BUFFERFLAG		128


#endif