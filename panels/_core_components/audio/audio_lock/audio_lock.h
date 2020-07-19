#ifndef lock_INCLUDED
	#define lock_INCLUDED

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

class audio_lock : public bncs_script_helper
{
public:
	audio_lock( bncs_client_callback * parent, const char* path );
	virtual ~audio_lock();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
private:
	int m_device;
	int m_deviceLock;
	int m_globalIndex;
	int m_state;
	bncs_string m_instance;
	bncs_string m_instanceLock;
	int m_deviceLockBase;
	int m_destDeviceOffset;
	int m_index;
	int m_deviceAudioBase;
	map< int, int > m_lockStates;
	portType m_destType;
	void setLockState(int);
	void checkEnableState(void);
};


#endif // lock_INCLUDED