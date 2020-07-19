#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "router_salvo.h"

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
EXPORT_BNCS_SCRIPT( router_salvo )

// constructor - equivalent to ApplCore STARTUP
router_salvo::router_salvo( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	// show a panel from file p1.bncs_ui and we'll know it as our panel 1
	panelShow( PNL_MAIN, "main.bncs_ui" );

	//initialise member vars
	m_strRouterInstance = "";
	m_intRouterDevice = 0;
	m_strMemoryInstance = "";
	m_intMemoryDevice = 0;
	m_slDevDestList = "";

	timerStart(TIMER_FETCH, TIMEOUT_FETCH);

}

// destructor - equivalent to ApplCore CLOSEDOWN
router_salvo::~router_salvo()
{
}

// all button pushes and notifications come here
void router_salvo::buttonCallback( buttonNotify *b )
{
	if( b->panel() == PNL_MAIN )
	{
		if( b->id() == "btnMemory")
		{
			panelPopup(POPUP_LIST, "popup_list.bncs_ui");
			switchViewKeyboard(false);

			updateSalvoList();

//			controlHide(POPUP_LIST, "keyboard");
//			controlHide(POPUP_LIST, "lblEnterSalvoName");
//			controlHide(POPUP_LIST, "btnConfirm");
//			controlHide(POPUP_LIST, "btnCancel");
		}
	}
	else if(b->panel() == POPUP_LIST)
	{
		//	infoWrite(m_intMemoryDevice, bncs_string("SAVE|%1#%2").arg(m_strLayout).arg(strSalvoName), 1);

		if( b->id() == "btnDelete")
		{
			bncs_string strName = getCurrentSalvo();
			if(strName.length() > 0){
				//delete the current salvo			
				infoWrite(m_intMemoryDevice, bncs_string("DELETE|%1#%2")
					.arg(m_strLayout).arg(strName), SLOT_COMMAND);
			}

			//request a delayed load the list of salvos for this panel
			timerStart(TIMER_RELOAD, TIMEOUT_RELOAD);

		}
		else if( b->id() == "btnNew")
		{
			switchViewKeyboard(true);
		}
		else if( b->id() == "btnRecall")
		{
			bncs_string strName = getCurrentSalvo();
			if(strName.length() > 0){
				infoWrite(m_intMemoryDevice, bncs_string("RECALL|%1#%2|DEV=%3,%4")
					.arg(m_strLayout).arg(strName).arg(m_intRouterDevice).arg(m_slDevDestList.toString()), SLOT_COMMAND);
			}

		}
		else if( b->id() == "btnSave")
		{
			bncs_string strName = getCurrentSalvo();
			if(strName.length() > 0){
				infoWrite(m_intMemoryDevice, bncs_string("SAVE|%1#%2")
					.arg(m_strLayout).arg(strName), SLOT_COMMAND);
			}
		}
		else if(b->id() == "btnCancel")
		{
			switchViewKeyboard(false);
		}
		else if(b->id() == "btnConfirm")
		{
			confirmNewSalvoName();
		}
		else if(b->id() == "lstSalvo")
		{
			controlEnable(POPUP_LIST, "btnDelete");
			controlEnable(POPUP_LIST, "btnRecall");
			controlEnable(POPUP_LIST, "btnSave");
		}
		else if(b->id() == "keyboard")
		{
			if(b->sub(0) == "return")
			{
				//rtn pressed
				confirmNewSalvoName();
			}
		}
		else if(b->id() == "btnClose")
		{
			panelDestroy(POPUP_LIST);
		}
	}
}

// all revertives come here
int router_salvo::revertiveCallback( revertiveNotify * r )
{
	if(r->device() == m_intMemoryDevice){
		if(r->index() == SLOT_STATUS_MESSAGE)
		{
			textPut("text", r->sInfo(), POPUP_LIST, "lblMemoryAutoMessage");
		}
	}
	return 0;
}

