#ifndef lock_INCLUDED
	#define lock_INCLUDED

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
 
class lock : public bncs_script_helper
{
public:
	lock( bncs_client_callback * parent, const char* path );
	virtual ~lock();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
private:
	int m_device;
	bncs_stringlist m_index;
	bncs_stringlist m_states;
	int m_state;
	bncs_string m_instance;
};


#endif // lock_INCLUDED