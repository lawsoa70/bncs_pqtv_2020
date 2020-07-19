#if !defined(AFX_CSIDRV_H__A9DB83DB_A9FD_11D0_BFD1_444553545200__INCLUDED_)
#define AFX_CSIDRV_H__A9DB83DB_A9FD_11D0_BFD1_444553545200__INCLUDED_

#include <bncsdef.h>

#ifndef GTHUNKS_EXPORTS
#undef GTHUNKS_API
#define GTHUNKS_API
#endif

class GTHUNKS_API extdriver {
	HWND hWndSpawn;
	PULONG txcount;
	PULONG rxcount;
	LPSTR netmsgtok;
	UINT iStatus;
	UINT iDrvType;
	static HINSTANCE hInstEx;
	ULONG deftx,defrx;
public:
	UINT iDevice;
	LPSTR netmsg;
	UINT paramcount;
	LPSTR param[16];
	LRESULT (*func)(extdriver*,LPCSTR);
	UINT connect(UINT iDevice, HINSTANCE hInstApp=NULL, UINT iType=DRVTYPE_NONE);
	UINT connectx(UINT iDevice);
	extdriver();
	~extdriver();
	void notify(LRESULT(*)(extdriver*,LPCSTR));
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
};

#endif