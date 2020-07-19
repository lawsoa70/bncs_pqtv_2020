	#if !defined(AFX_BNCS_HELPER_H)
	#define AFX_BNCS_HELPER_H

	#if _MSC_VER > 1000
	#pragma once
	#endif // _MSC_VER > 1000
	#pragma warning(disable : 4786 4996)
	//
	// colledia includes
	#include <bncs_string.h>
	#include <bncs_stringlist.h>
	#include <bncs_config.h>

//TODO include bncs_string and bncs_config

//bool getDevSlot  ( const bncs_string& strInstance, const bncs_string& strParam, int* intDev, int* intSlot, char* szType); 
int getInstanceDevice(const bncs_string &strInstance);
//int getInstanceSlot(const bncs_string& strInstance, bncs_string& strParam );
bncs_string getObjectSetting(const bncs_string& strObject, const bncs_string& strSetting);
int getRouterSize(int intDevice, int intSwitch);
bncs_string getWorkstationSetting(const bncs_string& strSetting);

//void gpiPoll(int intDevice, int intMin, int intMax);
//void gpiQuery(int intDevice, int intMin, int intMax);
//void gpiSet(int intDevice, int intIndex, int intValue);

void infoPoll(int intDevice, int intMin, int intMax);
//void infoQuery(int intDevice, int intMin, int intMax);
void infoWriteInt(int intDevice, int intValue, int intIndex);
void infoWriteString(int intDevice, const bncs_string& strValue, int intIndex);

void routerCrosspoint(int intDevice, int intSource, int intDest, const bncs_string& strMask);
void routerPoll(int intDevice, int intMin, int intMax);
//void routerQuery(int intDevice, int intMin, int intMax);

#endif // !defined(BNCS_HELPER_H)
