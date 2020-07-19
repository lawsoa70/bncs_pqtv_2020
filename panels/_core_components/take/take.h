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
 
class take : public bncs_script_helper
{
public:
	take( bncs_client_callback * parent, const char* path );
	virtual ~take();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
private:
	int m_device;
	int m_lockDevice;
	int m_source;
	int m_dest;
	int m_state;
	bncs_string m_instance;
	bncs_string m_lockInstance;
	bool m_undoPossible;

	void checkEnableState( void );

};


#endif // lock_INCLUDED