// all database name changes come back here
void router_salvo::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string router_salvo::parentCallback( parentNotify *p )
{
	//Property set commands
	if( p->command() == "instance" )
	{
		m_strRouterInstance = p->value();
		bool blnRouterInstance = getDev( m_strRouterInstance, &m_intRouterDevice);
	}
	else if( p->command() == "memory_instance" )
	{
		m_strMemoryInstance = p->value();
		bool blnMemoryInstance = getDev( m_strMemoryInstance, &m_intMemoryDevice);
		infoRegister(m_intMemoryDevice, SLOT_STATUS_MESSAGE, SLOT_STATUS_MESSAGE);
	}
	//Methods
	else if( p->command() == "layout" )	// we need to move this to a persistent setting as this is now not set externally
	{
		m_strLayout = p->value();
	}
	//Methods
//	else if (p->command() == "setDevDestList")	// we need to build this ourselves now rather than rely on routerCanvas
//	{
//		m_strDevDestList = p->value();
//	}

	else if (p->command() == "getSettings")	// new stuff comes in here
	{
		debug("router_salvo::PCB::getSettings:: value is <%1>", p->value());
		// format is <id=dest_1,instance=rtr_sta_sdi,dest_index=41>
		bncs_stringlist sl = p->value();
		if (sl.getNamedParam("instance") == m_strRouterInstance)	// check the instance values
		{
			if (sl.getNamedParam("id").find("dest_") > -1)	// check we have a correctly named id
			{
				bncs_string strDestIndex = sl.getNamedParam("dest_index");
				if (m_slDevDestList.find(strDestIndex) == -1)	// have we seen this before - another control *may* have instigated a getSettings
				{
					m_slDevDestList << strDestIndex;
					debug("router_salvo::PCB::getSettings:: instance value match, dest list now <%1>", m_slDevDestList.toString());
				}
				else
				{
					debug("router_salvo::PCB::getSettings:: dest <%1> already in list <%2>", strDestIndex, m_slDevDestList.toString());
				}
			}
		}
	}

	//Return Commands
	else if( p->command() == "return" )
	{
		//Called by visual editor to persist settings in bncs_ui
		if(p->value() == "all")
		{
			bncs_stringlist sl;		// a means of returning the string
			sl += "memory_instance=" + m_strMemoryInstance;
			sl += "layout=" + m_strLayout;
			return sl.toString('\n');	// return our stringlist as newline delimited single string
		}
		//Called by parent to learn the components settings
		else if( p->value() == "getSettings" )	// this will now be depricated as the component will be responsible for getting and construicting the settings from other components
		{
			bncs_stringlist sl;		// a means of returning the string
			sl += "getSettings=instance=" + m_strRouterInstance;
			sl += "memory_instance=" + m_strMemoryInstance;
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
void router_salvo::timerCallback( int id )
{
	switch (id)
	{
	case TIMER_RELOAD:
		timerStop(TIMER_RELOAD);
		updateSalvoList();
		//textPut("text", "Salvo List Reloaded", POPUP_LIST, "lblMemoryAutoMessage");
		break;

	case TIMER_FETCH:
		timerStop(TIMER_FETCH);
		hostNotify("fetch");	// go get 'em boy!

	default:
		timerStop(id);
		break;
	}
}

void router_salvo::switchViewKeyboard(bool blnKeyboardVisible)
{
	if(blnKeyboardVisible)
	{
		controlHide(POPUP_LIST, "btnDelete");
		controlHide(POPUP_LIST, "btnNew");
		controlHide(POPUP_LIST, "btnRecall");
		controlHide(POPUP_LIST, "btnSave");

		controlShow(POPUP_LIST, "keyboard");
		controlShow(POPUP_LIST, "lblEnterSalvoName");
		controlShow(POPUP_LIST, "btnConfirm");
		controlShow(POPUP_LIST, "btnCancel");

		controlDisable(POPUP_LIST, "lstSalvo");
		textPut("selected", "none", POPUP_LIST, "lstSalvo");

		//give the keyboard control focus so the hardware keyboard works
		textPut("setFocus", "", POPUP_LIST, "keyboard");

	}
	else
	{
		//now hide the "NEW" controls again
		controlHide(POPUP_LIST, "keyboard");
		controlHide(POPUP_LIST, "lblEnterSalvoName");
		controlHide(POPUP_LIST, "btnConfirm");
		controlHide(POPUP_LIST, "btnCancel");

		//and re-enable the other controls
		controlShow(POPUP_LIST, "btnDelete");
		controlShow(POPUP_LIST, "btnNew");
		controlShow(POPUP_LIST, "btnRecall");
		controlShow(POPUP_LIST, "btnSave");

		//initially disable the controls that depend on a valid listbox selection
		controlDisable(POPUP_LIST, "btnDelete");
		controlDisable(POPUP_LIST, "btnRecall");
		controlDisable(POPUP_LIST, "btnSave");

		controlEnable(POPUP_LIST, "lstSalvo");
	}
}

void router_salvo::saveSalvo(bncs_string strSalvoName)
{
	infoWrite(m_intMemoryDevice, bncs_string("SAVE|%1#%2").arg(m_strLayout).arg(strSalvoName), SLOT_COMMAND);
}

void router_salvo::updateSalvoList()
{
	textPut( "clear", POPUP_LIST, "lstSalvo" );			//clear the listbox
	textPut( "sort", "false", POPUP_LIST, "lstSalvo" );  //switch sorting off (speeds up load of listbox)
	
	for( int intSlot = SLOT_FIRST_MEMORY ; intSlot < 4096 ; intSlot++ ) { 
		bncs_string strName;
		routerName( m_intMemoryDevice, MEMORY_NAME_DB_SWITCH, intSlot, strName );
		int intNameAsNumber = strName.toInt();
		if (intNameAsNumber != intSlot) { // i.e only add memory names for this layout and also not those indexes which do not have memories
			bncs_string strPrefix = m_strLayout + "#";
			int intFind = strName.find( strPrefix ); 
			if (intFind != -1) { // then area string found so memory for this layout
				bncs_stringlist sltLayoutSalvo = bncs_stringlist(strName, '#');

				if(sltLayoutSalvo[1] == TEST_MEMORY_NAME)
				{
					//ignore this memory - it is used by the test monitors component
				}
				else
				{
					//first remove any other copies of this salvo name
					textPut( "remove.text.all", sltLayoutSalvo[1], POPUP_LIST, "lstSalvo");
					//now add this salvo name
					textPut( "add", sltLayoutSalvo[1], POPUP_LIST, "lstSalvo");
				}
			}
		}
	}
	textPut( "sort", "true", POPUP_LIST, "lstSalvo" ); // now sort just once at end
}

bncs_string router_salvo::getCurrentSalvo()
{
	bncs_string strSalvoName;
	textGet("selected", POPUP_LIST, "lstSalvo", strSalvoName);
	return strSalvoName;
}

void router_salvo::confirmNewSalvoName()
{
	//Get the name of the new salvo to be created
	bncs_string strNewSalvoName;
	textGet("text", POPUP_LIST, "keyboard", strNewSalvoName);
	debug("new salvo name is '%1'", strNewSalvoName);
	
	switchViewKeyboard(false);

	//Now save the salvo
	saveSalvo(strNewSalvoName);

	//request a delayed load the list of salvos for this panel
	timerStart(TIMER_RELOAD, TIMEOUT_RELOAD);
}
