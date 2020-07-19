/* bncs_auto_helper.cpp - Helper functions for v4.5 Automatics */

	#include "bncs_auto_helper.h"

//	#include "stdafx.h"

/* TODO:	
1. Add DOXYGEN tags
2. Add implementation for getInstanceSlot
3. Add implementation for gpiPoll
4. Add implementation for gpiQuery
5. Add implementation for gpiSet
6. Add implementation for infoQuery
7. Add implementation for routerQuery
*/




/*
Function:
	getDevSlot
Description:
	get device, slot and type for a instance, parameter pair

Parameters:
	instance  The name of the instance  
	param  The name of the parameter  
	*dev  Pointer to an integer to return the device number  
	*slot  Pointer to an integer to return the slot number  
	*type  Pointer to an character to return the device type, for example 'I' for infodriver  

Returns:
	true if all OK, otherwise false

Example:
        int iDev, iSlot;
        char cType;

        getDevSlot( "ARC1","Input", & iDev, & iSlot, & cType);  
        // iDev, iSlot and cType will now contain correct information.
        debug("Arc 1 ddev=%1, slot=%2, type=%3",iDev, iSlot, cType);
*/

//NEW not ready
bool getDevSlot  ( const bncs_string& strInstance, const bncs_string& strParam,  
	int* intDev, int* intSlot, char* szType)  
{
	bncs_config cfg(bncs_string("instances.%1").arg(strInstance));
	if(cfg.isValid())
	{
		bncs_stringlist sltRef = cfg.attr("ref");
		int intOffset = sltRef.getNamedParam("offset").toInt();
		int intParamSlot = 0; 
		//TODO
		// get a warning on this line in compiler  -----   return intOffset + intParamSlot;	
		return true;
	}
	else 
	{
		return false;
	}
}

int getInstanceDevice(const bncs_string& strInstance)
{
	bncs_config cfg(bncs_string("instances.%1").arg(strInstance));
	if(cfg.isValid())
	{
		bncs_stringlist sltRef = cfg.attr("ref");
		return sltRef.getNamedParam("device").toInt();
	}
	else 
	{
		return -1;
	}
}

//NEW not ready
int getDevSlot(const bncs_string& strInstance, bncs_string& strParam )
{
	bncs_config cfg(bncs_string("instances.%1").arg(strInstance));
	if(cfg.isValid())
	{
		bncs_stringlist sltRef = cfg.attr("ref");
		int intOffset = sltRef.getNamedParam("offset").toInt();
		int intParamSlot = 0; //TODO
		return intOffset + intParamSlot;	
	}
	else 
	{
		return -1;
	}
}
/*
Function:
	getObjectSetting

Description:
	Return named setting for this object (from object_settings.xml) 

Parameters:
	strObject  the name of the object to get information about  
	strSetting  name of setting to return  

Returns:
	object setting or null string if not found
        bncs_string strSetting = getObjectSetting( "myObject", "mySetting" );  
        // s contains the "mySetting" setting of the "myObject" object
*/
bncs_string getObjectSetting(const bncs_string& strObject, const bncs_string& strSetting)
{
	bncs_config cfg(bncs_string("object_settings.%1.%2").arg(strObject).arg(strSetting));
	bncs_string strValue = "";
	if(cfg.isValid())
	{
		strValue = cfg.attr("value");
	}
	return strValue;
}

/* 
Function:
	getRouterSize
Description:
   	Return the size of a device from its device settings file 

Parameters:
	intDevice  	The device number  
	intSwitch	The database number (0 or 1)  

Returns:
	the size of the database
        
Example:        
        int intSize = getRouterSize( 123, 1 );                
        // intSize contains the number of destinations in the database for device 123

*/
int getRouterSize(int intDevice, int intSwitch) 
{
	char szFileName[32];
	char szKey[32];
	sprintf(szFileName, "dev_%03d.ini", intDevice);
	sprintf(szKey, "DatabaseSize_%d", intSwitch);
	int intSize = 0;
	// to do ---- sort out declaration of r_p      atoi(r_p(szFileName, "Database", szKey, "0", TRUE));
	return intSize;
}

/*
Function:
	getWorkstationSetting

Description:
	Return named setting for this workstation 

Parameters:
	strSetting  	name of setting to return  

Returns:
	workstation setting or null string if not found

Example:        
        bncs_string strSetting = getWorkstationSetting( "intSDIDestMon" );      
        // strSetting contains the "intSDIDestMon" setting
*/


/*  ALL REST COMMENTED OUT UNTIL THEY WORK -- -- PAW



bncs_string getWorkstationSetting(const bncs_string& strSetting)
{
	//TODO check workstation() call is correct
	////// correct this line   ////// bncs_config cfg(bncs_string("workstation_settings.%1.%2").arg(workstation()).arg(strSetting));
	bncs_string strValue = "";
	if(cfg.isValid())
	{
		strValue = cfg.attr("value");
	}
	return strValue;
}

//void gpiPoll(int intDevice, int intMin, int intMax){}
//void gpiQuery(int intDevice, int intMin, int intMax){}
//void gpiSet(int intDevice, int intIndex, int intValue){}


void infoPoll(int intDevice, int intMin, int intMax)
{
	bncs_string strCmd = bncs_string("IP %1 %2 %3").arg(intDevice).arg(intMin).arg(intMax);
	ec.txinfocmd(strCmd);
}

//void infoQuery(int intDevice, int intMin, int intMax){}
void infoWriteInt(int intDevice, int intValue, int intIndex)
{
	bncs_string strCmd = bncs_string("IW %1 '%2' %3").arg(intDevice).arg(intValue).arg(intIndex);
	ec.txinfocmd(strCmd);
}

void infoWriteString(int intDevice, const bncs_string& strValue, int intIndex)
{
	bncs_string strCmd = bncs_string("IW %1 '%2' %3").arg(intDevice).arg(strValue).arg(intIndex);
	ec.txinfocmd(strCmd);
}

void routerCrosspoint(int intDevice, int intSource, int intDest, const bncs_string& strMask)
{
	bncs_string strCmd = bncs_string("RC %1 %2 %3").arg(intDevice).arg(intSource).arg(intDest).arg(strMask);
	ec.txinfocmd(strCmd);
}

void routerPoll(int intDevice, int intMin, int intMax)
{
	bncs_string strCmd = bncs_string("RP %1 %2 %3").arg(intDevice).arg(intMin).arg(intMax);
	ec.txinfocmd(strCmd);
}

//void routerQuery(int intDevice, int intMin, int intMax){}

  */