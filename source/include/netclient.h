#ifndef NETCLIENT_INCLUDED
	#define NETCLIENT_INCLUDED

#include <winsock.h>

class netclient {
    LPHOSTENT host_info;
    LPSERVENT serv_info;
	HWND hWnd;
	SOCKET sock;
	SOCKADDR_IN	server;
	long notifyflags;
	LPTSTR szServerName;
public:
	netclient(HINSTANCE hInst=NULL);
	~netclient();
	int open(LPCSTR szServer,int iPort=23);
	close(void);
	long txdata(PBYTE bData,long lLen);
	long txdata(LPCSTR szData);
	long rxdata(PBYTE bData,long lLen);
	BOOL notify(void(*)(netclient*), long lNot);
	void (*func)(netclient*);
	long thisnotify;
	long thiserror;
	SOCKET getsocket() {return sock;}
	SOCKADDR_IN getserver() {return server;}
	LPCTSTR getservername() {return szServerName;}
	int getport();
};


#endif // NETCLIENT_INCLUDED