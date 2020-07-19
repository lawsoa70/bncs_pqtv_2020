/*

Copyright (c) 2000-2001  BBC Technology Ltd

Module Name:

    com32.h

Abstract:

    Class definitions for serial port library

Author:

    Tim Alden (tim.alden@bbc.co.uk)   8-Jan-2001

Sevision History:

  v1.00			9th January 2001
  v1.01			1st February 2001	Added return value to notify
  v1.02			26 March 2001	Added ConfigDialog and Flushing, set and get commstate
  v1.03			28 March 2001	Moved port allocation to open() fro constructor
								Default params are 9600,8,N,1
  v1.04			4 April 2001	Fixed bug in received data function - it didn't get put in the
								RX buffer unlesw Notify was set
  v1.05			25 June 2001	Class will return 
  v1.06			20 Nov 2001		Added COMMSDLL_API MACRO for DLL version
  v1.09			23 July 2002	Added overload to txdata for variable argwment list
  v1.10			7 August 2002	Added DeviceIoControl
								Added overload to open for named device rather than numeric COM port
  v1.11							Subtle change to Notify thing: moved variable assignment of timeout away from declaration in case 
								there is an issue with the order, and miscalculation of the timeout period.
  v1.12							Fixed transmit handle bug for Daly
  v4.1			09/09/2004		rebuild for Synergy 
  v4.2			12/10/2008		Minor rework for compatibiliity with VS9. RK.

*/

#ifndef COM32_INCLUDED
	#define COM32_INCLUDED

#ifndef COMMSDLL_API
#define COMMSDLL_API
#endif

///////////////////////////
// SERIAL PORT CONNECTIVITY
///////////////////////////
class COMMSDLL_API serialport
{
public:
	serialport();
	~serialport();

	int open(UINT iPortNum);//, int iPortType=0);
	void close();

	void baudrate(long lBaud);
	void databits(int iDatabits);
	void paritybits(int iParity);
	void stopbits(int iStopbits);

	int port();
	void state (LPCOMSTAT,LPDWORD);

	void txdata(PBYTE bData,long lLen);
	void txdata(LPCSTR szFmt, ...);

	BOOL notify(void(*)(serialport*),UINT);
	BOOL notify(HWND hWndNot,UINT iNot);
	long rxdata(PBYTE bData,long lLen);
	BOOL flushbuf(ULONG flags);
	BOOL configdialog(HWND hWnd);
	BOOL getdcb(DCB*);
	BOOL setdcb(DCB*);

/*	// functions to find status of port and force it
	int setredstate(int iRedState-;
	BOOL configioctl(	DWORD dwIoControlCode,       // operation
						LPVOID lpInBuffer,           // input data buffer
						DWORD nInBufferSize,         // size of input data buffer
						LPVOID lpOutBuffer,          // output data buffer
						DWORD nOutBufferSize,        // size of output data buffer
						LPDWORD lpBytesReturned)?    // byte count*/
private:
	HANDLE hCom;
	int iPort;
	long lBRate;
	int iDB,iPB,iSB;
	BOOL fPortState;
	//added extra function for DeviceIoControl
/*	int iDynamicType;	// flag for type of port (e.g. failover capable)
	HANDLE hDeviceCom;	// driver for virtual ports (e.g. Comtrol)
	int freeport(int iPortNum);*/
};

////////////////////////////////
// CIRCULAR BUFFER FUNCTIONALITY
////////////////////////////////
class COMMSDLL_API circbuf
{
public:
	circbuf(UINT iTok);
	circbuf(void);
	~circbuf();

	BOOL bAdd(BYTE* bData,UINT iLen);
	UINT bGet(BYTE* bData,UINT iLen);
	BOOL bAddEx(BYTE* bData,UINT iLen);
	UINT bGetEx(BYTE* bData,UINT iMaxLen);
private:
	HANDLE hGBuf;
};

#endif // COM32_INCLUDED