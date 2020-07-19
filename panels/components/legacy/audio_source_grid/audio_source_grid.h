#ifndef audio_source_grid_INCLUDED
	#define audio_source_grid_INCLUDED

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
 
class audio_source_grid : public bncs_script_helper
{
public:
	audio_source_grid( bncs_client_callback * parent, const char* path );
	virtual ~audio_source_grid();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
private:
	void setupGrid();
	void loadPage(int pageNumber, bool tellHost);

	bncs_string m_strLayout;
	int m_intPageOffset;
	bncs_string m_strInstance;
	int m_intLastPage;
	bool m_blnTellHost;
};


#endif // audio_source_grid_INCLUDED