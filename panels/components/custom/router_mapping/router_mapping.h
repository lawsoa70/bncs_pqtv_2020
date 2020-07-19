#ifndef router_mapping_INCLUDED
	#define router_mapping_INCLUDED

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
 
class router_mapping : public bncs_script_helper
{
public:
	router_mapping( bncs_client_callback * parent, const char* path );
	virtual ~router_mapping();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );

private:
	bncs_string m_sourcePanel;
	bncs_string m_destPanel;
	bncs_string m_instance;
};

#endif // router_mapping_INCLUDED
 