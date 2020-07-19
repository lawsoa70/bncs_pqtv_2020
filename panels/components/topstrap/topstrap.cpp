#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_stringlist.h>
#include <bncs_config.h>
#include "topstrap.h"

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( topstrap )

const int PNL_MAIN(1);

const bncs_string LAYOUT_DEFAULT("default");

const bncs_string AREA_XML("ops_areas");

// constructor - equivalent to ApplCore STARTUP
topstrap::topstrap( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	//Init Vars
	m_strLayout = LAYOUT_DEFAULT;

	//Set area from workstation settings
	setArea( getWorkstationSetting("ops_area") );

	//timerStart(TIMER_SET_AREA, TIMEOUT_SET_AREA);

	// you may need this call to set the size of this component 
	//  if it's used in a popup window 
//	setSize( 1024,668 );		// set the size explicitly
}

// destructor - equivalent to ApplCore CLOSEDOWN 
topstrap::~topstrap()
{
}

// all button pushes and notifications come here
void topstrap::buttonCallback( buttonNotify *b )
{

}

// all revertives come here
int topstrap::revertiveCallback( revertiveNotify * r )
{


	return 0;
}

// all database name changes come back here
void topstrap::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string topstrap::parentCallback( parentNotify *p )
{
	
	return "";
}

// timer events come here
void topstrap::timerCallback( int id )
{

}

void topstrap::setArea(bncs_string strArea)
{
	m_strArea = strArea;
	m_strLayout = "";

	bncs_config c(bncs_string("%1.%2").arg(AREA_XML).arg(m_strArea));
	if(c.isValid())
	{
		while(c.isChildValid())
		{
			if(c.childAttr("id") == "topstrap_layout")
			{
				m_strLayout = c.childAttr("value");
				break;
			}
			c.nextChild();
		}
	}

	if(panelShow(1,bncs_string("layouts/%1.bncs_ui").arg(m_strLayout)))
	{
		panelShow(1,bncs_string("layouts/%1.bncs_ui").arg(LAYOUT_DEFAULT));
	}

	debug("topstrap::setArea() area=%1 layout=%2", m_strArea, m_strLayout);


}


bncs_string topstrap::getSetting(bncs_string id, bncs_string parameter)
{
	bncs_string strReturn = bncs_config ( bncs_string("%1.%2").arg(id).arg(parameter) ).attr("value");
	debug("topstrap::getSetting() %1.%2 -> %3", id, parameter, strReturn);

	//<gallery_floor>																																										
	//	<object id="gallery_1" >
	//		<setting id="dest_wallbox_1" value="rtr_sdi_salt,1" />


	return strReturn;
}
