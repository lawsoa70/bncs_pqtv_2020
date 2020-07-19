#pragma once
#include <windows.h>
#include <bncsdef.h>

#ifndef GTHUNKS_EXPORTS
#undef GTHUNKS_API
#define GTHUNKS_API
#endif 

class GTHUNKS_API cGRD
{
	HWND hWndSpawn;
	PULONG txcount;
	PULONG rxcount;
	ULONG deftx,defrx;
	static LRESULT WINAPI CallbackProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT (*Cxptevent)(cGRD *cG, UINT iDevice, UINT iSrc, UINT iDest, LPCSTR szMask);
	HWND hWndGRD;
protected:
	virtual LRESULT xptevent(cGRD *cG, UINT iDevice, UINT iSrc, UINT iDest, LPCSTR szMask);
public:
	UINT iGRD;
	int iStatus;

	cGRD(void);
	~cGRD(void);
	int connect(UINT iExtGRD);
	void setcounters(PULONG lpTX,PULONG lpRX);
	int getsrc(int iDest);
	bool setdest(int iSrc, int iDest);
	void incrx();
	void notify( LRESULT(*in)(cGRD *,UINT iDevice, UINT iSrc, UINT iDest, LPCSTR szMask));
};
