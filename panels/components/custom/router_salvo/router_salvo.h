#ifndef router_salvo_INCLUDED
	#define router_salvo_INCLUDED

#include <bncs_script_helper.h>
#include "..\..\..\..\SOURCE\INCLUDE\bncs_string.h"	// Added by ClassView

#ifdef WIN32
	#ifdef DOEXPORT_SCRIPT
		#define EXPORT_SCRIPT __declspec(dllexport) 
	#else
		#define EXPORT_SCRIPT __declspec(dllimport) 
	#endif
#else 
	#define EXPORT_SCRIPT
#endif

class router_salvo : public bncs_script_helper
{
public:
	router_salvo( bncs_client_callback * parent, const char* path );
	virtual ~router_salvo();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
private:
	void confirmNewSalvoName();
	bncs_string getCurrentSalvo();
	void updateSalvoList();
	void saveSalvo(bncs_string strSalvoName);
	void switchViewKeyboard(bool blnKeyboardVisible);
	
	bncs_string m_strLayout;			//The name of the layout that this component has been loaded onto
	bncs_stringlist m_slDevDestList;		//The list of dev_id,destinations that the layout contains to be used for a recall filter
	bncs_string m_strRouterInstance;	//the name of the router instance
	bncs_string m_strMemoryInstance;	//The instance of the memory automatic
	int m_intRouterDevice;				//The device id of the router
	int m_intMemoryDevice;				//The device id of the memory automatic
};


#endif // router_salvo_INCLUDED