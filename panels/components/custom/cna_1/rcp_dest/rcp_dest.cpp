#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "rcp_dest.h"

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( rcp_dest )

// constructor - equivalent to ApplCore STARTUP
rcp_dest::rcp_dest( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	//Initialise Member Vars
	m_instance = "";
	m_intRcpDevice = 0;
	m_intRcpOffset = 0;
	m_intRcpStatusSlot = 0;
	m_intRcpCameraSlot = 0;
	m_strRcpName = "";
	m_blnSelected = false;
	m_strCcuInstance = "";
	m_strCcuName = "";
	m_blnStatusAvailable = false;
	m_strStatesheetDestSelected = "dest_selected";
	m_strStatesheetDestDeselected = "dest_deselected";
	m_strStatesheetAvailable = "dest_selected";
	m_strStatesheetNotAvailable = "dest_deselected";

	// show a panel from file p1.bncs_ui and we'll know it as our panel PNL_MAIN
	panelShow( PNL_MAIN, "p1.bncs_ui" );

	// you may need this call to set the size of this component 
	//  if it's used in a popup window 
//	setSize( 1024,668 );		// set the size explicitly
	setSize( PNL_MAIN );		// set the size to the same as the specified panel
	timerStart(TIMER_SETUP, 100);
}

// destructor - equivalent to ApplCore CLOSEDOWN
rcp_dest::~rcp_dest()
{
}

// all button pushes and notifications come here
void rcp_dest::buttonCallback( buttonNotify *b )
{
	if( b->panel() == PNL_MAIN )
	{
		if (b->id() == "btn_dest")
		{
			debug("rcp_dest::buttonCallback btn_dest Released\n");
			hostNotify(bncs_string("selected_rcp=%1").arg(m_instance));
		}
		else if (b->id() == "comp_ccu")
		{
			if (b->command() == "release")
			{
				infoWrite(m_intRcpDevice, "0", m_intRcpCameraSlot);
			}
			else if (b->command() == "get_rcp_instance")
			{
				textPut("parent_instance", m_instance, PNL_MAIN, "comp_ccu");
			}
		}
	}
}

// all revertives come here
int rcp_dest::revertiveCallback( revertiveNotify * r )
{
	if (r->device() == m_intRcpDevice)
	{
		if (r->index() == m_intRcpStatusSlot)
		{
			if (r->sInfo() == "Available")
			{
				m_blnStatusAvailable = true;
				textPut("text", "RCP Available", PNL_MAIN, "lbl_available");
				textPut("statesheet", m_strStatesheetAvailable, PNL_MAIN, "lbl_available");
			}
			else
			{
				m_blnStatusAvailable = false;
				textPut("text", "RCP Unavailable", PNL_MAIN, "lbl_available");
				textPut("statesheet", m_strStatesheetNotAvailable, PNL_MAIN, "lbl_available");
			}
		}
		else if (r->index() == m_intRcpCameraSlot)
		{
			m_strCcuInstance = bncs_string("cna_1/cam/%1").arg(r->sInfo().toInt(), '0', 2);
			debug("rcp_dest::revertiveCallback m_strCcuInstance=%1\n", m_strCcuInstance);
			bncs_config c(bncs_string("instances.%1").arg(m_strCcuInstance));
			m_strCcuName = c.attr("alt_id");
			debug("rcp_dest::revertiveCallback m_strCcuName=%1\n", m_strCcuName);
			if (m_strCcuName.length() == 0)
			{
				m_strCcuName = "Unknown CCU";
			}
			textPut("instance", m_strCcuInstance, PNL_MAIN, "comp_ccu");
		}
	}

	return 0;
}

