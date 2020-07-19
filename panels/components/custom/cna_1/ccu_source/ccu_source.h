#ifndef ccu_source_INCLUDED
	#define ccu_source_INCLUDED

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
 
class ccu_source : public bncs_script_helper
{
public:
	ccu_source( bncs_client_callback * parent, const char* path );
	virtual ~ccu_source();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
	
private:
	bncs_string m_instance;
	bncs_string m_strCcuName;
	map <bncs_string, bncs_string> m_rcpMap;
	int m_intCcuDevice;
	int m_intCcuOffset;
	int m_intCcuStatusSlot;
	int m_intRcpsListSlot;
	boolean m_blnStatusAvailable;
	bncs_string m_strStatesheetAvailable;
	bncs_string m_strStatesheetNotAvailable;

	//Internal methods
	void start();				//called by the parent via a parantCallback "method"

};


#endif // ccu_source_INCLUDED