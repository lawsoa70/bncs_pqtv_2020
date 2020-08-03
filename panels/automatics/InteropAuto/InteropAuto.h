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