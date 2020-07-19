
#ifndef COM32C_INCLUDED
	#define COM32C_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif
/* exports */
BOOL EnableCommNotification(HANDLE hCom,HWND hWndMain, int iBytesNotify);
int ReadComm(HANDLE hCom,LPBYTE bDest,WORD wBytes);
int WriteComm(HANDLE hCom,LPBYTE bData, WORD wBytes);
HANDLE OpenComm32(UINT iPort,long lBaudRate,int iDataBits,int iStopBits, int iParity);
void CloseComm32(HANDLE hCom);
DWORD GetCommError(HANDLE hCom,LPCOMSTAT);
BOOL FlushComm(HANDLE hCom, ULONG ulFlags);
BOOL ConfigCommDialog(HANDLE hCom, HWND hWnd);
BOOL GetCommState32(HANDLE, DCB*);
BOOL SetCommState32(HANDLE, DCB*);

/* spare exports for use */
HGLOBAL bInitBuf(UINT);
void bKillBuf(HGLOBAL hGBuf);
void bKillBufEx(HGLOBAL* hGBuf);
BOOL bAddData(HGLOBAL hGBuf,BYTE* data, UINT iLen);
UINT bGetData(HGLOBAL hGBuf,BYTE* data, UINT iLen);

#ifdef __cplusplus
}
#endif

#endif // COM32C_INCLUDED