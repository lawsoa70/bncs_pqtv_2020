#ifndef router_mv_quad_INCLUDED
	#define router_mv_quad_INCLUDED
#pragma warning (disable:4786)
#include <map>

#include <bncs_script_helper.h>
#include "Multiviewer.h"

#ifdef WIN32
	#ifdef DOEXPORT_SCRIPT
		#define EXPORT_SCRIPT __declspec(dllexport) 
	#else
		#define EXPORT_SCRIPT __declspec(dllimport) 
	#endif
#else 
	#define EXPORT_SCRIPT
#endif

typedef map<int, int> MAP_INT_INT;
typedef map<int, int>::iterator MAP_INT_INT_ITERATOR;

typedef map<int, bncs_string> MAP_INT_STRING;
typedef map<int, bncs_string>::iterator MAP_INT_STRING_ITERATOR;

typedef map<int, CMultiviewer*> MAP_INT_MULTIVIEWER;
typedef map<int, CMultiviewer*>::iterator MAP_INT_MULTIVIEWER_ITERATOR;

#define PNL_MAIN		1
	
#define PANEL_DEFAULT	"default"

#define MAX_MV_COUNT	99

#define MAX_INPUTS		4
	
class router_mv_quad : public bncs_script_helper
{
public:
	router_mv_quad( bncs_client_callback * parent, const char* path );
	virtual ~router_mv_quad();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );

private:

	MAP_INT_INT m_mapSourceMVOutput;	//map to look up which multiviewer output is on a source
	MAP_INT_STRING m_mapDestMVInput;	//map to look up which multiviewer input is on a dest
	MAP_INT_MULTIVIEWER	m_mapMultiviewers;	//map to hold multiviewer objects

	bncs_string m_strRouterInstance;	//the name of the router instance
	bncs_string m_strPanel;				//the name of the panel layout to display
	int m_intDestIndex;					//the index of the destination
	int m_intRouterDevice;				//the dev id of the router
	bncs_string m_background;			//background colour from the parent
	bncs_string m_strStudio;			//tv studio id

	int m_intCurrentSource;				//last source routed to this dest
	int m_intCurrentMultiviewer;

	bncs_stringlist m_sltQuadCurrentSource;		//temp storage
	bool m_blnLabelsVisible;			//flag to hold if labels are visible

	//Internal methods
	void start();				//called by the parent via a parantCallback "method"
	bncs_string getCompositeSourceTally(int intSource);

	void setCurrentMultiviewer(int intMultiviewer);
	void initMultiviewers();
	void updateTallyDisplay(int intMultiviewer);

};


#endif // router_mv_quad_INCLUDED