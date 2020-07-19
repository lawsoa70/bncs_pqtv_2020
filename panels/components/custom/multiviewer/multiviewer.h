#ifndef multiviewer_INCLUDED
	#define multiviewer_INCLUDED

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
 
class multiviewer : public bncs_script_helper
{
public:
	multiviewer( bncs_client_callback * parent, const char* path );
	virtual ~multiviewer();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
	
private:
	bncs_string m_strInstance;		// which multiviewer e.g. studio_b_mv_1
	bncs_string m_strRouter;		// e.g. rtr_stb_sdi
	int m_intRouterDev;				// router device number
	bncs_string m_strMvLayout;		// multiviewer screen layout e.g. 12_way
	bncs_string m_strSelectedTile;	// the id of the selected tile e.g. tile_6
	int m_intDestDatabase;
	int m_intDestInstanceDatabase;
	int m_intTallyDatabase;
	int m_intDestCount;				// number of inputs even if they're not all used in some layouts
	int m_intFirstDest;				// index of first input dest
	//bncs_string m_strArea;		// e.g. studio_b

	void startInstance();
};


#endif // multiviewer_INCLUDED