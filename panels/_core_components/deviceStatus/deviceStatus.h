#ifndef deviceStatus_INCLUDED
	#define deviceStatus_INCLUDED

#include <bncs_script_helper.h>
#include <map>

#ifdef WIN32
	#ifdef DOEXPORT_SCRIPT
		#define EXPORT_SCRIPT __declspec(dllexport) 
	#else
		#define EXPORT_SCRIPT 
	#endif
#else 
	#define EXPORT_SCRIPT
#endif
 
class deviceStatus : public bncs_script_helper
{
public:
	deviceStatus( bncs_client_callback * parent, const char* path );
	virtual ~deviceStatus();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
	
private:
	bncs_string m_panel;
	bncs_string m_instance;
	int m_deviceStatusId;
	int m_device;
	static std::map< unsigned int, bncs_string> m_workstations;
};


#endif // deviceStatus_INCLUDED