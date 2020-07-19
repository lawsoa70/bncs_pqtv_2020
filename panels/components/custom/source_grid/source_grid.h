#ifndef source_grid_INCLUDED
	#define source_grid_INCLUDED

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
 
class source_grid : public bncs_script_helper
{
public:
	source_grid( bncs_client_callback * parent, const char* path );
	virtual ~source_grid();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
	
private:
	bncs_string m_strInstance;
	bncs_string m_strLayout;
	
	int m_currentPage;
};


#endif // source_grid_INCLUDED