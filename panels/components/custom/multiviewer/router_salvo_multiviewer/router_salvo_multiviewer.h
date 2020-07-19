#ifndef router_salvo_multiviewer_INCLUDED
	#define router_salvo_multiviewer_INCLUDED

#include <bncs_script_helper.h>
#include "..\..\..\SOURCE\INCLUDE\bncs_string.h"	// Added by ClassView

#ifdef WIN32
	#ifdef DOEXPORT_SCRIPT
		#define EXPORT_SCRIPT __declspec(dllexport) 
	#else
		#define EXPORT_SCRIPT 
	#endif
#else 
	#define EXPORT_SCRIPT
#endif
 
class router_salvo_multiviewer : public bncs_script_helper
{
public:
	router_salvo_multiviewer( bncs_client_callback * parent, const char* path );
	virtual ~router_salvo_multiviewer();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
	
private:
	bncs_string m_strMvInstance;		//The instance of MV e.g. studio_b_mv_4
	bncs_string m_strMemoryInstance;	//The instance of the memory automatic
	int m_intMemoryDevice;				//The device id of the memory automatic
	bncs_string m_strMemoryLayout;		//The name of the layout that the memory was saved from
	bncs_string m_strRouterInstance;	//the name of the router instance e.g. rtr_std_b
	int m_intRouterDevice;				//The device id of the router
	int m_intDestCount;					// number of inputs even if they're not all used in some layouts
	int m_intFirstDest;					// index of first input dest
	//bncs_stringlist m_slDevDestList;	//The list of dev_id,destinations that the layout contains to be used for a recall filter

	bncs_string getCurrentSalvo();
	void updateSalvoList();
};

#endif // router_salvo_multiviewer_INCLUDED