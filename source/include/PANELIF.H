#ifndef PANELIF_INCLUDED
	#define PANELIF_INCLUDED

#include <bncsdef.h>
#include <windows.h>


#ifndef GTHUNKS_EXPORTS
#undef GTHUNKS_API
#define GTHUNKS_API
#endif

//WORKS WITH panelif VERSION 1.20

class GTHUNKS_API cPanel {
	HWND hWndSpawn;
	PULONG txcount;
	PULONG rxcount;
	LPSTR netmsgtok;
	LPSTR szServerName;
	UINT iStatus;
	LPSTR netmsg;
	UINT paramcount;
	LPSTR param[16];
	ULONG deftx,defrx;
	UINT iWorkstation;
	UINT tokenize(void);
	void incrx(void);
	static LRESULT WINAPI ClientWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
protected:
	/*the new stuff */
	virtual LRESULT notify(cPanel*,LPCSTR)=0;
public:
	UINT getparamcount(void);
	LPSTR getparam(UINT);
	UINT connect(LPCSTR szServer="");
	cPanel();
	virtual ~cPanel();
	UINT setstate(UINT iState);
	UINT getstate(void);
	void setcachestate(UINT iState);
	UINT getcachestate(void);
	BOOL cachetest(UINT iDevice, UINT iMin, UINT iMax);
	void cacheflush(UINT iDevice);
	LRESULT txrtrcmd(LPCSTR szMessage, BOOL fWhen=LATER);
	LRESULT txinfocmd(LPCSTR szMessage, BOOL fWhen=LATER);
	LRESULT txgpicmd(LPCSTR szMessage, BOOL fWhen=LATER);
	LRESULT regtallyrange(UINT iDevice, UINT iMin, UINT iMax, BOOL fInsert=OVERWRITE);
	LRESULT unregtallyrange(UINT iDevice=0, UINT iMin=0, UINT iMax=0);
	void setcounters(PULONG rxcounter, PULONG txcounter);
	void getdbname(WORD device, WORD database, WORD index, LPSTR namebuffer, int iMaxSize=32);
	int getdbindex(WORD device, WORD database, LPCSTR name);
	void setdbname(WORD device, WORD database, WORD index, LPCSTR name, BOOL fPoll=0);
	/*the new stuff */
	LPCSTR getServerName(void){return szServerName;}
	BOOL getcachedata(UINT iDevice, UINT iParam, LPSTR szResult, UINT iMaxLen);
	int getcacheindex(UINT iDevice, UINT iMin, UINT iMax, LPCSTR szSearchString);
	LRESULT txnetcmd(LPCSTR szMessage);
//hint	void deviceprofile( ... );
};

#endif // PANELIF_INCLUDED