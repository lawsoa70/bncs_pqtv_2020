#ifndef routerDetails_INCLUDED
	#define routerDetails_INCLUDED

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
 
class routerDetails : public bncs_script_helper
{
public:
	routerDetails( bncs_client_callback * parent, const char* path );
	virtual ~routerDetails();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );

private:
	bncs_string m_instance;
	int m_device;
	int m_offset;
	int m_database;
	bncs_string m_strDatabase;
	int m_index;
	bncs_string m_text;
};


#endif // routerDetails_INCLUDED