#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include <map>
#include "ccu_source.h"

#define PNL_MAIN	1

#define TIMER_SETUP	1

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( ccu_source )

// constructor - equivalent to ApplCore STARTUP
ccu_source::ccu_source( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	//Initialise Member Vars
	m_instance = "";
	m_strCcuName = "Unknown CCU";
	m_intCcuDevice = 0;
	m_intCcuOffset = 0;
	m_intCcuStatusSlot = 0;
	m_intRcpsListSlot = 0;
	m_blnStatusAvailable = false;
	m_strStatesheetAvailable = "dest_selected";
	m_strStatesheetNotAvailable = "dest_deselected";
	// make a list of RCPs and their names
	bncs_config c("instances.cna_1_driver_stc");
	while (c.isChildValid()) {
		debug("ccu_source::ccu_source child tag=%1\n", c.childTag());
		if (c.childTag() == "group") {
			bncs_string groupInstance = c.childAttr("instance");
			debug("ccu_source::ccu_source groupInstance=%1\n", groupInstance);
			bncs_config d(bncs_string("instances.%1").arg(groupInstance));
			debug("ccu_source::ccu_source type=%1\n", d.attr("type"));
			if (d.attr("type") == "cna_1_rcp") {
				debug("ccu_source::ccu_source rcp_num=%1, alt_id=%2\n\n", d.attr("rcp_num"), d.attr("alt_id"));
				m_rcpMap.insert(pair<bncs_string, bncs_string>(d.attr("rcp_num"), d.attr("alt_id")));
			} // end if type = cna_1_rcp
		} // end if tag = group
		c.nextChild();
	} // end while valid children

	// show a panel from file p1.bncs_ui and we'll know it as our panel PNL_MAIN
	panelShow( PNL_MAIN, "p1.bncs_ui" );

	// you may need this call to set the size of this component 
	//  if it's used in a popup window 
//	setSize( 1024,668 );		// set the size explicitly
	setSize( PNL_MAIN );		// set the size to the same as the specified panel
	timerStart(TIMER_SETUP, 100);

}

// destructor - equivalent to ApplCore CLOSEDOWN
ccu_source::~ccu_source()
{
}

// all button pushes and notifications come here
void ccu_source::buttonCallback( buttonNotify *b )
{
	if( b->panel() == PNL_MAIN )
	{
		debug("cc_source::buttonCallback any button Released\n");
		hostNotify(bncs_string("selected_ccu=%1").arg(m_instance));
	}
}

// all revertives come here
int ccu_source::revertiveCallback( revertiveNotify * r )
{
	if (r->device() == m_intCcuDevice)
	debug("ccu_source::revertiveCallback sInfo='%1'\n", r->sInfo());
	{
		if (r->index() == m_intCcuStatusSlot)
		{
			if (r->sInfo() == "Available")
			{
				m_blnStatusAvailable = true;
				textPut("text", "CCU Available", PNL_MAIN, "lbl_available");
				textPut("statesheet", m_strStatesheetAvailable, PNL_MAIN, "lbl_available");
			}
			else
			{
				m_blnStatusAvailable = false;
				textPut("text", "CCU Unavail...", PNL_MAIN, "lbl_available");
				textPut("statesheet", m_strStatesheetNotAvailable, PNL_MAIN, "lbl_available");
			}
		}
		else if (r->index() == m_intRcpsListSlot)
		{
			bncs_stringlist slRcpList = bncs_stringlist(r->sInfo());
			bncs_string rcps = "";
			for (bncs_stringlist::iterator it = slRcpList.begin(); it != slRcpList.end(); ++it) {
				debug("ccu_source::revertiveCallback *it=%1\n", *it);
				debug("ccu_source::revertiveCallback count=%1\n", bncs_string((int)m_rcpMap.count(*it)));
				if (m_rcpMap.count(*it)) { // that RCP exists
					rcps += bncs_string("%1|").arg(m_rcpMap[*it].replace("|", " "));
				}
			}
			if (rcps.length()) {
				textPut("text", rcps, PNL_MAIN, "lbl_others");
				controlShow(PNL_MAIN, "lbl_others");
			}
			else {
				controlHide(PNL_MAIN, "lbl_others");
				textPut("text", "", PNL_MAIN, "lbl_others");
			}
		}
	}

	return 0;
}

// all database name changes come back here
void ccu_source::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string ccu_source::parentCallback( parentNotify *p )
{
	debug("ccu_source::parentCallback command='%1' value='%2'\n", p->command(), p->value());

	//Property set commands
	if (p->command() == "instance")
	{
		m_instance = p->value();
		debug("ccu_source::parentCallback instance=%1\n", m_instance);
	}

	//Runtime "methods"
	else if (p->command() == "start")
	{
		start();
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
		sltEvents << "selected_ccu=<instance>";
		sltEvents << "instance=<instance>";
		sltEvents << "getSettings=<settings>";
		return sltEvents.toString('\n');
	}
	else if (p->command() == "_commands")
	{
		bncs_stringlist sltCommands;
		return sltCommands.toString('\n');
	}

	return "";
}

// timer events come here
void ccu_source::timerCallback( int id )
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

void ccu_source::start()
{
	//Get the device ID for the configured instance
	debug("ccu_source::start m_instance=%1\n", m_instance);

	if (getDev(m_instance, &m_intCcuDevice, &m_intCcuOffset))
	{ // true if valid instance
		debug("ccu_source::start id=%1, offset=%2\n", m_intCcuDevice, m_intCcuOffset);
		char type;
		getDevSlot(m_instance, "status", &m_intCcuDevice, &m_intCcuStatusSlot, &type);
		getDevSlot(m_instance, "rcps_list", &m_intCcuDevice, &m_intRcpsListSlot, &type);
		debug("ccu_source::start StatusSlot=%1, RcpsListSlot=%2\n", m_intCcuStatusSlot, m_intRcpsListSlot);

		//Display the CCU name
		bncs_config c(bncs_string("instances.%1").arg(m_instance));
		m_strCcuName = c.attr("alt_id");
		debug("ccu_source::start element=%1, name=%2\n", bncs_string("instances.%1").arg(m_instance), m_strCcuName);
		textPut("text", m_strCcuName, PNL_MAIN, "btn_name");

		//Register and poll the RCP
		infoRegister(m_intCcuDevice, m_intCcuStatusSlot, m_intCcuStatusSlot, false);
		infoRegister(m_intCcuDevice, m_intRcpsListSlot, m_intRcpsListSlot, true);
		infoPoll(m_intCcuDevice, m_intCcuStatusSlot, m_intCcuStatusSlot);
		infoPoll(m_intCcuDevice, m_intRcpsListSlot, m_intRcpsListSlot);

		//Unhide the controls (except 'others' which is done separately)
		controlShow(PNL_MAIN, "lbl_available");
		controlShow(PNL_MAIN, "lbl_others");
		controlEnable(PNL_MAIN, "btn_name");
	}
	else
	{ // invalid instance
		m_strCcuName = "Unknown!";
		textPut("text", m_strCcuName, PNL_MAIN, "btn_name");
		//hide the other controls
		controlHide(PNL_MAIN, "lbl_available");
		controlHide(PNL_MAIN, "lbl_others");
		controlDisable(PNL_MAIN, "btn_name");
		m_instance = "";
	}
}
