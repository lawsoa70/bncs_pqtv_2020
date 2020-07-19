#ifndef router_dest_INCLUDED
	#define router_dest_INCLUDED

#pragma message( "************************************************************************")
#pragma message( "************************************************************************")
#pragma message( "************************************************************************")
#pragma message( "**           THIS IS A CORE COMPONENT - PLEASE DON'T MODIFY!          **")
#pragma message( "************************************************************************")
#pragma message( "************************************************************************")
#pragma message( "* Do however, feel free to copy it somewhere else, remove this message *")
#pragma message( "* and use & adapt however you see fit                                  *")
#pragma message( "************************************************************************")
#pragma message( "************************************************************************")
#pragma message( "************************************************************************")

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

#define PNL_MAIN		1
	
#define PANEL_DEFAULT	"default"

//Nexus Database usage
#define DB_HYDRA_IN_TYPE	6
#define DB_HYDRA_OUT_TYPE	7

#define TAKE_NORMAL		"NORMAL"
#define TAKE_M_L		"M-L"
#define TAKE_M_R		"M-R"
#define TAKE_L_L		"L-L"
#define TAKE_L_LR		"L-LR"
#define TAKE_L_R		"L-R"
#define TAKE_R_L		"R-L"
#define TAKE_R_LR		"R-LR"
#define TAKE_R_R		"R-R"
#define TAKE_L_M		"L-M"
#define TAKE_R_M		"R-M"
 
#define	ROUTER_TYPE_HYDRA	"hydra"

#define TIMER_INITIALLY_SELECTED	1
#define TIMER_DEST_INDEX			2

class router_dest : public bncs_script_helper
{
public:
	router_dest( bncs_client_callback * parent, const char* path );
	virtual ~router_dest();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );

private:
	bncs_string m_strAlias;

	bool m_blnLockable;					//flag to store if the dest is lockable
	bool m_blnLockState;				//flag to store current lock state - true=locked

	bool m_blnStereoDest;				//flag to store if the dest is stereo

	bool m_blnKeepLastSource;			//flag enabled if this dest is also routed by OCP GPI auto
	bool m_blnEnabled;
	bool m_blnInitiallySelected;
	bool m_blnSelected;

	bncs_string m_strRouterInstance;	//the name of the router instance
	bncs_string m_strPanel;				//the name of the panel layout to display
	int m_intDestIndex;					//the index of the destination
	int m_intLockDevice;				//the dev id of the router's lock infodriver
	int m_intRouterDevice;				//the dev id of the router
	int m_intDefaultSource;				// the default source routed to this destination
	
	//NOTE this is always custom so shouldn't be configurable
	bncs_string m_strTallyDatabase;				//the database switch to use for tally names
	//this is always 1 if shown
	int m_intDestDatabase;				//the database switch to use for the dest name
	//this is always 7
	int m_intDestInstanceDatabase;		//the database switch to use for the dest instance name
	
	int m_intCurrentSource;				//current source routed to this dest
	int m_intCurrentSourceRight;		//current source routed to the associated "Right" dest

	int m_intLastSource;				//last source routed to this dest
	int m_intLastSourceRight;			//last source routed to the associated "Right" dest

	bncs_string m_strStatesheetDestSelected;	//
	bncs_string m_strStatesheetDestDeselected;	//

	bncs_string m_strRouterInstanceType;	//holds the instance type of the configured router
	
	//Internal methods
	void start();				//called by the parent via a parantCallback "method"
	bncs_string getCompositeSourceTally(int intSource);
	char getFirstChar(const bncs_string& strName);		//returns the first char of a string
	void routeStereo(int intSource);		//makes the appropriate route to a stereo destination
	void updateTallyMonoStereo();		//custom tally label handler for stereo destination
	void makeRoute(int intSource,bncs_string strMode);		//make a route

	bncs_string m_strDestType;	//holds the type of the dest "-1,0,M,L,R,1,2,3,4,5,6,7,8"
};


#endif // router_dest_INCLUDED