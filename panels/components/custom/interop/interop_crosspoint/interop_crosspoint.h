#ifndef interop_crosspoint_INCLUDED
	#define interop_crosspoint_INCLUDED

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

#define MASTER_COUNT                   20
#define SLAVE_COUNT                    50
#define MASTER_LOCAL_STATE_TABLE      100
#define MASTER_OUTPUT_STATE_TABLE     200
#define MASTER_BLUE_TABLE             300
#define MASTER_RED_TABLE              400
#define SLAVE_BLUE_TABLE              500
#define SLAVE_RED_TABLE               600
#define MASTER_MASTER_MATRIX_TABLE   1000
#define MASTER_SLAVE_MATRIX_TABLE    2000
 
class interop_crosspoint : public bncs_script_helper
{
public:
	interop_crosspoint( bncs_client_callback * parent, const char* path );
	virtual ~interop_crosspoint();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
	
private:
	bncs_string m_myParam;
	bncs_string m_instance;
	int m_intDevice;
	bncs_string m_strDestinationType;
	int m_intSourceIndex;
	int m_intDestinationIndex;
	boolean m_boolCrosspoint;
	int m_intSlot;

	//Internal methods
	int getMasterMasterMatrixSlot(int, int);
	int getMasterSlaveMatrixSlot(int, int);

	boolean getRangeInUse(int, int, int &, int &);
};


#endif // interop_crosspoint_INCLUDED