#ifndef mv_select_INCLUDED
	#define mv_select_INCLUDED

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
 
class mv_select : public bncs_script_helper
{
public:
	mv_select( bncs_client_callback * parent, const char* path );
	virtual ~mv_select();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
	
private:
	bncs_string m_strArea;			// e.g. studio_b
	bncs_string m_strMvSettings;	// e.g. studio_b_multiviewers
	bncs_string m_strLayoutDriver;	// the evertz driver instance
	int m_intLayoutDriverDev;		// its device number
	int m_intLayoutInfoSlot;
	bncs_string m_strRouter;		// SDI router instance
	int m_intRouterDev;				// its device number
	bncs_stringlist m_slAreaMvList;	// list of the mvs in the area
	bncs_stringlist m_slLayoutList; // list of available layouts for the current mv

	bncs_string getCurrentMv ();
	void changeMvSelection (bncs_string strMvInstance);
	void createLayoutButton(bncs_string strLayoutName, bncs_string strControlName, int intButtonCount, bool blnReuse);
};


#endif // mv_select_INCLUDED