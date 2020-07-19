#ifndef tally_INCLUDED
	#define tally_INCLUDED

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
 
class tally : public bncs_script_helper
{
public:
	tally( bncs_client_callback * parent, const char* path );
	virtual ~tally();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	int getInstanceDevice(const bncs_string & instance);
	void timerCallback( int );
	bncs_string m_spanelname;
	bncs_string m_sdestindex;
	bncs_string m_sinstance;
	int m_iinstance;
	bncs_string m_sdatabase;
	int m_idatabase;
	bncs_string m_ssingleline;
	bool m_bsingleline;

};


#endif // tally_INCLUDED