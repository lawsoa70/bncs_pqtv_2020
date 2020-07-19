#ifndef routerList_INCLUDED
	#define routerList_INCLUDED

#include <bncs_script_helper.h>

#ifdef WIN32
	#ifdef DOEXPORT_SCRIPT
		#define EXPORT_SCRIPT __declspec(dllexport) 
	#else
		#define EXPORT_SCRIPT 
	#endif
#else 
	#define EXPORT_SCRIPT
#endif
 
class routerList : public bncs_script_helper
{
public:
	routerList( bncs_client_callback * parent, const char* path );
	virtual ~routerList();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
	
private:
	int m_device;
	bncs_string m_instance;
	int m_database;
	bool m_sort;
	bncs_string m_remove_begins;
	int filtered;

	void loadListbox( void );
};


#endif // routerList_INCLUDED