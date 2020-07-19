#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "router_mv_quad.h"

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( router_mv_quad )

// constructor - equivalent to ApplCore STARTUP
router_mv_quad::router_mv_quad( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	//initialise member vars
	m_intDestIndex = 0;
	m_strRouterInstance = "";
	m_intRouterDevice = 0;
	m_strPanel = PANEL_DEFAULT;
	m_strStudio = getWorkstationSetting("ops_area");
	m_intCurrentMultiviewer = 0;
	m_blnLabelsVisible = true;

	initMultiviewers();

	panelShow( PNL_MAIN, m_strPanel );

}

// destructor - equivalent to ApplCore CLOSEDOWN
router_mv_quad::~router_mv_quad()
{
	//delete all CMultiview objects
	for (MAP_INT_MULTIVIEWER_ITERATOR it = m_mapMultiviewers.begin(); it != m_mapMultiviewers.end(); it++)
	{
		CMultiviewer* mv = it->second;
		delete mv;
	}
	m_mapMultiviewers.clear();
}

// all button pushes and notifications come here
void router_mv_quad::buttonCallback( buttonNotify *b )
{
	//No buttons
}

// all revertives come here
int router_mv_quad::revertiveCallback( revertiveNotify * r )
{
	debug("router_mv_quad::revertiveCallback dev='%1' index='%2' sInfo='%3' info=%4", 
		r->device(), r->index(), r->sInfo(), r->info());
	
	//Handler for the destination's router
	if( r->device() == m_intRouterDevice ) 
	{
		if( r->index() == m_intDestIndex ) 
		{

			//setCurrentMultiviewer(m_mapSourceMVOutput[r->info()]);


			m_intCurrentSource = r->info();
			//check which multiview if any this source relates to
			MAP_INT_INT_ITERATOR ito = m_mapSourceMVOutput.find(m_intCurrentSource);
			if(ito != m_mapSourceMVOutput.end())
			{
				setCurrentMultiviewer(ito->second);
			}
			else
			{
				setCurrentMultiviewer(0);
			}
		}
		else
		{
			//check if this destination is in the mv inputs map
			MAP_INT_STRING_ITERATOR it = m_mapDestMVInput.find(r->index());
			if(it != m_mapDestMVInput.end())
			{
				bncs_stringlist sltMVIndexInput = it->second;
				debug("router_mv_quad::revertiveCallback    *found on mv='%1' input='%2'", 
					sltMVIndexInput[0], sltMVIndexInput[1]);

				int intMultiviewer = sltMVIndexInput[0].toInt();
				int intInput = sltMVIndexInput[1].toInt();

				//set the source tally for this multiviewer
				CMultiviewer* mv = m_mapMultiviewers[intMultiviewer];
				if(mv){
					mv->setInputSource(intInput, r->info());
				}

				//is this multiviewer currently displayed on this monitor
				if(intMultiviewer == m_intCurrentMultiviewer)
				{
					textPut("text", getCompositeSourceTally(r->info()), 
						PNL_MAIN, bncs_string("lblTally_%1").arg(intInput));
				}
			}
		}
	}
	return 0;
}

// all database name changes come back here
void router_mv_quad::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string router_mv_quad::parentCallback( parentNotify *p )
{
	debug("destMon::parentCallback command='%1' value='%2'", p->command(), p->value());

	//Property set commands
	if( p->command() == "instance" )
	{
		m_strRouterInstance = p->value();
		debug("destMon::parentCallback instance=%1", m_strRouterInstance);
	}
	else if( p->command() == "dest_index" )
	{
		m_intDestIndex = p->value().toInt();
		debug("destMon::parentCallback dest_index=%1", m_intDestIndex);

	}
	else if( p->command() == "panel" )
	{
		if(p->value().length() >0){
			m_strPanel = p->value();
		}
		else
		{
			m_strPanel = PANEL_DEFAULT;
		}
		panelDestroy(PNL_MAIN);
		panelShow( PNL_MAIN, bncs_string("%1.bncs_ui").arg(m_strPanel) );
		debug("destMon::parentCallback panel=%1", m_strPanel);

		start();
	}
	else if( p->command() == "background" )
	{
		//textPut("stylesheet", m_strBackgroundStyle, PNL_MAIN, "background");
		textPut("stylesheet",p->value(),1,"lblBackground");
		//textPut("stylesheet", "stc_group_background", 1, "background");
		m_background=p->value();
	}

	//Return Commands
	else if( p->command() == "return" )
	{
		//Called by visual editor to persist settings in bncs_ui
		if(p->value() == "all")
		{
			return  
				bncs_string( "dest_index=" ) + bncs_string(m_intDestIndex) + "\n" +
				bncs_string( "panel=" ) + bncs_string(m_strPanel) + "\n" +
				bncs_string( "background=" ) + m_background;
		
		}

		//Called by parent to learn the components settings
		else if( p->value() == "getSettings" )
		{
			return bncs_string( "getSettings=" )
				+ bncs_string( "instance=" ) + m_strRouterInstance + ","
				+ bncs_string( "dest_index=" ) + bncs_string(m_intDestIndex);
		}
	}
	return "";
}