// all database name changes come back here
void rcp_dest::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string rcp_dest::parentCallback( parentNotify *p )
{
	debug("rcp_dest::parentCallback command='%1' value='%2'\n", p->command(), p->value());

	//Property set commands
	if (p->command() == "instance") // this is the RCP
	{
		m_instance = p->value();
		debug("rcp_dest::parentCallback instance=%1\n", m_instance);
	}

	//Runtime "methods"

	else if (p->command() == "select")
	{
		debug("router_dest::parentCallback select");

		if (p->value() == m_instance) {
			textPut("statesheet", m_strStatesheetDestSelected, PNL_MAIN, "btn_dest");
			m_blnSelected = true;
		}
		else {
			textPut("statesheet", m_strStatesheetDestDeselected, PNL_MAIN, "btn_dest");
			m_blnSelected = false;
		}
	}

	else if (p->command() == "deselect")
	{
		debug("rcp_dest::parentCallback deselect");
		m_blnSelected = false;
		textPut("statesheet", m_strStatesheetDestDeselected, PNL_MAIN, "btnDest");
	}

	else if (p->command() == "route_ccu") // value is CCU instance
	{
		if (m_blnSelected)
		{
			m_strCcuInstance = p->value();
			infoWrite(m_intRcpDevice, (m_strCcuInstance.right(2)).toInt(), m_intRcpCameraSlot);
		}
	}

	//Called by parent to asyncronously learn the components settings
	else if (p->command() == "getSettings")
	{
		hostNotify(bncs_string("getSettings=")
			+ bncs_string("id=") + id() + ","	// we also need the id to identify where the settings are from
			+ bncs_string("instance=") + m_instance);
	}

	//Return Commands
	else if (p->command() == "return")
	{
		//Called by visual editor to persist settings in bncs_ui
		if (p->value() == "all")
		{
			return "";
		}
		//Called by parent to learn the components settings
		else if (p->value() == "getSettings")
		{
			return bncs_string("getSettings=")
				+ bncs_string("id=") + id() + ","	// we also need the id to identify where the settings are from
				+ bncs_string("instance=") + m_instance;
		}
	}

	//Connections information for BNCS Vis Ed
	else if (p->command() == "_events")
	{
		bncs_stringlist sltEvents;
		sltEvents << "selected_index=<dest_index>";
		sltEvents << "dest_index=<dest_index>";
		sltEvents << "instance=<instance>";
		sltEvents << "source_index=<source index routed to this dest>";
		sltEvents << "command=selected";
		sltEvents << "getSettings=<settings>";
		return sltEvents.toString('\n');
	}
	else if (p->command() == "_commands")
	{
		bncs_stringlist sltCommands;
		sltCommands << "select=<instance>";
		sltCommands << "deselect";
		sltCommands << "route_ccu=<ccu_number>";
		return sltCommands.toString('\n');
	}

	return "";
}

// timer events come here
void rcp_dest::timerCallback( int id )
{
	switch( id )
	{
	case TIMER_SETUP:
		timerStop(id);
		start();
		break;

	default:	// Unhandled timer event
		timerStop(id);
		break;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// Callbacks above - Methods below ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void rcp_dest::start()
{
	//Get the device ID for the configured instance
	debug("rcp_dest::start m_instance=%1\n", m_instance);
	getDev(m_instance, &m_intRcpDevice, &m_intRcpOffset);
	debug("rcp_dest::start id=%1, offset=%2\n", m_intRcpDevice, m_intRcpOffset);
	char type;
	getDevSlot(m_instance, "status", &m_intRcpDevice, &m_intRcpStatusSlot, &type);
	getDevSlot(m_instance, "camera", &m_intRcpDevice, &m_intRcpCameraSlot, &type);
	debug("rcp_dest::start StatusSlot=%1, CameraSlot=%2\n", m_intRcpStatusSlot, m_intRcpCameraSlot);

	//Display the RCP name
	bncs_config c(bncs_string("instances.%1").arg(m_instance));
	m_strRcpName = c.attr("alt_id");
	debug("rcp_dest::start element=%1, name=%2\n", bncs_string("instances.%1").arg(m_instance), m_strRcpName);
	textPut("text", m_strRcpName, PNL_MAIN, "btn_dest");

	//Register and poll the RCP
	infoRegister(m_intRcpDevice, m_intRcpStatusSlot, m_intRcpStatusSlot, false);
	infoRegister(m_intRcpDevice, m_intRcpCameraSlot, m_intRcpCameraSlot, true);
	infoPoll(m_intRcpDevice, m_intRcpStatusSlot, m_intRcpStatusSlot);
	infoPoll(m_intRcpDevice, m_intRcpCameraSlot, m_intRcpCameraSlot);

}
