#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "router_dest.h"

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( router_dest )

// constructor - equivalent to ApplCore STARTUP
router_dest::router_dest( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{

	//Initialise Member Vars
	m_blnInitiallySelected = false;
	m_blnStereoDest = false;
	m_blnLockable = false;
	m_blnLockState = false;
	m_strRouterInstance = "";
	m_strRouterInstanceType = "";
	m_intDestIndex = 0;
	m_intLockDevice = 0;
	m_intRouterDevice = 0;
	m_intTallyDatabase = 0;
	m_intLabelDatabase = 2;
	m_intDestDatabase = 1;
	m_intDestInstanceDatabase = 7;
	m_intCurrentSource = 0;
	m_intCurrentSourceRight = 0;
	m_intLastSource = 0;
	m_intLastSourceRight = 0;
	m_intDefaultSource=0;
	m_blnEnabled = true;
	m_blnKeepLastSource = false;

	m_blnSelected = false;

	m_strStatesheetDestSelected = "dest_selected";
	m_strStatesheetDestDeselected = "dest_deselected";

	m_strAlias = "";


	m_strPanel = PANEL_DEFAULT;

	// show the default panel layout on start
	panelShow( PNL_MAIN, bncs_string("%1.bncs_ui").arg(m_strPanel) );

	// you may need this call to set the size of this component 
	//  if it's used in a popup window 
	//	setSize( 1024,668 );		// set the size explicitly
	//	setSize( 1 );				// set the size to the same as the specified panel

}

// destructor - equivalent to ApplCore CLOSEDOWN
router_dest::~router_dest()
{
}

// all button pushes and notifications come here
void router_dest::buttonCallback( buttonNotify *b )
{
	if( b->panel() == PNL_MAIN )
	{
		if( b->id() == "btnDest")
		{
			debug("router_dest::buttonCallback btnDest Released");
			hostNotify("command=selected");
			hostNotify(bncs_string("selected_index=%1").arg(m_intDestIndex) );
		}
		else if( b->id() == "btnLock")
		{
			debug("router_dest::buttonCallback btnLock Released");
			
			//TODO - add toggle infowrite code...
			if(m_blnLockState)
			{
				infoWrite(m_intLockDevice, "0", m_intDestIndex);	
			}
			else
			{
				infoWrite(m_intLockDevice, "1", m_intDestIndex);	
			}
		}
	}
}

// all revertives come here
int router_dest::revertiveCallback( revertiveNotify * r )
{
	debug("router_dest::revertiveCallback dev='%1' index='%2' sInfo='%3' info=%4 type=%5", 
		r->device(), r->index(), r->sInfo(), r->info(), m_strRouterInstanceType);
	
	//Handler for the destination's router
	if( r->device() == m_intRouterDevice ) 
	{
		//A nexus router destination
		if(m_strRouterInstanceType == ROUTER_TYPE_HYDRA)
		{
			//debug("router_dest::revertiveCallback update tally AUDIO");
			if( r->index() == m_intDestIndex ) 
			{
				m_intCurrentSource = r->info();
			}
			else if( r->index() == m_intDestIndex + 1)
			{
				m_intCurrentSourceRight = r->info();
			}
			updateTallyMonoStereo();
		}
		//A normal single level vision destination
		else
		{
			//debug("router_dest::revertiveCallback update tally VIDEO");
			if( r->index() == m_intDestIndex ) 
			{
				m_intCurrentSource = r->info();

				//VISION TALLY
				textPut( "text", getCompositeSourceTally(r->info()), PNL_MAIN, "lblTally" );
			}
		}
		hostNotify(bncs_string("source_index=%1").arg(r->info()));

	}
	//Handler for the destination's lock device
	else if(r->device() == m_intLockDevice)
	{
		//it can only be the dest index
		if(r->index() == m_intDestIndex)
		{
			//Does the lock slot contain exactly "1" meaning locked
			if(r->sInfo().stripWhiteSpace().length() > 0 && r->sInfo() != "0")
			{
				m_blnLockState = true;
				debug("router_dest::revertiveCallback dest %1 is locked", m_intDestIndex);
				if(m_blnLockable){
					textPut("statesheet", "dest_locked", PNL_MAIN, "btnLock");
				}
				else
				{
					textPut( "led", "true", PNL_MAIN, "btnDest");
					textPut( "colour.led", "red", PNL_MAIN, "btnDest");
				}

				if(m_blnSelected)
				{
					hostNotify("locked=1");
				}
			}
			else
			{
				m_blnLockState = false;
				debug("router_dest::revertiveCallback dest %1 is unlocked", m_intDestIndex);
				if(m_blnLockable){
					textPut("statesheet", "dest_unlocked", PNL_MAIN, "btnLock");
				}
				else
				{
					textPut( "led", "false", PNL_MAIN, "btnDest");
				}

				if(m_blnSelected)
				{
					hostNotify("locked=0");
				}
			}
		}
	}
	return 0;
}

// all database name changes come back here
void router_dest::databaseCallback( revertiveNotify * r )
{
	//debug("router_dest::databaseCallback() %1 %2 %3", r->device(), r->database(), r->index());
	if( r->device() ==m_intRouterDevice )
	{
		// source names 0 and 2 only are relevant
		if( (r->database() == 0) || (r->database() == 2)){
			//A nexus router destination
			if(m_strRouterInstanceType == ROUTER_TYPE_HYDRA)
			{
				if ((r->index()==m_intCurrentSource) || ( r->index() == m_intCurrentSource + 1))
				{
					updateTallyMonoStereo();
				}
			}
			//A normal single level vision destination
			else
			{
				//debug("database router=%1 index=%2 info=%3 sInfo=%4", r->device(), r->index(), r->info(), r->sInfo());
				if (r->index()==m_intCurrentSource)
				{
					//VISION TALLY
					textPut( "text", getCompositeSourceTally(r->index()), PNL_MAIN, "lblTally");
				}
			}
		}
		else if( r->database() == 1 && r->index()==m_intDestIndex)
		{
			bncs_string strDestName;
			routerName(m_intRouterDevice, m_intDestDatabase, m_intDestIndex, strDestName);

			if(m_strAlias.length() > 0)
			{
				textPut( "text", m_strAlias, PNL_MAIN, "btnDest" );
			}
			else
			{
				textPut( "text", strDestName, PNL_MAIN, "btnDest" );
			}

			bncs_string strDestInstance;
			routerName(m_intRouterDevice, m_intDestInstanceDatabase, m_intDestIndex, strDestInstance);
			hostNotify("instance=" + strDestInstance);

		}
	}
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string router_dest::parentCallback( parentNotify *p )
{
	debug("router_dest::parentCallback command='%1' value='%2'", p->command(), p->value());

	//Property set commands
	if( p->command() == "instance" )
	{
		m_strRouterInstance = p->value();
		debug("router_dest::parentCallback instance=%1", m_strRouterInstance);
	}
	else if( p->command() == "dest_database" )
	{
		m_intDestDatabase = p->value().toInt();
		debug("router_dest::parentCallback dest_database=%1", m_intDestDatabase);
	}
	else if( p->command() == "dest_instance_database" )
	{
		m_intDestInstanceDatabase = p->value().toInt();
		debug("router_dest::parentCallback dest_instance_database=%1", m_intDestDatabase);
	}
	else if( p->command() == "dest_index" )
	{
		m_intDestIndex = p->value().toInt();
		debug("router_dest::parentCallback dest_index=%1", m_intDestIndex);
		
		//Initialise timer to fire dest_index event that may be received by a router_salvo component
		timerStart(TIMER_DEST_INDEX, 100);
	}
	else if( p->command() == "default_source" )
	{
		m_intDefaultSource = p->value().toInt();
		debug("router_dest::parentCallback source_index=%1", m_intDefaultSource);

	}
	else if( p->command() == "alias" )
	{
		m_strAlias = p->value();
		debug("router_dest::parentCallback alias=%1", m_strAlias);
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
		debug("router_dest::parentCallback panel=%1", m_strPanel);
		textPut("text", bncs_string("Config:|Dest<%1>").arg(m_intDestIndex), PNL_MAIN, "lblTally");

		//For design time assistance show the index and name of the configured dest
		int intRouter;
		getDev(m_strRouterInstance, &intRouter);
		bncs_string strDestName;
		routerName(intRouter, 1, m_intDestIndex, strDestName);
		textPut( "text", bncs_string("%1|Dest %1|On %3")
			.arg(strDestName).arg(m_intDestIndex).arg(m_strRouterInstance), PNL_MAIN, "btnDest" );
	}

	else if( p->command() == "tally_database" )
	{
		m_intTallyDatabase = p->value().toInt();
		debug("router_dest::parentCallback tally_database=%1", m_intTallyDatabase);
		start();
	}

	else if (p->command() == "label_database")
	{
		m_intLabelDatabase = p->value().toInt();
		debug("router_dest::parentCallback label_database=%1", m_intLabelDatabase);
		start();
	}

	else if (p->command() == "initially_selected")
	{
		//This property should be set to true when a single destination is used on a panel
		// it will send a selected_index event when the timer fires
		if(p->value().lower() == "true")
		{
			m_blnInitiallySelected = true;
			timerStart(TIMER_INITIALLY_SELECTED, 50);
		}
		else
		{
			m_blnInitiallySelected = false;
		}
	}

	//Runtime "methods"
	else if( p->command() == "start" )
	{
		start();
	}
	else if( p->command() == "select" )
	{
		debug("router_dest::parentCallback select");
		if(p->value() == "this")
		{
			debug("router_dest::parentCallback select button");
			hostNotify("command=selected");
			hostNotify(bncs_string("selected_index=%1").arg(m_intDestIndex) );			
		}
		else if(p->value().toInt() == m_intDestIndex)
		{
			if(!m_blnSelected)
			{
				textPut("statesheet", m_strStatesheetDestSelected, PNL_MAIN, "btnDest");
				m_blnSelected = true;
				
				//Notify current lock state
				if(m_blnLockState)
				{
					hostNotify("locked=1");
				}
				else
				{
					hostNotify("locked=0");
				}

			}
		}
		else
		{
			if(m_blnSelected)
			{
				textPut("statesheet", m_strStatesheetDestDeselected, PNL_MAIN, "btnDest");
				m_blnSelected = false;
			}
		}
	}
	else if( p->command() == "deselect" )
	{
		debug("router_dest::parentCallback deselect");
		textPut("statesheet", m_strStatesheetDestDeselected, PNL_MAIN, "btnDest");
	}
	else if( p->command() == "route_source" )
	{
		//bncs_stringlist sltParams = bncs_stringlist(p->value());
		//int intSource = sltParams.getNamedParam("index").toInt();
		if(m_blnSelected)
		{
			int intSource = p->value().toInt();
			bncs_string strMode = TAKE_NORMAL;	//sltParams.getNamedParam("mode");
			debug("router_dest::parentCallback route_source index='%1' mode='%2'", intSource, strMode);
			makeRoute( intSource,strMode);
		}

	}
	else if( p->command() == "route" )
	{
		bncs_stringlist sltParams = bncs_stringlist(p->value());
		int intSource = sltParams[0].toInt();
		int intDest = sltParams[1].toInt();
		if(intDest == m_intDestIndex)
		{
			debug("router_dest::parentCallback route source='%1' dest='%2'", intSource, intDest);
			bncs_string strMode = TAKE_NORMAL;
			makeRoute( intSource,strMode);
		}
		
	}
	else if( p->command() == "route_default" )
	{

	
		int intSource=m_intDefaultSource;
		bncs_string strMode=TAKE_NORMAL;
		debug("router_dest::parentCallback route_default index='%1' mode='%2'", intSource, strMode);
		makeRoute(intSource, strMode);

	}
	else if( p->command() == "untake" )
	{
		if(m_blnSelected){

			if(m_intLastSource){
				routerCrosspoint(m_intRouterDevice, m_intLastSource, m_intDestIndex);
			}
			if(m_blnStereoDest && m_intLastSourceRight)
			{
				routerCrosspoint(m_intRouterDevice, m_intLastSourceRight, m_intDestIndex + 1);
			}
		}
	}
	else if( p->command() == "set_lock" )
	{
		int intState = p->value().toInt();
		if(intState == 1)
		{
			infoWrite(m_intLockDevice, "1", m_intDestIndex);	
		}
		else
		{
			infoWrite(m_intLockDevice, "0", m_intDestIndex);	
		}

	}
	else if( p->command() == "disable" )
	{
		if(m_blnEnabled)
		{
			m_blnEnabled = false;
			controlDisable(PNL_MAIN, "btnDest");
			controlDisable(PNL_MAIN, "btnLock");
		}
	}
	else if( p->command() == "enable" )
	{
		if(!m_blnEnabled)
		{
			m_blnEnabled = true;
			controlEnable(PNL_MAIN, "btnDest");
			controlEnable(PNL_MAIN, "btnLock");
		}
	}

	//Called by parent to asyncronously learn the components settings
	else if (p->command() == "getSettings")
	{
		hostNotify( bncs_string("getSettings=")
			+ bncs_string("id=") + id() + ","	// we also need the id to identify where the settings are from
			+ bncs_string("instance=") + m_strRouterInstance + ","
			+ bncs_string("dest_index=") + bncs_string(m_intDestIndex));
	}

	//Return Commands
	else if( p->command() == "return" )
	{
		//Called by visual editor to persist settings in bncs_ui
		if(p->value() == "all")
		{
			return 
				bncs_string( "dest_index=" ) + bncs_string(m_intDestIndex) + "\n" +
				bncs_string( "dest_database=" ) + bncs_string(m_intDestDatabase) + "\n" +
				bncs_string( "dest_instance_database=" ) + bncs_string(m_intDestInstanceDatabase) + "\n" +
				bncs_string( "panel=" ) + bncs_string(m_strPanel) + "\n" +
				bncs_string( "default_source=" ) + bncs_string(m_intDefaultSource) + "\n" +
				bncs_string( "alias=" ) + bncs_string(m_strAlias) + "\n" +
				bncs_string( "tally_database=" ) + bncs_string(m_intTallyDatabase) + "\n" + 
				bncs_string( "label_database=" ) + bncs_string(m_intLabelDatabase) + "\n" +
				bncs_string( "initially_selected=" ) + bncs_string((m_blnInitiallySelected ? "true" : "false")) + "\n";
		}
		//Called by parent to learn the components settings
		else if( p->value() == "getSettings" )
		{
			return bncs_string( "getSettings=" )
				+ bncs_string( "instance=" ) + m_strRouterInstance + ","
				+ bncs_string( "dest_index=" ) + bncs_string(m_intDestIndex);
		}
	}

	//Connections information for BNCS Vis Ed
	else if(p->command() == "_events")
	{
		bncs_stringlist sltEvents;
		sltEvents << "selected_index=<dest_index>";
		sltEvents << "dest_index=<dest_index>";
		sltEvents << "instance=<instance>";
		sltEvents << "source_index=<source index routed to this dest>";
		sltEvents << "command=selected";
		sltEvents << "locked=<locked state 1 or 0>";
		sltEvents << "getSettings=<settings>";
		return sltEvents.toString('\n');
	}
	else if(p->command() == "_commands")
	{
		bncs_stringlist sltCommands;
		sltCommands << "disable";
		sltCommands << "enable";
		sltCommands << "route_source=<source_index>";
		sltCommands << "select=<dest_index>";
		sltCommands << "untake";
		return sltCommands.toString( '\n' );
	}

	return "";
}

// timer events come here
void router_dest::timerCallback( int id )
{
	if(id == TIMER_INITIALLY_SELECTED)
	{
		timerStop(TIMER_INITIALLY_SELECTED);
		hostNotify(bncs_string("selected_index=%1").arg(m_intDestIndex) );
	}
	else if(id == TIMER_DEST_INDEX)
	{
		timerStop(TIMER_DEST_INDEX);
		hostNotify(bncs_string("dest_index=%1").arg(m_intDestIndex));

		bncs_string strDestInstance;
		routerName(m_intRouterDevice, m_intDestInstanceDatabase, m_intDestIndex, strDestInstance);
		hostNotify("instance=" + strDestInstance);
	}
}

void router_dest::start()
{
	//Get the device ID for the configured instance
	getDev(m_strRouterInstance, &m_intRouterDevice);
	//debug("router_dest::start id=%1 index=%2", m_intRouterDevice, m_intDestIndex);

	//Display the router database name
	bncs_string strDestName;
	routerName(m_intRouterDevice, m_intDestDatabase, m_intDestIndex, strDestName);

	if(m_strAlias.length() > 0)
	{
		textPut( "text", m_strAlias, PNL_MAIN, "btnDest" );
	}
	else
	{
		textPut( "text", strDestName, PNL_MAIN, "btnDest" );
	}

	//What type of router has the component been set to work with

	bncs_config cfgInstance = bncs_config(bncs_string("instances.%1").arg(m_strRouterInstance));
	m_strRouterInstanceType = cfgInstance.attr("type");

	if(m_strRouterInstanceType == ROUTER_TYPE_HYDRA)
	{
		//A special router behaviour - nexus mono/stereo/5.1 audio router
		
		
		//bncs_string m_strDestType;	//holds the type of the dest "-1,0,M,L,R,1,2,3,4,5,6,7,8"

		//Next check the destination type
		routerName(m_intRouterDevice, DB_HYDRA_OUT_TYPE, m_intDestIndex, m_strDestType);
		debug("router_dest::start dest '%1' is a Hydra dest of type '%2'", m_intDestIndex, m_strDestType);

		switch(getFirstChar(m_strDestType)){
			case 'L':	//Left of a stereo pair
			case '1':	//Left of an A1/A2 pair
			case '3':	//Left of an A3/A4 pair
			case '5':	//Left of an A5/A6 pair
			case '7':	//Left of an A7/A8 pair
				m_blnStereoDest = true;

				//TEMP ASSUMPTION - the "Right" leg of the stereo pair is on the following destination
				//Register with the router destination AND the following dest for the "Right" leg
				routerRegister(m_intRouterDevice, m_intDestIndex, m_intDestIndex + 1);
				routerPoll(m_intRouterDevice, m_intDestIndex, m_intDestIndex + 1);
				break;
			default:			
				//Only register with the router destination specified
				routerRegister(m_intRouterDevice, m_intDestIndex, m_intDestIndex);
				routerPoll(m_intRouterDevice, m_intDestIndex, m_intDestIndex);
		}
	}
	else
	{
		//A normal single level router

		//Register and poll the destination
		routerRegister(m_intRouterDevice, m_intDestIndex, m_intDestIndex);
		routerPoll(m_intRouterDevice, m_intDestIndex, m_intDestIndex);
	}

	//Check to see if the loaded panel has a lock button
	bncs_string strGetLockText = "";
	textGet("text", PNL_MAIN, "btnLock", strGetLockText);
	if(strGetLockText.length() > 0)
	{
		m_blnLockable = true;
		debug("router_dest::start dest %1 has lock button", m_intDestIndex);
	}

	//Register with the destination's lock slot		
	getDev(m_strRouterInstance + "_lock", &m_intLockDevice);
	if(m_intLockDevice > 0)
	{
		infoRegister(m_intLockDevice, m_intDestIndex, m_intDestIndex);
		infoPoll(m_intLockDevice, m_intDestIndex, m_intDestIndex);
	}

	bncs_string strKeepLastSourceInstance = getObjectSetting(m_strRouterInstance, m_intDestIndex);
	if(strKeepLastSourceInstance.length() > 0)
	{
		int intKeepLastSourceDevice = 0;
		getDev(strKeepLastSourceInstance, &intKeepLastSourceDevice);

		if(	intKeepLastSourceDevice > 0 && intKeepLastSourceDevice < 1000 )
		{
			m_blnKeepLastSource = true;
		}
	}
	debug("router_dest::start dest=%1 KeepLastSourceInstance=%2 KeepFlag=%3", 
		m_intDestIndex, strKeepLastSourceInstance, m_blnKeepLastSource?"true":"false");


}

bncs_string router_dest::getCompositeSourceTally(int intSource)
{
	bncs_config cRouterInstance("instances." + m_strRouterInstance); // a config object for the current router instance
	bncs_string strLocation = cRouterInstance.attr("location"); // the location of the router e.g. "studio_a"
	bncs_stringlist slExternalSources;
	bncs_stringlist slLoopedSources;
	if (strLocation.length() > 0)
	{
		bncs_config cExternalSources("object_settings." + strLocation + "_bfeumdlabel_auto.sources_external");
		bncs_string strExternalSources = cExternalSources.attr("value");
		if (strExternalSources.length() > 0)
		{
			slExternalSources.fromString(strExternalSources);
		}
		bncs_config cLoopedSources("object_settings." + strLocation + "_bfeumdlabel_auto.sources_looped");
		bncs_string strLoopedSources = cLoopedSources.attr("value");
		if (strLoopedSources.length() > 0)
		{
			slLoopedSources.fromString(strLoopedSources);
		}
	}

	bncs_string strTally;	//Contains the static part of the name
	bncs_string strLabel;	//Contains the dynamic part of the name
	
	routerName(m_intRouterDevice, m_intTallyDatabase, intSource, strTally);
	routerName(m_intRouterDevice, m_intLabelDatabase, intSource, strLabel);

	strTally.replace('|', ' ');
	strLabel = strLabel.simplifyWhiteSpace();

	if(strLabel.length() > 0 && strLabel != "---")
	{
		if (slExternalSources.find(bncs_string(intSource)) > -1 ||
			slLoopedSources.find(bncs_string(intSource)) > -1)
		{
			strTally += "|" + strLabel;
		}
	}
	//debug("router_dest::getCompositeSourceTally source=%1 tally=%2", intSource, strTally);
	
	//Fix cases where apostrophe has been defined in a source name
	strTally.replace('\'', '`');

	return strTally;
}

char router_dest::getFirstChar(const bncs_string &strName)
{
	char szName[257];
	strcpy(szName, strName);
	return szName[0];
}

void router_dest::updateTallyMonoStereo()
{
	bncs_string strTally;
	bncs_string strSourceType;
	routerName(m_intRouterDevice, DB_HYDRA_IN_TYPE, m_intCurrentSource, strSourceType);
	if(m_blnStereoDest)
	{
		//Check the source type
		debug("router_dest::updateTallyMonoStereo srce '%1' is of type '%2'", m_intCurrentSource, strSourceType);

		bool blnIsRouteStereoToStereo = false;
		switch(getFirstChar(strSourceType)){
			case 'L':	//Left of a stereo pair
			case '1':	//Left of an A1/A2 pair
			case '3':	//Left of an A3/A4 pair
			case '5':	//Left of an A5/A6 pair
			case '7':	//Left of an A7/A8 pair
				//The "Left" leg of a stereo source has been routed to this dest component

				//Check if the "Right" leg has also been routed
				if(m_intCurrentSourceRight == m_intCurrentSource + 1)
				{
					blnIsRouteStereoToStereo = true;
				}
		}

		if(blnIsRouteStereoToStereo)
		{
			//A stereo to stereo route been made
			//Report the user level source name from DB0 of the "Left"leg
			routerName(m_intRouterDevice, 0, m_intCurrentSource, strTally);
			textPut( "text", strTally, PNL_MAIN, "lblTally" );
		}
		else
		{
			if(m_intCurrentSource == m_intCurrentSourceRight){
				//A stereo dest has the same source routed to both legs

				//Report the single level source name from DB2
				routerName(m_intRouterDevice, 2, m_intCurrentSource, strTally);
				
				if(getFirstChar(strSourceType) != 'M')
				{
					//If the source is anything other than mono the two routes should be displayed
					strTally = strTally + "|" + strTally;
				}
				textPut( "text", strTally, PNL_MAIN, "lblTally" );
			}
			else
			{
				//A split route of some kind has been made
				//Report the single level source name of the "Left" leg from DB2
				bncs_string strTallyRight;	//the singel level name of the "Right" leg
				routerName(m_intRouterDevice, 2, m_intCurrentSource, strTally);
				routerName(m_intRouterDevice, 2, m_intCurrentSourceRight, strTallyRight);
				textPut( "text", bncs_string("%1|%2").arg(strTally).arg(strTallyRight), PNL_MAIN, "lblTally" );
			}
		}
	}
	else
	{
		//MONO
		
		//A single level destination always report the single level source name from DB2
		routerName(m_intRouterDevice, 2, m_intCurrentSource, strTally);
		textPut( "text", strTally.replace('|',' '), PNL_MAIN, "lblTally" );
		if(getFirstChar(strSourceType) != 'M')
		{
			//If the source is anything other than mono the state should be mixed
			textPut( "statesheet", "tally_audio_mixed", PNL_MAIN, "lblTally" );
		}
		else
		{
			textPut( "statesheet", "tally_audio_normal", PNL_MAIN, "lblTally" );
		}
	}
}

void router_dest::routeStereo(int intSource)
{
	//Check the source type
	bncs_string strSourceType;
	routerName(m_intRouterDevice, DB_HYDRA_IN_TYPE, intSource, strSourceType);
	debug("router_dest::routeStereo srce '%1' is of type '%2'", intSource, strSourceType);

	bool blnIsRouteStereoToStereo = false;
	switch(getFirstChar(strSourceType)){
		case 'L':	//Left of a stereo pair
		case '1':	//Left of an A1/A2 pair
		case '3':	//Left of an A3/A4 pair
		case '5':	//Left of an A5/A6 pair
		case '7':	//Left of an A7/A8 pair
			//The "Left" leg of a stereo source is to be routed to this dest component
			blnIsRouteStereoToStereo = true;
			break;
	}

	if(blnIsRouteStereoToStereo)
	{
		//A stereo to stereo route should be made
		routerCrosspoint(m_intRouterDevice, intSource, m_intDestIndex);
		routerCrosspoint(m_intRouterDevice, intSource + 1, m_intDestIndex + 1);
	}
	else
	{
		//A dual mono route should be made
		routerCrosspoint(m_intRouterDevice, intSource, m_intDestIndex);
		routerCrosspoint(m_intRouterDevice, intSource, m_intDestIndex + 1);
	}

}

void router_dest::makeRoute(int intSource, bncs_string strMode){

		if(intSource > 0 && !m_blnLockState)
		{
			//first save last source
			m_intLastSource = m_intCurrentSource;
			if(m_blnStereoDest)
			{
				m_intLastSourceRight = m_intCurrentSourceRight;
			}

			if(m_blnKeepLastSource)
			{
				//Write new source to router device DB3
				routerModify(m_intRouterDevice, 3, m_intDestIndex, bncs_string(intSource), false);
			}

			//TODO move the route handling to a function
			if(strMode.length() == 0 || strMode == TAKE_NORMAL)
			{
				if(m_blnStereoDest)
				{
					routeStereo(intSource);
				}
				else
				{
					//Make single crosspoint
					routerCrosspoint(m_intRouterDevice, intSource, m_intDestIndex);
				}
			}
			//MONO modes
			else if(strMode == TAKE_M_L)
			{
				routerCrosspoint(m_intRouterDevice, intSource, m_intDestIndex);
			}
			else if(strMode == TAKE_M_R)
			{
				routerCrosspoint(m_intRouterDevice, intSource, m_intDestIndex+1);
			}
			else if(strMode == TAKE_L_M)
			{
				routerCrosspoint(m_intRouterDevice, intSource, m_intDestIndex);
			}
			else if(strMode == TAKE_R_M)
			{
				routerCrosspoint(m_intRouterDevice, intSource+1, m_intDestIndex);
			}
			//STEREO modes
			else if(strMode == TAKE_L_L)
			{
				routerCrosspoint(m_intRouterDevice, intSource, m_intDestIndex);
			}
			else if(strMode == TAKE_L_LR)
			{
				routerCrosspoint(m_intRouterDevice, intSource, m_intDestIndex);
				routerCrosspoint(m_intRouterDevice, intSource, m_intDestIndex+1);
			}
			else if(strMode == TAKE_L_R)
			{
				routerCrosspoint(m_intRouterDevice, intSource, m_intDestIndex+1);
			}
			else if(strMode == TAKE_R_L)
			{
				routerCrosspoint(m_intRouterDevice, intSource+1, m_intDestIndex);
			}
			else if(strMode == TAKE_R_LR)
			{
				routerCrosspoint(m_intRouterDevice, intSource+1, m_intDestIndex);
				routerCrosspoint(m_intRouterDevice, intSource+1, m_intDestIndex+1);
			}
			else if(strMode == TAKE_R_R)
			{
				routerCrosspoint(m_intRouterDevice, intSource+1, m_intDestIndex+1);
			}

		}

}

