#ifndef InteropAuto_INCLUDED
	#define InteropAuto_INCLUDED

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
 
#define MASTER_DB_NUMBER                2
#define SLAVE_DB_NUMBER                 3

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


class InteropAuto : public bncs_script_helper
{
public:
	InteropAuto( bncs_client_callback * parent, const char* path );
	virtual ~InteropAuto();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );

private:
	bncs_string m_myParam;
	bncs_string m_instance;
	int m_intDevice;
	int m_intMinimumMaster;
	int m_intMaximumMaster;
	int m_intMinimumSlave;
	int m_intMaximumSlave;
	bncs_string m_strMasterOutputState[MASTER_COUNT];
	boolean m_boolMasterRedFlash[MASTER_COUNT];
	boolean m_boolMasterBlue[MASTER_COUNT];
	boolean m_boolMasterRed[MASTER_COUNT];
	boolean m_boolSlaveBlue[SLAVE_COUNT];
	boolean m_boolSlaveRed[SLAVE_COUNT];
	boolean m_boolMasterMasterMatrix[MASTER_COUNT][MASTER_COUNT];
	boolean m_boolMasterSlaveMatrix[MASTER_COUNT][SLAVE_COUNT];
	
	//Internal methods
	void setMasterLocalState(int, bncs_string);
	void setMasterOutputState(int, bncs_string);
	void setMasterBlueSlot(int, int);
	void setMasterRedSlot(int, int);
	void setSlaveBlueSlot(int, int);
	void setSlaveRedSlot(int, int);
	void setMasterMasterMatrixSlot(int, int, int);
	void setMasterSlaveMatrixSlot(int, int, int);

	int getMasterLocalStateSlot(int);
	int getMasterOutputStateSlot(int);
	int getMasterBlueSlot(int);
	int getMasterRedSlot(int);
	int getSlaveBlueSlot(int);
	int getSlaveRedSlot(int);
	int getMasterMasterMatrixSlot(int, int);
	int getMasterSlaveMatrixSlot(int, int);

	boolean getRangeInUse(int, int, int &, int &);
};

#endif // InteropAuto_INCLUDED