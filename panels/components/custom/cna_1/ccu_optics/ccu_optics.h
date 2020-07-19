#ifndef ccu_optics_INCLUDED
	#define ccu_optics_INCLUDED

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
 
class ccu_optics : public bncs_script_helper
{
public:
	ccu_optics( bncs_client_callback * parent, const char* path );
	virtual ~ccu_optics();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
	
private:
	bncs_string m_instance;
	bncs_string m_strLevel;
};


#endif // ccu_optics_INCLUDED