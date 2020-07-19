#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "router_salvo_multiviewer.h"

#define PNL_MAIN				1
#define POPUP_LIST				2
#define MEMORY_NAME_DB_SWITCH	2
#define SLOT_COMMAND			1
#define SLOT_STATUS_MESSAGE		2
#define SLOT_FIRST_MEMORY		81

#define TEST_MEMORY_NAME		"TESTRESTORE"

#define TIMER_RELOAD			1
#define TIMEOUT_RELOAD			750
#define TIMER_FETCH				2
#define TIMEOUT_FETCH			100

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( router_salvo_multiviewer )

// constructor - equivalent to ApplCore STARTUP
router_salvo_multiviewer::router_salvo_multiviewer( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	// show a panel from file p1.bncs_ui and we'll know it as our panel PNL_MAIN
	panelShow( PNL_MAIN, "main.bncs_ui" );

	//initialise member vars
	m_strMvInstance = "";
	m_strMemoryInstance = "";
	m_intMemoryDevice = 0;
	m_strMemoryLayout = "";
	m_intDestCount = 0;
	m_intFirstDest = -1;


	timerStart(TIMER_FETCH, TIMEOUT_FETCH);

	// you may need this call to set the size of this component 
	//  if it's used in a popup window 
//	setSize( 1024,668 );		// set the size explicitly
//	setSize( PNL_MAIN );		// set the size to the same as the specified panel
}

// destructor - equivalent to ApplCore CLOSEDOWN
router_salvo_multiviewer::~router_salvo_multiviewer()
{
}

// all button pushes and notifications come here
void router_salvo_multiviewer::buttonCallback( buttonNotify *b )
{
	if (b->panel() == PNL_MAIN)
	{
		if (b->id() == "btnMemory")
		{
			panelPopup(POPUP_LIST, "popup_list.bncs_ui");
			updateSalvoList();
		}
	}
	else if (b->panel() == POPUP_LIST)
	{
		if (b->id() == "btnRecall")
		{
			bncs_string strSalvoName = getCurrentSalvo();
			if (strSalvoName.length() > 0) {

				debug(bncs_string("Device: <%1> String : <RECALL|%2#%3|DEV=%4,RANGE,%5,%6> Slot: <%7>")
					.arg(m_intMemoryDevice)
					.arg(m_strMemoryLayout).arg(strSalvoName).arg(m_intRouterDevice).arg(m_intFirstDest).arg(m_intFirstDest + m_intDestCount - 1)
					.arg(SLOT_COMMAND));

				infoWrite(m_intMemoryDevice, bncs_string("RECALL|%1#%2|DEV=%3,RANGE,%4,%5")
					.arg(m_strMemoryLayout).arg(strSalvoName).arg(m_intRouterDevice)
					.arg(m_intFirstDest).arg(m_intFirstDest + m_intDestCount - 1), SLOT_COMMAND);

			}
		} else if (b->id() == "lstSalvo") {
			controlEnable(POPUP_LIST, "btnRecall");
		} else if (b->id() == "btnClose") {
			panelDestroy(POPUP_LIST);
		}
	}
}

// all revertives come here
int router_salvo_multiviewer::revertiveCallback( revertiveNotify * r )
{
	if (r->device() == m_intMemoryDevice){
		if (r->index() == SLOT_STATUS_MESSAGE)
		{
			textPut("text", r->sInfo(), POPUP_LIST, "lblMemoryAutoMessage");
		}
	}
	return 0;
}

