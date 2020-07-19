#if !defined(AFX_CDRIVER_H__A9DB83DB_A9FD_11D0_BFD1_444553545200__INCLUDED_)
#define      AFX_CDRIVER_H__A9DB83DB_A9FD_11D0_BFD1_444553545200__INCLUDED_

#include <bncsdef.h>

#ifndef GTHUNKS_EXPORTS
#undef GTHUNKS_API
#define GTHUNKS_API
#endif

class GTHUNKS_API cDriver {
	HWND hWndSpawn;
	PULONG txcount;
	PULONG rxcount;
	LPSTR netmsgtok;
	LPSTR szServerName;
	UINT iStatus;
	UINT iDrvType;
	ULONG deftx,defrx;
	static LRESULT WINAPI DriverWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
protected:
	virtual LRESULT notify(cDriver* ex,LPCSTR lps)=0;
public:
	UINT iDevice;
	LPSTR netmsg;
	UINT paramcount;
	LPSTR param[16];
//	LRESULT (*func)(cDriver*,LPCSTR);
	UINT connect(UINT iDevice, LPCSTR szServer="", UINT iType=DRVTYPE_NONE);
	UINT connectx(UINT iDevice);
	disconnect();
	cDriver(UINT iTest=235076);
	~cDriver();
//	void setCcallback(LRESULT(*)(cDriver*,LPCSTR));
	void setcounters(PULONG txcounter, PULONG rxcounter);
	void incrx(void);
	UINT getstate(void);
	UINT setstate(UINT stateflags);
	LRESULT txrevmsg(LPSTR netmessage,UINT iCallingWS);
	LRESULT txquerymsg(LPSTR netmessage,UINT iCallingWS, BOOL lastflag=FALSE);
	void getdbname(WORD device, WORD database, WORD index, LPSTR namebuffer, int iMaxSize=32);
	int getdbindex(WORD device, WORD database, LPCSTR name);
	UINT gettype(void);
	LRESULT setifmode(WORD mode);
	WORD getifmode(void);
	UINT tokenize(void);
	LPCSTR getServerName(void);
};

#endif