//	alarms_output_base.h




#if !defined(AFX_ALARMS_OUTPUT_BASE_H__8956E02E_E1FC_4AC6_94E9_BD18D307F6DE__INCLUDED_)
#define AFX_ALARMS_OUTPUT_BASE_H__8956E02E_E1FC_4AC6_94E9_BD18D307F6DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class QDateTime;
class QStringList;
class Calarm_Process;
class QSemaphore;

#include <list>
using namespace std;

//! interface for the alarms_output_base class.

/*!
	This abstract base class provides the function protptypes for the output modules of the alarm system.
	\n
	All alarm output modules will be expected to implement the pure virtual functions listed. Alarm modules must be writen
	so that they always present the relevant and correct information, without duplication, this may mean that you only 
	send changes to a network/client, there fore you need to remember the last state that you sent.
	\n
	dataReady() is expected to be called up to 50 times a second, if the module does not reply to this within a limited 
	time the module will be assumed to have failed in some way and will not receive any more dataReady() calls.
	
*/
class alarms_output_base  
{
public:
	/*! Return the Tx count to be displayed on the main application. */
	virtual long getTxCount(){ return 0;};	
	/*! Return the Rx count to be displayed on the main application. */
	virtual long getRxCount(){ return 0;};
	/*! This is called each time processing has been done, you are expected to check the data and 
	do what ever is necessary with it.
	\param data is the pointer to the int array of process states.
	\param length is the length of the process state array.
	\return Return true if  OK, otherwise false*/
	virtual bool dataReady(int *data, int length)=0;
	/*! This is called to enquire if the module is OK
	\return Return true if module is OK, false if there are errors*/
	virtual bool getStatusOk()=0;
	/*! This is called before the destructor, giving the module a chance to notify anybody else that it is about to close.*/
	virtual void aboutToClose()=0;
	/*! \return return the QDateTime of the configuration file that has been loaded*/
	virtual QDateTime getConfigFileDate()=0;
	/*! \Return Return the class name*/
	virtual const char* getClassName()=0;
	/*! \param bDebugOn sets the debug mode
	\return Return the subsequent debug mode*/
	virtual bool setDebugMode(bool bDebugOn)=0;
	/*! \return Return the current debug mode*/
	virtual bool getDebugMode()=0;
	/*! Sets the module to Rx only, this is for redundancy, the main application will do any negociation with
	other alarm systems to determine which system is active
	\return Return true if in Rx only mode mode*/
	virtual bool setRxOnly(bool)=0;
	/*! \return Return true if in Rx only mode mode*/
	virtual bool getTxMode()=0;
	/*! init is called when the module needs to load it's configuation file, the module should also do any intialisation it needs
	to do at this point.
	\param sFileName points the the configuration file to load.
	\param sProcs is a list of the proccesses in the order that they will be presented in the state array in dataReady().
	\return Return true if initialised OK, otherwise false.
	*/
	virtual bool init(char* sFileName, QStringList sProcs)=0;
	/*! This function points the module at the list of processes, this allows the module to change the state of a process
	i.e. force a process off, or acknowledge an error, not all modules will need to be able to do this.*/
	virtual void setOverrideList(list <Calarm_Process*>* pProcList,QSemaphore* sProcs) = 0;
	alarms_output_base(){};
	/*! To allow the module to have messages displayed on the main application this function must return any messages that need displaying.
	This function is called in the main apllications thread, please be aware that this may be an issue.
	\return QStringList of messages.*/
	virtual QStringList getLastLogs()=0;
	virtual ~alarms_output_base(){};
	/*! This function is called after initialisation to set the last known state of the processes, do with this what ever needs to be done.
	\param data is the pointer to the int array of process states.
	\param length is the length of the process state array.*/
	virtual void setIntialStates(int *data, int length){return;};
};

#endif // !defined(AFX_ALARMS_OUTPUT_BASE_H__8956E02E_E1FC_4AC6_94E9_BD18D307F6DE__INCLUDED_)
