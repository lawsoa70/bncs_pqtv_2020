#ifndef axonIdentity_INCLUDED
	#define axonIdentity_INCLUDED

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
 
class axonIdentity : public bncs_script_helper
{
public:
	axonIdentity( bncs_client_callback * parent, const char* path );
	virtual ~axonIdentity();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
};


#endif // axonIdentity_INCLUDED