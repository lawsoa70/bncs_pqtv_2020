#ifndef globalXY_INCLUDED
	#define globalXY_INCLUDED

#include <bncs_script_helper.h>
//#include "..\..\source\shared\LastSceRouteHandler.h"
#include "..\..\SOURCE\INCLUDE\bncs_stringlist.h"	// Added by ClassView

#ifdef WIN32
	#ifdef DOEXPORT_SCRIPT
		#define EXPORT_SCRIPT __declspec(dllexport) 
	#else
		#define EXPORT_SCRIPT __declspec(dllimport) 
	#endif
#else 
	#define EXPORT_SCRIPT
#endif

//# define LAST_SOURCE_DATABASE 0x3


class globalXY : public bncs_script_helper
{
public:
	void selectRouter(int intRouter);
	globalXY( bncs_client_callback * parent, const char* path );
	virtual ~globalXY();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
	//helper functions
	void dump(const bncs_string & var, const bncs_string & value);
private:	
	void setListboxSelectedItem(bncs_string strControl, int intPortIndex);
	bncs_string m_strRouterList;			//List of routers
	bncs_stringlist m_sltRouterInstances;	//
	void load( int intRouter );
	int m_iDestNo;						//The currently selected dest
	int m_iSourceNo;					//The currently selected source
	int m_iDeviceNo;					//The dev id of the current router
	int m_iDeviceLockNo;				//The dev id of the lock infodriver of the current router
	bncs_string sLastSource;
	bool m_doRenameDest;
	bool fIgnoreSelectionChange;
	//int m_iRenameSourceNo;				//The source that is being renamed
	
	int m_intCurrentDestCurrentSource;	//The source that the currently selected destination currently has routed to it
	int m_intUndoSource;				//The source that will be routed to the current destination if UNDO is pressed

	int m_intLastRouter;	//the last router that was selected
	
	int m_iRenameDetails; //What information to show on the rename panel
	bool m_bRenameSoftNames; // What information to show on the rename panel

	void loadSourceNames( bool maintainSelectedHighlight=false );
	void loadDestNames( bool maintainSelectedHighlight=false );
	void showDump(void);
	void showSourceIndex(int intSource);
	void setRouterList(bncs_string strRouterList);
 	void highlightButton( int intRouter);
	void renamePanel( bool bDest);
	void setRenameTypes(bncs_string instance);

	int m_intNumpadIndex;	// holds the current index for the numpad
	bool m_blnShowUnusedPorts;	//flag to enable unused ports of a router to be displayed
	bool m_blnCurrentDestLocked;	//flag to hold the lock state of the current destination

	int m_intRenameDatabase;	//name database that is currently being edited using the keyboard
	bool m_blnAlphaSortSource;	//current mode used for sorting source list - true=Alpha, false=Numeric
	bool m_blnAlphaSortDest;	//current mode used for sorting dest list - true=Alpha, false=Numeric
	

};


#endif // globalXY_INCLUDED