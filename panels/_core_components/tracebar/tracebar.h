#ifndef tracebar_INCLUDED
	#define tracebar_INCLUDED

#include <bncs_script_helper.h>

#ifdef WIN32
	#ifdef DOEXPORT_SCRIPT
		#define EXPORT_SCRIPT __declspec(dllexport) 
	#else
		#define EXPORT_SCRIPT __declspec(dllimport) 
	#endif
#else 
	#define EXPORT_SCRIPT
#endif

#include <map>
using namespace std;
 
#define MAX_LEVEL 10
#define MAX_INDEXES 20
#define DB_SOURCE 0
#define DB_DESTINATION 1
#define DB_UNKNOWN -1
#define DB_SOURCE_ADJUST 6
#define DB_DESTINATION_ADJUST 7

class tracebarItem
{
	int m_sourceDevice;
	int m_destDevice;
	int m_dest;
	int m_source;
};

class tracebar : public bncs_script_helper
{
public:
	tracebar( bncs_client_callback * parent, const char* path );
	virtual ~tracebar();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	
	void resetTrace( void );
	void resetTraceFrom( int button );
	
	void timerCallback( int );
	void iteration( void );
	bool showName( void );
	void init( void );

private:
	bool m_blnReadOnly;			//used in package router source/dest info views

	// filled from object_settings.xml
	int m_iNoIndexes;
	bncs_stringlist m_routerList[MAX_INDEXES];

	// set by parent
	bncs_string m_sInstance;

	// run by process
	bncs_string m_iterationRouterName;
	int m_iterationDatabase;
	int m_iterationDevice;
	int m_iterationDestination;
	int m_iterationPosition;				// current iteration step

	bool m_packager;
	bool m_dynamicUpdates;

	// parameters
	bncs_string m_sUnusedStyle;
	bncs_string m_sSourceStyle;
	bncs_string m_sRouteStyle;
	bncs_string m_sDestinationStyle;
	bncs_string m_sPanel;

	bncs_stringlist m_idlist;

	map< int, bncs_string> m_adjustnames;	// position > adjustName
	map< DWORD, DWORD > m_reverseMap;		// (device/slot) > (position/tally)
};


#endif // tracebar_INCLUDED
