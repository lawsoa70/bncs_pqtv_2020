#ifndef statusMonWs_INCLUDED
	#define statusMonWs_INCLUDED

#include <bncs_script_helper.h>

#include <map>
using namespace std;

#ifdef WIN32
	#ifdef DOEXPORT_SCRIPT
		#define EXPORT_SCRIPT __declspec(dllexport) 
	#else
		#define EXPORT_SCRIPT __declspec(dllimport) 
	#endif
#else 
	#define EXPORT_SCRIPT
#endif
 
class statusMonWs : public bncs_script_helper
{
public:
	statusMonWs( bncs_client_callback * parent, const char* path );
	virtual ~statusMonWs();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );

	void loadWorkstationNames( void );
	bncs_string workstationName( void );

	void reset( void );
private:
	int m_ws;
	int m_device;
	bncs_string m_instance;

	static map< int, bncs_string > m_workstationNames;

	void check( void );

	bncs_string m_lastEntry;
	DWORD m_lastUpdated;

	int m_alarmState;
};


#endif // statusMonWs_INCLUDED