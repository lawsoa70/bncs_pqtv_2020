#ifndef router_list_INCLUDED
	#define router_list_INCLUDED

#include <bncs_script_helper.h>
#include <map>

#ifdef WIN32
	#ifdef DOEXPORT_SCRIPT
		#define EXPORT_SCRIPT __declspec(dllexport) 
	#else
		#define EXPORT_SCRIPT __declspec(dllimport) 
	#endif
#else 
	#define EXPORT_SCRIPT
#endif
 
class router_list : public bncs_script_helper
{
public:
	router_list( bncs_client_callback * parent, const char* path );
	virtual ~router_list(){};

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
private:
	int m_ExtraDatabase;
	void displayListbox(bool bSortAZ, bool bFilter, bncs_string m_inputSearch);
	bool m_bSortAZ;
	bool m_bFilter;
	bool m_blnShowUnusedPorts;
	void updateListbox(bncs_string index, bncs_string newText, bool bSortAZ);
	bncs_string m_instance;
	bncs_string m_panel;
	bncs_string m_inputSearch;
	int m_database;
	int m_start;
	int m_end;
	int m_device;
	int m_indexOffset;

	bool m_rangeAll;
	void load( void );
	multimap<bncs_string,bncs_string> m_mSortedAZ;
	multimap<bncs_string,bncs_string> m_mSortedIndex;
};


#endif // router_list_INCLUDED