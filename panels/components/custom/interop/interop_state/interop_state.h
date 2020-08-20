#ifndef interop_state_INCLUDED
	#define interop_state_INCLUDED

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
 
#define PNL_MAIN	1
#define PANEL_DEFAULT	"default"
#define TIMER_SETUP	1
#define TIMER_LOCAL_FLASH_OFF 2
#define TIMER_LOCAL_FLASH_ON  3
#define TIMER_EXTERNAL_FLASH_OFF 4
#define TIMER_EXTERNAL_FLASH_ON  5
#define TIMER_OUTPUT_FLASH_OFF 6
#define TIMER_OUTPUT_FLASH_ON  7

#define MASTER_COUNT                   20
#define SLAVE_COUNT                    50

#define MASTER_LOCAL_STATE_TABLE      100
#define MASTER_EXTERNAL_STATE_TABLE   200
#define MASTER_OUTPUT_STATE_TABLE     300
#define MASTER_BLUE_TABLE             500
#define MASTER_RED_TABLE              600
#define SLAVE_BLUE_TABLE              700
#define SLAVE_RED_TABLE               800
#define ALLOW_EXTERNAL_TABLE          900
#define MASTER_MASTER_MATRIX_TABLE   1000
#define MASTER_SLAVE_MATRIX_TABLE    2000

class interop_state : public bncs_script_helper
{
public:
	interop_state( bncs_client_callback * parent, const char* path );
	virtual ~interop_state();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
	
private:
	bncs_string m_myParam;
	bncs_string m_instance;
	int m_intDevice;
	int m_intMasterIndex;
	boolean m_boolReadOnly;
	bncs_string m_strPanel;
	bncs_string m_strLocalState;
	bncs_string m_strExternalState;
	bncs_string m_strOutputState;
	boolean m_boolAllowExternal;
	int m_intLocalSlot;
	int m_intExternalSlot;
	int m_intOutputSlot;
	int m_intAllowExternalSlot;

	//Internal methods
	int getMasterLocalStateSlot(int);
	int getMasterExternalStateSlot(int);
	int getMasterOutputStateSlot(int);
	int getMasterAllowExternalSlot(int);
};


#endif // interop_state_INCLUDED