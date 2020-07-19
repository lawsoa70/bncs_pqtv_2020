/****************************************************/
/* Written by David Yates                           */
/* Copyright Atos 2008 */
/****************************************************/
#include <windows.h>

#ifndef CCCLIENT_AS_DLL
	#define CCCLIENT_AS_DLL
#endif

#pragma warning ( disable: 4251 4786 )

#include <ccClient.h>
#include <map>
using namespace std;


#ifdef DOEXPORT_SHIM
	#define EXPORT_SHIM __declspec(dllexport) 
#else
	#define EXPORT_SHIM __declspec(dllimport) 
#endif

class regStruct
{
public:
	regStruct( IccClientCallback * _w, int _device, int _min, int _max )
	{
		w = _w;
		device = _device;
		min = _min;
		max = _max;
		deleted = false;
	};
	IccClientCallback * w;
	int device;
	int min;
	int max;
	bool deleted;
};

class pollStruct
{
public:
	pollStruct( int d, int s, char t, int retryDelay )
	{
		device = d;
		slot = s;
		type = t;
		retryDelayCount = retryDelay;
		retryCount = 0;
	};
	int device;
	int slot;
	char type;
	int retryCount;			// how many times we've retried
	int retryDelayCount;	// how long we should wait before trying again
};

#define INSERT 0
#define OVERWRITE 1
#define LATER 0
#define NOW 1

#define	RETRY_DEFAULT_DELAY	40	// default number of 100mS to wait before trying to poll again
#define RETRY_DELAY_MULTIPLIER 40	// this times number of retries 
#define RETRY_COUNT_MAXIMUM	12	// maximum number of retres at count*4secs is approx 5 mins

#include <timer.h>

class EXPORT_SHIM bncs_dll_client_shim : public ccClient, private IccClientCallback, private ItimerCallback
{
public:
	bncs_dll_client_shim();
	virtual ~bncs_dll_client_shim();
	
	void infoPoll( int device, int start, int end, bool when = LATER );
	void gpiPoll( int device, int start, int end, bool when = LATER );
	void routerPoll( int device, int start, int end, bool when = LATER );

	bool reg( IccClientCallback *w, int device, int min, int max, int insert = OVERWRITE );
	void unreg(IccClientCallback *w, int device = 0);
	void unreg(IccClientCallback *w, int device, int slot);
	int totalRegistrations( void );
	int deletedRegistrations( void );
	void dumpRegistrations( void );
	void counters( unsigned int & tx, unsigned int & rx, unsigned int & retryFail );
	void resetCounters( void );
	int pollsOutstandingCount( void );
	int pollsOutstandingDeviceList( int * out, int size );

	void cleanupIfRequired(); // Use this on program termination.
	bool isRegistered( int device, int slot );
private:
#pragma warning (disable: 4251 )
	multimap< int, regStruct* > regList;
	typedef multimap< int, regStruct* >::iterator regListIt;

	map< long, pollStruct* > pollList;
	typedef map< long, pollStruct* >::iterator pollListIt;
#pragma warning (default: 4251 )

	bool m_regCleanupRequired;
	int iMaxTx;

	unsigned int iRetryFailCount;
	
	void rereg( int device ); 
	void timerCallback( int id );
	void removePoll(int iDev, int iSlot);
	bool addPoll( char type, int device, int start, int end, bool when );

	// ccClient callback functions
	void cccConnected( void );
	void cccDisconnected( void );
	void cccInfoRevertive( int device, int index, const string & value );
	void cccRtrRevertive( int device, int index, int info );
	void cccGpiRevertive( int device, int index, bool state );
	void cccDatabaseChange( int device, int index, int database );
	void cccDeviceMode( int workstation, int device, enum cc::redundancyState state );
	void cccDeviceMessage( int workstation, int device, const string & message, int reference );
	void cccConfigChangeMessage( const string & msg );
	void regCleanup( void );

	timer *m_timer;
	bool m_timerRunning;

	bool m_connected;
};
 