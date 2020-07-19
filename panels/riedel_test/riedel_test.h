#ifndef riedel_test_INCLUDED
	#define riedel_test_INCLUDED

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
 
class riedel_test : public bncs_script_helper
{
public:
	riedel_test( bncs_client_callback * parent, const char* path );
	virtual ~riedel_test();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
	
private:
	bncs_string m_myParam;
	bncs_string m_instance;
	
};


#endif // riedel_test_INCLUDED