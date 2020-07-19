#ifndef slinkNetStatus_INCLUDED
	#define slinkNetStatus_INCLUDED

#include <bncs_script_helper.h>
#include <ccClient.h>

#ifdef WIN32
	#ifdef DOEXPORT_SCRIPT
		#define EXPORT_SCRIPT __declspec(dllexport) 
	#else
		#define EXPORT_SCRIPT 
	#endif
#else 
	#define EXPORT_SCRIPT
#endif
 
class slinkNetStatus : public bncs_script_helper, IccClientCallback
{
public:
	slinkNetStatus( bncs_client_callback * parent, const char* path );
	virtual ~slinkNetStatus();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
	
private:
	bncs_string m_name;
	int m_ws;
	bncs_string m_instance;
	int m_device;
	ccClient *m_client;

protected:
	virtual void cccDeviceMessage(int workstation, int device, const string & message, int reference);
};


#endif // slinkNetStatus_INCLUDED