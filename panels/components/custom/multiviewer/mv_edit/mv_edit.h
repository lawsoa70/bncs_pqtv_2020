#ifndef mv_edit_INCLUDED
	#define mv_edit_INCLUDED

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
 
class mv_edit : public bncs_script_helper
{
public:
	mv_edit( bncs_client_callback * parent, const char* path );
	virtual ~mv_edit();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
	
private:
	//bncs_string m_myParam;
	bncs_string m_strInstance;
	int m_intDevice;
	int m_intTallyDatabase;
	bncs_stringlist m_slMvSourceList;
	bncs_stringlist m_slMvList;
	int m_intSelectedDest;
	int m_intSelectedSource;
	bncs_string m_strArea;
};


#endif // mv_edit_INCLUDED