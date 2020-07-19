#ifndef adjust_INCLUDED
	#define adjust_INCLUDED

#pragma message( "************************************************************************")
#pragma message( "************************************************************************")
#pragma message( "************************************************************************")
#pragma message( "**           THIS IS A CORE COMPONENT - PLEASE DON'T MODIFY!          **")
#pragma message( "************************************************************************")
#pragma message( "************************************************************************")
#pragma message( "* Do however, feel free to copy it somewhere else, remove this message *")
#pragma message( "* and use & adapt however you see fit                                  *")
#pragma message( "************************************************************************")
#pragma message( "************************************************************************")
#pragma message( "************************************************************************")

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
 
class adjust : public bncs_script_helper
{
public:
	adjust( bncs_client_callback * parent, const char* path );
	virtual ~adjust();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
private:
	bncs_string m_instance;
	int m_database;
	int m_device;
	int m_offset;
	bncs_string m_adjustName;
	bncs_string m_text;
};


#endif // adjust_INCLUDED