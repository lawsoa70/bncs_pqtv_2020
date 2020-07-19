#ifndef topstrap_INCLUDED
	#define topstrap_INCLUDED

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
 
class topstrap : public bncs_script_helper
{
public:
	topstrap( bncs_client_callback * parent, const char* path );
	virtual ~topstrap();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
private:

	//Common vars
	bncs_string m_strArea;
	bncs_string m_strLayout;

	//Common Functions
	bncs_string getSetting(bncs_string id, bncs_string parameter);
	void setArea(bncs_string strArea);

};


#endif // topstrap_INCLUDED