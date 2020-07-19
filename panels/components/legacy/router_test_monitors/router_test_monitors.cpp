#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "router_test_monitors.h"

#define PNL_MAIN				1
#define PNL_POPUP_SOURCE_GRID	2

#define TIMER_SETUP				1
#define TIMEOUT_SETUP			1
#define TIMER_ENABLE			2
#define TIMEOUT_ENABLE			50

#define BUTTON_TEXT_TEST_OFF	"Monitor Test|OFF"
#define BUTTON_TEXT_TEST_ON		"Monitor Test|ON"


// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( router_test_monitors )

// constructor - equivalent to ApplCore STARTUP
router_test_monitors::router_test_monitors( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	// show a panel from file p1.bncs_ui and we'll know it as our panel 1
	panelShow( PNL_MAIN, "main.bncs_ui" );

	//initialise member vars
	m_intTestSourceIndex = 0;
	m_blnTestState = false;	 //always start with test off
	m_intTestStatusSlot = 0;
	m_intTestInfoDriver = 0;
	m_strTestStatusInstance = "";
	m_strStudio = getWorkstationSetting("ops_area");
	m_slDevDestList = "";
	m_slDevIdList = "";
}

// destructor - equivalent to ApplCore CLOSEDOWN
router_test_monitors::~router_test_monitors()
{
}

// all button pushes and notifications come here
void router_test_monitors::buttonCallback( buttonNotify *b )
{
	debug("router_test_monitors::BCB::panel<%1>, id<%2>, command<%3>, value<%4>", b->panel(), b->id(), b->command(), b->value());
	if( b->panel() == PNL_MAIN )
	{
		if( b->id() == "btnTest"){
			bncs_string strRet;
			textGet("text", PNL_MAIN, b->id(), strRet);			
			if(strRet == BUTTON_TEXT_TEST_ON)
			{
				notifyTestState(0);
			}
			else if (strRet == BUTTON_TEXT_TEST_OFF)
			{
				notifyTestState(1);
			}
		}
		else if( b->id() == "btnSelect")
		{
			panelPopup(PNL_POPUP_SOURCE_GRID, "popup_source.bncs_ui");
			textPut("instance", m_strRouterInstance, PNL_POPUP_SOURCE_GRID, "source_grid");
			textPut("background", bncs_string("%1_group_background").arg(m_strStudio)
				, PNL_POPUP_SOURCE_GRID, "source_grid");

			//textPut("page_offset", 1, PNL_MAIN, "source_grid");
		}
	}
	else if( b->panel() == PNL_POPUP_SOURCE_GRID )
	{
		if( b->id() == "btnClose")
		{
			panelDestroy(PNL_POPUP_SOURCE_GRID);
		}
		else if( b->id() == "btnKeepOpen")
		{

			textPut("statesheet", "enum_selected", PNL_POPUP_SOURCE_GRID, "btnKeepOpen");
		}
		else if( b->id() == "source_grid")
		{
			if (b->command() == "index" && b->value() > 0)
			{
				panelDestroy(PNL_POPUP_SOURCE_GRID);
					
				m_intTestSourceIndex = b->value().toInt();
				displaySelectedSource();
			}
		}
	}
}

// all revertives come here
int router_test_monitors::revertiveCallback( revertiveNotify * r )
{
	if( r->device() == m_intTestInfoDriver)
	{
		if (r->index() == m_intTestStatusSlot)
		{
			// test status slot
			if (r->sInfo()=="1") {
				setTestState(1);
			}
			else {
				setTestState(0);
			}
		} // correct slot
	} // monitors test info driver
	return 0;
}

