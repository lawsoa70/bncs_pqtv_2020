#ifndef tab_INCLUDED
	#define tab_INCLUDED

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
 
class tab : public bncs_script_helper
{
public:
	tab( bncs_client_callback * parent, const char* path );
	virtual ~tab();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
	bncs_string panelPath(bncs_string panelName);

	bncs_string m_layout;
	bncs_stringlist m_tabList;
	bncs_stringlist m_UIList;
	bncs_stringlist m_tabName;
	bncs_string m_firstTabShown;
	bncs_string m_panelLocation;
	bncs_string m_instance;
	bncs_string m_lastTab;
	bncs_string m_lastUIShown;

	bncs_string m_selectedState, m_deselectedState;
	bncs_string m_bgSelectedPixmap, m_bgDeselectedPixmap;

private:
	void selectTab( bncs_string );
	void deselectTab( bncs_string );
	void textPutPanel( bncs_string, bncs_string, bncs_string, bncs_string );

};


#endif // tab_INCLUDED