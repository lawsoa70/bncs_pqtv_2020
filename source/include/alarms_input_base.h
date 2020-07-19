// alarms_input_base.h: interface for the alarms_input_base class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALARMS_INPUT_BASE_H__5B5DE69F_EB80_4A75_A553_B36E33A9F586__INCLUDED_)
#define AFX_ALARMS_INPUT_BASE_H__5B5DE69F_EB80_4A75_A553_B36E33A9F586__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class QDateTime;
class QStringList;

//! interface for the alarms_input_base class.

/*!
	This abstract base class provides the function protptypes for the input modules of the alarm system.
	\n
	All alarm acquisition modules will be expected to implement the pure virtual functions listed.
	The getData() function is crucial and may be called up to 50 times a second. It must provide the latest states of the inputs and the length of this data.
*/

class alarms_input_base  
{
public:

	/*! Return the Tx count to be displayed on the main application.*/
	virtual long getTxCount(){ return 0;};
	/*! Return the Rx count to be displayed on the main application. */
	virtual long getRxCount(){ return 0;};
	/*! This is called by the main application (in the main thread) each time it requires the current state of the inputs. This during the main calculation loop. This function may be called up to 50 times a second.
	\param data is a pointer to a pointer to the data - use this to point to the data array
	\param length is a pointer to the length of the data - use this to point to the length
	\return Return true if successful, otherwise false*/
	virtual bool getData(int **data, int* length)=0;
	/*! This is called to enquire if the module is OK
	\return Return true if module is OK, false if there are errors.*/
	virtual bool getStatusOk()=0;
	/*! This is called before the destructor, giving the module a chance to notify anything else that it is about to close.*/
	virtual void aboutToClose(){};
	/*! \return Return the QDateTime of the configuration file that has been loaded.*/
	virtual QDateTime getConfigFileDate()=0;
	/*! \Return Return the class name.*/
	virtual const char* getClassName()=0;
	/*! \param bDebugOn sets the debug mode
	\return Return the subsequent debug mode.*/
	virtual bool setDebugMode(bool)=0;
	/*! \return Return the current debug mode.*/
	virtual bool getDebugMode()=0;
	/*! init is called when the module needs to load it's configuation file, the module should also do any intialisation it needs
	to do at this point.
	\param sFileName points the the configuration file to load.
	\return Return true if initialised OK, otherwise false.
	*/
	virtual bool init(char* sFileName)=0;
	alarms_input_base(){};
	virtual ~alarms_input_base(){};
	/*! To allow the module to have messages displayed on the main application this function must return any messages that need displaying.
	This function is called in the main aplications thread, please be aware that this may be an issue.
	\return QStringList of messages.*/
	virtual QStringList getLastLogs()=0;

};

#endif // !defined(AFX_ALARMS_INPUT_BASE_H__5B5DE69F_EB80_4A75_A553_B36E33A9F586__INCLUDED_)