// all database name changes come back here
void router_test_monitors::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string router_test_monitors::parentCallback( parentNotify *p )
{
	//Property set commands
	if( p->command() == "instance" )
	{
		m_strRouterInstance = p->value();
		getDev(m_strRouterInstance, &m_intRouterDevice);

	}
	else if( p->command() == "test_source_index" )
	{
		m_intTestSourceIndex = p->value().toInt();
		timerStart(TIMER_SETUP, TIMEOUT_SETUP);
	}
	else if( p->command() == "background_style" )
	{
		m_strBackgroundStyle = p->value();
		textPut("stylesheet", m_strBackgroundStyle, PNL_MAIN, "background");
	}
	else if( p->command() == "memory_instance" )
	{
		m_strMemoryInstance = p->value();
		getDev(m_strMemoryInstance, &m_intMemoryDevice);
	}
	else if (p->command() == "layout")	// we need to move this to a persistent setting as this is now not set externally
	{
		m_strLayout = p->value();
	}
	else if (p->command() == "test_state_slot")
	{
		m_intTestStatusSlot = p->value().toInt();
		
		//Check if both have now been set
		registerPollStatusSlot();
	}
	else if( p->command() == "test_state_instance" )
	{
		int intDevice;
		if(getDev(p->value(), &intDevice))
		{
			m_strTestStatusInstance = p->value();
			m_intTestInfoDriver = intDevice;

			//Check if both have now been set
			registerPollStatusSlot();
		}
	}

	else if (p->command() == "getSettings")	// new stuff comes in here
	{
		debug("router_test_monitors::PCB::getSettings:: value is <%1>", p->value());
		// format is <id=dest_1,instance=rtr_sta_sdi,dest_index=41>
		bncs_stringlist sl = p->value();
		if (sl.getNamedParam("instance") == m_strRouterInstance)	// check the instance values
		{
			if (sl.getNamedParam("id").startsWith("dest_"))	// check we have a correctly named id
			{
				bncs_string strDestIndex = sl.getNamedParam("dest_index");
				if (m_slDevDestList.find(strDestIndex) == -1)	// have we seen this before - another control *may* have instigated a getSettings
				{
					m_slDevDestList << strDestIndex;
					m_slDevIdList << sl.getNamedParam("id");
					debug("router_test_monitors::PCB::getSettings:: instance value match, dest list now <%1>", m_slDevDestList.toString());
				}
				else
				{
					debug("router_test_monitors::PCB::getSettings:: dest <%1> already in list <%2>", strDestIndex, m_slDevDestList.toString());
				}
			}
		}
	}

	//Runtime "methods"
	else if( p->command() == "setTest" )
	{
		setTestState(p->value().toInt());
	}

	//Return Commands
	else if( p->command() == "return" )
	{
		//Called by visual editor to persist settings in bncs_ui
		if(p->value() == "all")
		{
			bncs_stringlist sl;		// a means of returning the string
			sl += "test_source_index=" + bncs_string(m_intTestSourceIndex);
			sl += "background_style=" + m_strBackgroundStyle;
			sl += "memory_instance=" + m_strMemoryInstance;
			sl += "layout=" + m_strLayout;
			sl += "test_state_instance=" + m_strTestStatusInstance;
			sl += "test_state_slot=" + bncs_string(m_intTestStatusSlot);
			return sl.toString( '\n' );	// return our stringlist as newline delimited single string
		}
		//Called by parent to learn the components settings
		else if( p->value() == "getSettings" )
		{
			bncs_stringlist sl;		// a means of returning the string
			sl += "getsettings=instance=" + m_strRouterInstance;
			sl += "test_source_index=" + bncs_string(m_intTestSourceIndex);
			sl += "background_style=" + m_strBackgroundStyle;
			sl += "memory_instance=" + m_strMemoryInstance;
			return sl.toString();

//			return bncs_string( "getSettings=" )
//				+ bncs_string( "instance=" ) + m_strRouterInstance + ","
//				+ bncs_string( "test_source_index=" ) + bncs_string(m_intTestSourceIndex) + ","
//				+ bncs_string( "background_style=" ) + m_strBackgroundStyle + ","
//				+ bncs_string( "memory_instance=" ) + m_strMemoryInstance;
		}
	}

	return "";


}

// timer events come here
void router_test_monitors::timerCallback( int id )
{
	switch (id)
	{
	case TIMER_SETUP:
		timerStop(id);
		displaySelectedSource();
		hostNotify("fetch");	// go get 'em boy!
		break;

	default:
		timerStop(id);
		break;
	}
}

void router_test_monitors::displaySelectedSource()
{
	bncs_string strName;
	routerName(m_intRouterDevice, 0, m_intTestSourceIndex, strName);
	textPut( "text", strName, PNL_MAIN, "lblSource" );
}

void router_test_monitors::setTestState(int intCommand)
{
	if(intCommand)
	{
		m_blnTestState = true;
		textPut("text", BUTTON_TEXT_TEST_ON, PNL_MAIN, "btnTest");
		textPut("statesheet", "dest_locked", PNL_MAIN, "btnTest");
	}
	else
	{
		m_blnTestState = false;
		textPut("text", BUTTON_TEXT_TEST_OFF, PNL_MAIN, "btnTest");
		textPut("statesheet", "dest_unlocked", PNL_MAIN, "btnTest");
	}
}

void router_test_monitors::notifyTestState(int intState)
{
	if (intState)
	{
		// state == 1 = test on
		// Save snapshot
		infoWrite(m_intMemoryDevice, bncs_string("SAVE|%1#TESTRESTORE").arg(m_strLayout), 1);
		// iterate through dests on panel
		for (bncs_stringlistIterator slIt = m_slDevDestList.begin(); slIt != m_slDevDestList.end(); slIt++)
		{
			// make crosspoint to test sce
			hostNotify(bncs_string("route=%1,%2").arg(m_intTestSourceIndex).arg(*slIt));
			debug("router_test_monitors::PCB::notifyTestState():: hostNotify route=<%1>,<%2>", m_intTestSourceIndex, *slIt);
		}

		infoWrite(m_intTestInfoDriver, "1", m_intTestStatusSlot);
	}
	else
	{
		// state == 0 = test off
		// recall snapshot
		infoWrite(m_intMemoryDevice, bncs_string("RECALL|%1#TESTRESTORE|DEV=%2,%3")
			.arg(m_strLayout).arg(m_intRouterDevice).arg(m_slDevDestList.toString()), 1);

		infoWrite(m_intTestInfoDriver, "0", m_intTestStatusSlot);
	}



//	bncs_stringlist sltNotify;
//	sltNotify += "instance=" + m_strRouterInstance;
//	sltNotify += "memory_instance=" + m_strMemoryInstance;
//	sltNotify += "source_index=" + bncs_string(m_intTestSourceIndex);
//	sltNotify += "state=" + bncs_string(intState);
//	hostNotify("test=" + sltNotify.toString());
}

void router_test_monitors::registerPollStatusSlot()
{
	if(m_intTestSourceIndex > 0 && m_intTestInfoDriver > 0)
	{
		//delete previous registration if any
		if(m_intTestInfoDriver > 0){
			infoUnregister(m_intTestInfoDriver);
		}

		//register for status slot
		infoRegister(m_intTestInfoDriver, m_intTestStatusSlot, m_intTestStatusSlot);
		infoPoll(m_intTestInfoDriver, m_intTestStatusSlot, m_intTestStatusSlot);
	}
}
