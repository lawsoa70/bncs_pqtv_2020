#ifndef router_shortcuts_INCLUDED
	#define router_shortcuts_INCLUDED

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
 
class router_shortcuts : public bncs_script_helper
{
public:
	router_shortcuts( bncs_client_callback * parent, const char* path );
	virtual ~router_shortcuts();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
	bncs_string m_strRouterInstance;
	bncs_string m_strPanel;
	bncs_string getLabel(int arrLabelPos, int device, int database, int index);
	void setButtonsState(bncs_string btnId);
	void setButtonsState(bncs_stringlist btnIds);
	int setListSizes();

	//int arrSourceIndex[10];
	bncs_stringlist listSourceIndex;
	//bncs_string arrButtonLabel[10];
	bncs_stringlist listButtonLabel;
	//bncs_string arrButtonAlias[10];
	bncs_stringlist listButtonAlias;
	int m_intSourceIndex;
	bncs_string m_strSourceName;
	int m_intDevice;
	int m_intRouterSize;
	bncs_string m_strBackgroundColour;
};


#endif // router_shortcuts_INCLUDED