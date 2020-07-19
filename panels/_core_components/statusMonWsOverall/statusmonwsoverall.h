#ifndef statusmonwsoverall_INCLUDED
	#define statusmonwsoverall_INCLUDED

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

#include <vector>
#include <set>
 
class statusmonwsoverall : public bncs_script_helper
{
public:
	statusmonwsoverall( bncs_client_callback * parent, const char* path );
	virtual ~statusmonwsoverall();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
	
private:
	bncs_string m_myParam;

	vector< int > m_workstations;
	set< int > m_ignoreList;
	int m_noWorkstationButtons;
	int m_noPageButtons;

	void loadWorkstations( void );
	void loadPage( int );
	void loadIgnoreList( void );

};


#endif // statusmonwsoverall_INCLUDED