#ifndef audio_take_INCLUDED
	#define audio_take_INCLUDED

#include <bncs_script_helper.h>

#include <map>
using namespace std;

#ifdef WIN32
	#ifdef DOEXPORT_SCRIPT
		#define EXPORT_SCRIPT __declspec(dllexport) 
	#else
		#define EXPORT_SCRIPT __declspec(dllimport) 
	#endif
#else 
	#define EXPORT_SCRIPT
#endif

#include "audio_common.h"

class audio_take : public bncs_script_helper
{
public:
	audio_take( bncs_client_callback * parent, const char* path );

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
private:
	int m_device;
	int m_deviceDest;
	int m_deviceLock;
	int m_deviceLockBase;

	int m_source;
	int m_dest;
	int m_globalDest;
	int m_state;

	bncs_string m_instanceBase;
	bncs_string m_instanceLockBase;
	int m_destDeviceOffset;

	bool m_force51Stereo;

	bool m_undoPossible;

	bncs_string m_mask;

	void checkEnableState( void );

	portType m_sourceType;
	portType m_destType;

	void setDefaultMask( void );

	map< int, int > m_lockStates;
};


#endif // audio_take_INCLUDED