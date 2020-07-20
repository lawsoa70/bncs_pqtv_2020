#ifndef router_test_monitors_INCLUDED
	#define router_test_monitors_INCLUDED

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
 
class router_test_monitors : public bncs_script_helper
{
public:
	router_test_monitors( bncs_client_callback * parent, const char* path );
	virtual ~router_test_monitors();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );

private:
	void registerPollStatusSlot();
	void setTestState(int intCommand);
	void displaySelectedSource();
	bncs_string m_strRouterInstance;	//the name of the router instance
	int m_intRouterDevice;				//the device number of the router instance
	int m_intTestSourceIndex;			//the index of the default test source to use
	bncs_string m_strBackgroundStyle;	//Style supplied by parent for background of component
	bncs_stringlist m_slDevDestList;	//fetched comma list of destinations on panel
	bncs_stringlist m_slDevIdList;		//fetched comma list of destination Ids on panel
	bncs_string m_strLayout;			//layout to use for test memory save and recall

	bool m_blnTestState;				//the test state of the control
	bncs_string m_strMemoryInstance;	//the instance of the memory automatic used to store the restore snapshot
	int m_intMemoryDevice;				// ...and the device number for that instance

	bncs_string m_strTestStatusInstance;	//the infodriver instance for the shared test state slot
	int m_intTestInfoDriver;				//the infodriver device for the shared test state slot
	int m_intTestStatusSlot;				//the infodriver slot for the shared test state

	void notifyTestState(int intState);	//sends new state to parent panel
	bncs_string m_strStudio;			//id of the studio eg sta or stc
};


#endif // router_test_monitors_INCLUDED