// all database name changes come back here
void router_salvo_multiviewer::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string router_salvo_multiviewer::parentCallback( parentNotify *p )
{
	//Property set commands
	if (p->command() == "instance" && p->value() != m_strMvInstance) {
		// instance is the multiviewer e.g. studio_b_mv_1
		m_strMvInstance = p->value();
		int startOfMv = m_strMvInstance.find("_mv");
		bncs_string area = m_strMvInstance.left(startOfMv);
		bncs_string strMvSettings = bncs_string("%1_multiviewers").arg(area); // e.g. studio_b_multiviewers
		m_strMemoryInstance = getObjectSetting(strMvSettings, "memory");
		getDev(m_strMemoryInstance, &m_intMemoryDevice);
		m_strMemoryLayout = getObjectSetting(strMvSettings, "memory_layout");
		m_strRouterInstance = getObjectSetting(strMvSettings, "router");
		getDev(m_strRouterInstance, &m_intRouterDevice);
		m_intDestCount = getObjectSetting(m_strMvInstance, "dest_count");
		m_intFirstDest = getObjectSetting(m_strMvInstance, "first_dest");
		infoRegister(m_intMemoryDevice, SLOT_STATUS_MESSAGE, SLOT_STATUS_MESSAGE);
	}

	//Methods

	//Return Commands
	else if (p->command() == "return")
	{
		//Called by visual editor to persist settings in bncs_ui
		if (p->value() == "all")
		{
			bncs_stringlist sl;		// a means of returning the string
			//sl += "memory_instance=" + m_strMemoryInstance;
			//sl += "layout=" + m_strMemoryLayout;
			return sl.toString('\n');	// return our stringlist as newline delimited single string
		}
		//Called by parent to learn the components settings
		else if (p->value() == "getSettings")	// this will now be depricated as the component will be responsible for getting and construicting the settings from other components
		{
			bncs_stringlist sl;		// a means of returning the string
			//sl += "getSettings=instance=" + m_strRouterInstance;
			//sl += "memory_instance=" + m_strMemoryInstance;
			return sl.toString();
		}
	}

	//Connections information for BNCS Vis Ed
	else if (p->command() == "_events")
	{
		bncs_stringlist sltEvents;
		sltEvents << "fetch";
		return sltEvents.toString('\n');
	}
	else if (p->command() == "_commands")
	{
		bncs_stringlist sltCommands;
		sltCommands << "[command]=[value]";
		return sltCommands.toString('\n');
	}

	return "";
}

// timer events come here
void router_salvo_multiviewer::timerCallback( int id )
{
	switch (id)
	{
	case TIMER_RELOAD:
		timerStop(TIMER_RELOAD);
		updateSalvoList();
		break;

	case TIMER_FETCH:
		timerStop(TIMER_FETCH);
		hostNotify("fetch");	// go get 'em boy!

	default:
		timerStop(id);
		break;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// Callbacks above - Methods below ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void router_salvo_multiviewer::updateSalvoList()
{
	textPut("clear", POPUP_LIST, "lstSalvo");			//clear the listbox
	textPut("sort", "false", POPUP_LIST, "lstSalvo");  //switch sorting off (speeds up load of listbox)
	controlDisable(POPUP_LIST, "btnRecall");

	for (int intSlot = SLOT_FIRST_MEMORY; intSlot < 4096; intSlot++) {
		bncs_string strName;
		routerName(m_intMemoryDevice, MEMORY_NAME_DB_SWITCH, intSlot, strName);
		int intNameAsNumber = strName.toInt();
		if (intNameAsNumber != intSlot) { // i.e only add memory names for this layout and also not those indexes which do not have memories
			bncs_string strPrefix = m_strMemoryLayout + "#";
			int intFind = strName.find(strPrefix);
			if (intFind != -1) { // then area string found so memory for this layout
				bncs_stringlist sltLayoutSalvo = bncs_stringlist(strName, '#');
				if (sltLayoutSalvo[1] == TEST_MEMORY_NAME)
				{
					//ignore this memory - it is used by the test monitors component
				}
				else
				{
					//first remove any other copies of this salvo name
					textPut("remove.text.all", sltLayoutSalvo[1], POPUP_LIST, "lstSalvo");
					//now add this salvo name
					textPut("add", sltLayoutSalvo[1], POPUP_LIST, "lstSalvo");
				}
			}
		}
	}
	textPut("sort", "true", POPUP_LIST, "lstSalvo"); // now sort just once at end
}

bncs_string router_salvo_multiviewer::getCurrentSalvo()
{
	bncs_string strSalvoName;
	textGet("selected", POPUP_LIST, "lstSalvo", strSalvoName);
	return strSalvoName;
}
