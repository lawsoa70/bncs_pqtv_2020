#ifndef audio_dest_INCLUDED
	#define audio_dest_INCLUDED

#pragma warning(disable:4786)
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
 
#include <audio_common.h>
#include <map>
#include <queue>
using namespace std;

class audio_dest : public bncs_script_helper
{
public:
	audio_dest( bncs_client_callback * parent, const char* path );
	virtual ~audio_dest();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );

private:
	bncs_string m_panelDynamic;
	bool m_dynamicTallies;
	bncs_string m_instance;
	bncs_string m_instanceLock;
	bncs_string m_alias;
	bncs_string m_panel;

	int m_source;
	int m_globalDest;
	int m_dest;
	int m_deviceSource;
	int m_deviceDest;
	int m_deviceLockBase;
	int m_deviceLock;
	int m_destDeviceOffset;

	bool m_force51Stereo;

	portType m_destType;
	portType m_sourceType;

	bool m_initiallySelected;
	bool m_selected;

	int m_tallies[ 6 ];
	bncs_stringlist m_defaultSourceList;

	map< int, int > m_lockStates;
	queue< DWORD > m_undo;
	bool m_blnReadOnly;
	void checkEnableState( void );
	void destButtonEvent( void  );
	void doRoute( int source, int dest );
	void clearUndoQueue( void );
};



#endif // audio_dest_INCLUDED