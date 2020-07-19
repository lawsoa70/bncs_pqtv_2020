/****************************************************************************
*			BBC_NB32.DLL :	General Header file for BBC NetBIOS implemenation	 *
*																									 *
*			(c) BBC Manchester 1993-98	:	Author Simon Dowson						 *
*																									 *
* History :																						 *
*																									 *
*				01/10/93		:	Beta release 1.00											 *
*				23/12/93		:	All receives now LocalAlloc()                    *
* 1.00.00	29/12/93		:	NetVersion() added										 *
*									Implementation specific error codes added			 *
*									with bit 7 of NCB return code set					 *
* 1.01.00	07/01/98		:  Allows adaptor to be specified in INI file		 *
* 2.00.00	12/08/98		:  Converted to use Netbios() call for 32 bit dll	 *
* 3.00.00   13/08/01    :  Added NetSetAdaptor() && NetSetAdaptor()         *
****************************************************************************/

#include <direct.h>
#include <dos.h>
#include <stdlib.h>
#include <nb30.h>

#define WM_NETBIOS			  WM_USER+2000
#define NETBIOS_NAME_LENGTH  16     /* Maximum NetBIOS name         */
#define NET_ERROR				  0x80

#define DEBUG_OFF		0
#define DEBUG_ON		1
#define DEBUG_EXTRA	2

#ifdef __cplusplus
extern "C"
{
#endif
// Function prototypes
/****************************************************************************
	 FILE: BBC_NB32.C - Contains Initialisiation Routines
****************************************************************************/
void __declspec(dllexport) FAR PASCAL NetCmdCode(LPSTR, BOOL);
void __declspec(dllexport) FAR PASCAL NetRetCode(LPSTR, BOOL);
void FAR PASCAL NetErrorMsg(LPSTR, BOOL);
UINT __declspec(dllexport) FAR PASCAL NetVersion(void);
VOID __declspec(dllexport) FAR PASCAL NetCopyName (LPSTR, LPSTR);

NCB_POST PostRoutine(PNCB);
void __declspec(dllexport) FAR PASCAL NetDebugMode(BOOL);
void NetODS(char *);

/****************************************************************************
	 FILE: NCBALLOC.C  -  Contains Memory Allocation Routines
****************************************************************************/

PNCB __declspec(dllexport) FAR PASCAL NetAllocNcbAndBuf( DWORD dwDataLen );
VOID __declspec(dllexport) FAR PASCAL	NetNcbFree       ( PNCB  pncbNcbWithBuffer);

/****************************************************************************
    FILE: NCBCALLS.C - Contains routines which submit NCBs for caller.
****************************************************************************/

//Synchronous calls
BYTE __declspec(dllexport) FAR PASCAL NetAddName   	 	(LPSTR, LPBYTE, BYTE);
BYTE __declspec(dllexport) FAR PASCAL NetAddGroupName 	(LPSTR, LPBYTE, BYTE);
BYTE __declspec(dllexport) FAR PASCAL NetDeleteName 	(LPSTR, BYTE, BYTE);
BYTE __declspec(dllexport) FAR PASCAL NetHangUp    	 	(BYTE, BYTE);
BYTE __declspec(dllexport) FAR PASCAL NetCancel    	 	(PNCB, BYTE);
BYTE __declspec(dllexport) FAR PASCAL NetReset			(BYTE, BYTE, BYTE);

//Asynchronous calls
BYTE __declspec(dllexport) FAR PASCAL NetAdaptorStatus (HWND, WORD, LPSTR, PNCB FAR *, BYTE);
BYTE __declspec(dllexport) FAR PASCAL NetCall 			(HWND, LPSTR, LPSTR, BYTE, BYTE, PNCB FAR *, BYTE);
BYTE __declspec(dllexport) FAR PASCAL NetListen 		 	(HWND, LPSTR, LPSTR, BYTE, BYTE, PNCB FAR *, BYTE);
BYTE __declspec(dllexport) FAR PASCAL NetSend			 	(HWND, LPSTR, WORD, BYTE, PNCB FAR *, BYTE);
BYTE __declspec(dllexport) FAR PASCAL NetSendDG			(HWND, LPSTR, WORD, LPSTR, BYTE, PNCB FAR *, BYTE);
BYTE __declspec(dllexport) FAR PASCAL NetSendBCDG		(HWND, LPSTR, WORD, BYTE, PNCB FAR *, BYTE);
BYTE __declspec(dllexport) FAR PASCAL NetReceive 		(HWND, WORD, BYTE, PNCB FAR *, BYTE);
BYTE __declspec(dllexport) FAR PASCAL NetReceiveAny	 	(HWND, WORD, BYTE, PNCB FAR *, BYTE);
BYTE __declspec(dllexport) FAR PASCAL NetReceiveDG		(HWND, WORD, BYTE, PNCB FAR *, BYTE);
BYTE __declspec(dllexport) FAR PASCAL NetReceiveBCDG	(HWND, WORD, BYTE, PNCB FAR *, BYTE);

/* Time out values */
#define CALL_TIME_OUT	 (double) (30 * 2) /* 2 Minutes */
#define SEND_TIME_OUT	 (double) (30 * 2) /* 2 Minutes */
#define RECEIVE_TIME_OUT (double) (60 * 2) /* 2 Minutes */

#define SEND_NCB_STO	(BYTE) ((2 * 30)* 1)   /* 1 Minutes in 1/2 sec intervals */
#define RECEIVE_NCB_RTO (BYTE) ((2 * 60)* 1)/* 1 Minutes in 1/2 sec intervals */

/****************************************************************************
	 FILE: NCBSUFU.C  -  General support Routines
****************************************************************************/

VOID FAR PASCAL NetCopyName (LPSTR, LPSTR);

#ifdef __cplusplus
}
#endif