// timer events come here
void router_mv_quad::timerCallback( int id )
{
}

void router_mv_quad::start()
{
	//Get the device ID for the configured instance
	getDev(m_strRouterInstance, &m_intRouterDevice);

	//Register and poll the monitor destination
	routerRegister(m_intRouterDevice, m_intDestIndex, m_intDestIndex);
	routerPoll(m_intRouterDevice, m_intDestIndex, m_intDestIndex);

	initMultiviewers();

}

bncs_string router_mv_quad::getCompositeSourceTally(int intSource)
{
	bncs_string strTally;	//Contains the static part of the name
	bncs_string strLabel;	//Contains the dynamic part of the name
	routerName(m_intRouterDevice, 0, intSource, strTally);
	routerName(m_intRouterDevice, 2, intSource, strLabel);
	if(strLabel != "---")
	{
		//strTally += " - " + strLabel; got rid of the soft name ADP
		strTally;
	}
	return strTally;
}

void router_mv_quad::initMultiviewers()
{
	//now read in mv_list
	bncs_stringlist sltMvObjects = "";
	for (int intMvIndex = 1; intMvIndex < MAX_MV_COUNT; intMvIndex++)
	{
		bncs_string strMvObject = getObjectSetting(m_strStudio + "_mv_list", intMvIndex);
		if(strMvObject.length())
		{
			sltMvObjects += strMvObject;
			
			//	<object id="stc_mv_1">	
			//		<item id="router" value="stc_sdi"/>
			//		<item id="output" value="121"/>
			//		<item id="input_1" value="25"/>
			//		<item id="input_2" value="26"/>
			//		<item id="input_3" value="27"/>
			//		<item id="input_4" value="28"/>
			//	</object>	

			//get the router instance for this multiviewer
			
			
			//get the output source on which the multiviewer source is presented
			int intSource = getObjectSetting(strMvObject, "output").toInt();

			CMultiviewer* mv = new CMultiviewer(intSource);
			
			m_mapMultiviewers.insert(make_pair(intMvIndex, mv));
			
			//add to static map mapSourceMVOutput key=source_index value=mv_index
			//used when a revertive arrives to update the monitor control
			m_mapSourceMVOutput.insert(make_pair(intSource, intMvIndex));

			for(int intInput = 1; intInput <= MAX_INPUTS; intInput++)
			{
				int intDest = getObjectSetting(strMvObject, bncs_string("input_%1").arg(intInput)).toInt();

				//add to a static map mapDestMVInput key=dest_index value=mv_index,input
				//used when a revertive arrives to update the current tally
				m_mapDestMVInput.insert( make_pair(intDest, bncs_string("%1,%2").arg(intMvIndex).arg(intInput)) );
			
				//Register and poll the mv destinations
				routerRegister(m_intRouterDevice, intDest, intDest, true);
				routerPoll(m_intRouterDevice, intDest, intDest);

			}
		}
		else
		{
			//last mv reached
			break;
		}
	}
	debug("router_mv_quad::initMultiviewers list='%1'", sltMvObjects.toString());
}

void router_mv_quad::setCurrentMultiviewer(int intMultiviewer)
{
	//check if labels were hidden and are now to be shown
	if(intMultiviewer > 0 && !m_blnLabelsVisible)
	{
		//make labels visible
		controlShow(PNL_MAIN, "lblTally_1");
		controlShow(PNL_MAIN, "lblTally_2");
		controlShow(PNL_MAIN, "lblTally_3");
		controlShow(PNL_MAIN, "lblTally_4");
		m_blnLabelsVisible = true;
	}

	//check if labels are shown and are to be hidden
	if(intMultiviewer == 0 && m_blnLabelsVisible)
	{
		controlHide(PNL_MAIN, "lblTally_1");
		controlHide(PNL_MAIN, "lblTally_2");
		controlHide(PNL_MAIN, "lblTally_3");
		controlHide(PNL_MAIN, "lblTally_4");
		m_blnLabelsVisible = false;
	}

	if(intMultiviewer > 0)
	{
		updateTallyDisplay(intMultiviewer);
	}

	//store current multiviewer
	m_intCurrentMultiviewer = intMultiviewer;
}

void router_mv_quad::updateTallyDisplay(int intMultiviewer)
{
	for(int intInput = 1; intInput <= MAX_INPUTS; intInput++)
	{
		
		CMultiviewer* mv = m_mapMultiviewers[intMultiviewer];
		if(mv){
			textPut("text", getCompositeSourceTally(mv->getInputSource(intInput)), 
				PNL_MAIN, bncs_string("lblTally_%1").arg(intInput));
		}
	}
}
