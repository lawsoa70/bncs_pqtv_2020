#include <bncsdef.h>


#ifndef GTHUNKS_EXPORTS
#undef GTHUNKS_API
#define GTHUNKS_API
#endif

class GTHUNKS_API extclient {
	HWND hWndSpawn;
	PULONG txcount;
	PULONG rxcount;
	LPSTR netmsgtok;
	UINT iStatus;
	LPSTR netmsg;
	UINT paramcount;
	LPSTR param[16];
	static HINSTANCE hInstEx;
	ULONG deftx,defrx;
	UINT iWorkstation;
	static LRESULT WINAPI ClientWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
public:
	UINT getparamcount(void);
	LPSTR getparam(UINT);
	LRESULT (*func)(extclient*,LPCSTR);
	UINT connect(HINSTANCE hInstApp=NULL);
	extclient();
	~extclient();
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
	void notify(LRESULT(*)(extclient*,LPCSTR));
	void setcounters(PULONG rxcounter, PULONG txcounter);
	void incrx(void);
	void getdbname(WORD device, WORD database, WORD index, LPSTR namebuffer, int iMaxSize=32);
	int getdbindex(WORD device, WORD database, LPCSTR name);
	void setdbname(WORD device, WORD database, WORD index, LPCSTR name, BOOL fPoll=0);
	UINT tokenize(void);
	LRESULT txnetcmd(LPCSTR szMessage);
};
