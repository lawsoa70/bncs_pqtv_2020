// LastSceRouteHandler.cpp: implementation of the LastSceRouteHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "LastSceRouteHandler.h"
#include "bncs_config.h"
#include "bncs_string.h"
#include "bncs_stringlist.h"
#include "bncs_script_helper.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LastSceRouteHandler::LastSceRouteHandler()

{
		bncs_config c( "object_settings.Last_Known_Source" );
		
	if( c.isValid())
	{
		while( c.isChildValid())
		{
			int intSettingID = c.childAttr( "id" );
			bncs_stringlist sl( c.childAttr( "value" ));
			//expecting value to contain "instance,dest"  e.g. ""vision_rtr",100"
		
	
			if( sl.count() == 2)
			{

				bncs_string sRouterInstanceName= sl[ 0];
				bncs_string sRouterInstance=getDeviceNo((sRouterInstanceName));

				bncs_string sRouterAndDest=(sRouterInstance + "," + sl[1]);
				DestOnRouter[intSettingID]=sRouterAndDest;
				bncs_string strTest = DestOnRouter[intSettingID];
				int iSize=DestOnRouter.size();
			}
			c.nextChild();
		}
	}

}

LastSceRouteHandler::~LastSceRouteHandler()
{

}


// simple helper to return the device number for the given instance 
int LastSceRouteHandler::getDeviceNo( const bncs_string & instance )
{
	bncs_config c( "instances." + instance );

	if( c.isValid())
	{
		bncs_stringlist sl = c.attr( "ref" );
		return sl.getNamedParam( "device" );
	}
	return 0;
}

//this function saves the value in the destination number of db3 of the device
//if indeed it is itemised in object settings.

bool LastSceRouteHandler::CheckLastSce(int iDevice, int iDest){
int iCounter=1;
bncs_string sDevAndIndex;
ITDESTONROUTER itDestOnRouter;
	
	bncs_string sDetails = bncs_string(iDevice);

	sDetails.append(",");
	sDetails.append(iDest);
	
	int iMapSize=DestOnRouter.size();

	itDestOnRouter=DestOnRouter.begin();
	while(itDestOnRouter!=DestOnRouter.end())
	{
		sDevAndIndex=itDestOnRouter->second;
		if(sDetails == sDevAndIndex){

		return true;
		}
		itDestOnRouter ++;
		
	}




	
	return false;

}
