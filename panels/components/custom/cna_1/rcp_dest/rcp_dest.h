#ifndef rcp_dest_INCLUDED
	#define rcp_dest_INCLUDED

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

#define PNL_MAIN		1

#define TIMER_SETUP	1
#define TIMER_INITIALLY_SELECTED	2
#define TIMER_DEST_INDEX			3

class rcp_dest : public bncs_script_helper
{
public:
	rcp_dest( bncs_client_callback * parent, const char* path );
	virtual ~rcp_dest();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
	
private:
	//bncs_string m_myParam;
	bncs_string m_instance; //e.g. "cna_1/rcp/01"
	bncs_string m_strRcpName; //e.g. "Gall C|RCP 1"
	int m_intRcpDevice;
	int m_intRcpOffset;
	int m_intRcpStatusSlot;
	int m_intRcpCameraSlot;
	boolean m_blnSelected;
	bncs_string m_strCcuInstance; //e.g. "cna_1/cam/01"
	bncs_string m_strCcuName; //e.g. "STC|CAM 1"
	boolean m_blnStatusAvailable;
	bncs_string m_strStatesheetDestSelected;
	bncs_string m_strStatesheetDestDeselected;
	bncs_string m_strStatesheetAvailable;
	bncs_string m_strStatesheetNotAvailable;

	//Internal methods
	void start();				//called by the parent via a parantCallback "method"
};


#endif // rcp_dest_INCLUDED