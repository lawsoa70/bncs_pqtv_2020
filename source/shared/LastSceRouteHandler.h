// LastSceRouteHandler.h: interface for the LastSceRouteHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LASTSCEROUTEHANDLER_H__2632D890_4D1A_45B2_81D8_1F362E3B4432__INCLUDED_)
#define AFX_LASTSCEROUTEHANDLER_H__2632D890_4D1A_45B2_81D8_1F362E3B4432__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#pragma warning(disable:4786)


#include <map>
using namespace std;
#include "bncs_string.h"
//#include "bncs_script_helper.h"
typedef map<int,bncs_string> ::iterator ITDESTONROUTER;

class LastSceRouteHandler  
{
public:
	LastSceRouteHandler();
	virtual ~LastSceRouteHandler();
	bool CheckLastSce(int iDevice,  int iDest);


private:
	map<int, bncs_string> DestOnRouter;
	int getDeviceNo( const bncs_string & instance );
	//map<int,bncs_string> ::iterator itDestOnRouter;


};

#endif // !defined(AFX_LASTSCEROUTEHANDLER_H__2632D890_4D1A_45B2_81D8_1F362E3B4432__INCLUDED_)
