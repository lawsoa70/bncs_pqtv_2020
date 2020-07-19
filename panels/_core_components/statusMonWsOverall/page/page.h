#ifndef page_INCLUDED
	#define page_INCLUDED

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
 
class page : public bncs_script_helper
{
public:
	page( bncs_client_callback * parent, const char* path );
	virtual ~page();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
	
private:
	bncs_string m_text;
	int m_device;
	map< int, bncs_string > m_workstations;
	int m_alarmState;
	void checkAlarmState( void );
};


#endif // page_INCLUDED