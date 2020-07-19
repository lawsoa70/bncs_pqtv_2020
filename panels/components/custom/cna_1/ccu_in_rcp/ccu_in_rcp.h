#ifndef ccu_in_rcp_INCLUDED
	#define ccu_in_rcp_INCLUDED

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
 
class ccu_in_rcp : public bncs_script_helper
{
public:
	ccu_in_rcp( bncs_client_callback * parent, const char* path );
	virtual ~ccu_in_rcp();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
	
private:
	bncs_string m_instance; //e.g. "cna_1/cam/01"
	bncs_string m_strCcuName; //e.g. "STC|CAM 1"
	bncs_string m_strCcuNumber; // e.g. "1"
	map <bncs_string, bncs_string> m_rcpMap;
	int m_intCcuDevice;
	int m_intCcuOffset;
	int m_intCcuStatusSlot;
	int m_intCcuRcpsListSlot;
	int m_strChuLevelSlot;
	int m_strCcuLevelSlot;
	boolean m_blnStatusAvailable;
	bncs_string m_strStatesheetAvailable;
	bncs_string m_strStatesheetNotAvailable;
	bncs_string m_strParentRcpInstance;
	bncs_string m_strParentRcpNumber;

	//Internal methods
	void start();				//called by the parent via a parantCallback "method"

};


#endif // ccu_in_rcp_INCLUDED