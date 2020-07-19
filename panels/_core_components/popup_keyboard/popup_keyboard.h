#ifndef popup_keyboard_INCLUDED
	#define popup_keyboard_INCLUDED

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
 
class popup_keyboard : public bncs_script_helper
{
public:
	popup_keyboard( bncs_client_callback * parent, const char* path );
	virtual ~popup_keyboard();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
	
private:
	bncs_string m_myParam;
	bncs_string m_instance;
	
};


#endif // popup_keyboard_INCLUDED