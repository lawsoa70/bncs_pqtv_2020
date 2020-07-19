#ifndef copy_paste_INCLUDED
	#define copy_paste_INCLUDED

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
 
class copy_paste : public bncs_script_helper
{
public:
	copy_paste( bncs_client_callback * parent, const char* path );
	virtual ~copy_paste();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );

	void pageCopy();
	void pagePaste();
	
private:
	bncs_string m_myParam;

	int m_mapDB;
	int m_mapDBOffset;
	int m_pageIndex;
	int m_copyPageIndex;
	int m_iDevice;

	bncs_string m_instance;

};


#endif // copy_paste_INCLUDED