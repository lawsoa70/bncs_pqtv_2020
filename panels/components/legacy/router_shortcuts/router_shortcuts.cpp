// Connections component to interact with sourceGrid and take. Returns the 
// same number of parameters as buttons on the form with prefix of source_

#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "router_shortcuts.h"

#define LAYOUT_DEFAULT  "default"
#define PANEL_MAIN     1

#define DATABASE_SOURCE  0

#define LAYOUTS_SUB_DIR "layouts";

#define DEFAULT_BACKGROUND_COLOUR "#9a9a9a";

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( router_shortcuts )

// constructor - equivalent to ApplCore STARTUP
router_shortcuts::router_shortcuts( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	m_strBackgroundColour = DEFAULT_BACKGROUND_COLOUR ;
	// show a panel from file p1.bncs_ui and we'll know it as our panel 1
	panelShow( PANEL_MAIN, LAYOUT_DEFAULT );

	int numBtns = setListSizes();
		
		for (int i = 0; i < numBtns; i++)
	{
// 		arrButtonAlias[i] = "";
// 		arrButtonLabel[i] = "";
// 		arrSourceIndex[i] = -1;
		listButtonAlias[i] = "";
		listButtonLabel[i] = "";
		listSourceIndex[i] = -1;
	}
	// you may need this call to set the size of this component 
	//  if it's used in a popup window 
//	setSize( 1024,668 );		// set the size explicitly
//	setSize( 1 );				// set the size to the same as the specified panel
}

// destructor - equivalent to ApplCore CLOSEDOWN
router_shortcuts::~router_shortcuts()
{
}

// all button pushes and notifications come here
void router_shortcuts::buttonCallback( buttonNotify *b )
{
	if( b->panel() == PANEL_MAIN )
	{
		if (b->id().startsWith("source"))
		{
			bncs_stringlist sltBtn = bncs_stringlist(b->id(), '_');
			int id = sltBtn[1].toInt();
			if (listSourceIndex[id - 1].toInt() >= 0)// && listSourceIndex[id - 1].toInt() <= m_intRouterSize)
			{
				hostNotify(bncs_string("index=%1").arg(listSourceIndex[id - 1]));
				setButtonsState(b->id());
			}
		}
//		if this is the only buttonNotify
			// hostnotify source=idx

	}
}

// all revertives come here
int router_shortcuts::revertiveCallback( revertiveNotify * r )
{
/*	switch( r->device() )
	{
		case 123:
 			textPut( "text", r->sInfo(), 1, 3 );
			break;
	}
*/	return 0;
}

// all database name changes come back here
void router_shortcuts::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string router_shortcuts::parentCallback( parentNotify *p )
{

	debug("router_dest::parentCallback command='%1' value='%2'", p->command(), p->value());

	//Property set commands
	if( p->command() == "instance" )
	{
		m_strRouterInstance = p->value();
		getDev(m_strRouterInstance, &m_intDevice);
	}
	else if( p->command().startsWith("source_index_") )  //TODO CHECK SYNTAX
	{
	//NOT	m_intDestIndex = p->value().toInt();
		bncs_stringlist sourceIndexList = bncs_stringlist(p->command(), '_');
		int listPos = sourceIndexList[2].toInt() - 1;
		int pVal = p->value().toInt();
		listSourceIndex[listPos] = pVal;
		bncs_string strLabel = getLabel(listPos, m_intDevice, DATABASE_SOURCE, pVal);
		m_intRouterSize = getRouterSize(m_intDevice, DATABASE_SOURCE);
		//debug("****rtrSize**** = %1", m_intRouterSize);
		if (pVal >= 0)// && pVal <= m_intRouterSize)
		{
			//debug("****pVal**** = %1", pVal);
			textPut("text", strLabel, PANEL_MAIN, bncs_string("source_%1").arg(listPos + 1));
			controlEnable(PANEL_MAIN, bncs_string("source_%1").arg(listPos + 1));
		}
		else
		{
			textPut("text", "!!!", PANEL_MAIN, bncs_string("source_%1").arg(listPos + 1));
			controlDisable(PANEL_MAIN, bncs_string("source_%1").arg(listPos + 1));
		}
	}
	else if( p->command().startsWith("source_alias_") )
	{
		bncs_stringlist sourceAliasList = bncs_stringlist(p->command(), '_');
		int listPos = sourceAliasList[2].toInt() - 1;
		listButtonAlias[listPos] = p->value();
		int devIndex = listSourceIndex[listPos].toInt();
		bncs_string strLabel = getLabel(listPos, m_intDevice, DATABASE_SOURCE, devIndex);
		textPut("text", strLabel, PANEL_MAIN, bncs_string("source_%1").arg(listPos + 1));
	}
	else if (p->command() == "source")
	{
		int pVal = p->value().toInt();
		if (pVal >= 0 && pVal <= m_intRouterSize)
		{
			bncs_stringlist btnIds;
			for (int i = 0; i < listSourceIndex.count(); i++)
			{
				if (listSourceIndex[i].toInt() == pVal)
				{
					btnIds.append(bncs_string(i + 1));
				}
			}
			setButtonsState(btnIds);
		}
	}
	else if( p->command() == "layout" )
	{
		if(p->value().length() >0)
		{
			if (p->value().right(8) == ".bncs_ui")
			{
				m_strPanel = m_strPanel.left(p->value().length() - 8);
			}
			else
			{
				m_strPanel = p->value();
			}
		}
		else
		{
			m_strPanel = LAYOUT_DEFAULT;
		}
		debug("m_strPanel=%1", m_strPanel);
		panelDestroy(PANEL_MAIN);
		bncs_string strLayoutDirectory = LAYOUTS_SUB_DIR;
		bncs_string strLayout =  bncs_string("%1\\%2.bncs_ui").arg(strLayoutDirectory).arg(m_strPanel) ;
		panelShow(PANEL_MAIN, strLayout);
		setSize( PANEL_MAIN );				// set the size to the same as the specified panel

		// Reize lists based on num of buttons
		int numBtns = setListSizes();
		
		//debug("router_dest::parentCallback panel=%1", m_strPanel);

		//For design time assistance show the index and name of the configured dest
		int intRouter;
		getDev(m_strRouterInstance, &intRouter);
	}
	else if ( p->command() == "background_colour")
	{
		if (p->value().length() > 0 )
		{
			m_strBackgroundColour  = p->value();
			textPut("colour.background", m_strBackgroundColour  , PANEL_MAIN, "grpBackground");
		}
	}


	//Runtime "methods"
	else if( p->command() == "deselect" )
	{
		setButtonsState(0);
		//TODO getControlIDs into stringlist and update each button on panel
		//	for i .......
		//{
		//		debug("router_dest::parentCallback deselect");
		//		textPut("statesheet", m_strStatesheetDestDeselected, PNL_MAIN,  bncs_string("source_%1").arg(i));
		//			
		//{
	}

	//Return Commands
	else if( p->command() == "return" )
	{
		//Called by visual editor to persist settings in bncs_ui
		if(p->value() == "all")
		{
			bncs_stringlist slt;
			slt << bncs_string("layout=%1").arg(m_strPanel);
			slt << bncs_string("background_colour=%1").arg(m_strBackgroundColour);

			for (int i = 0; i < listSourceIndex.count(); i++)
			{
				slt << bncs_string( "source_index_%1=%2" ).arg(i + 1).arg(bncs_string(listSourceIndex[i]));
				slt << bncs_string( "source_alias_%1=%2" ).arg(i + 1).arg(bncs_string(listButtonAlias[i]));
			}
			return slt.toString('\n');
				
		
		}
		//Called by parent to learn the components settings
	}

	//Connections information for BNCS Vis Ed
	else if(p->command() == "_events")
	{
		bncs_stringlist sltEvents;
		sltEvents << "index=<index>";
		return sltEvents.toString( '\n' );
	}
	else if(p->command() == "_commands")
	{
		bncs_stringlist sltCommands;
		sltCommands << "source=[value]";
		return sltCommands.toString( '\n' );
	}

	return "";
}

// timer events come here
void router_shortcuts::timerCallback( int id )
{
}

bncs_string router_shortcuts::getLabel(int listLabelPos, int device, int database, int index)
{
	bncs_string strLabel;
	debug(":::::%1", listButtonAlias[listLabelPos]);
	if (listButtonAlias[listLabelPos].length() == 0)
	{
		routerName(device, database, index, strLabel);
	}
	else
	{
		strLabel = listButtonAlias[listLabelPos];
	}
	listButtonLabel[listLabelPos] = strLabel;
	return strLabel;
}

void router_shortcuts::setButtonsState(bncs_string btnId)
{
	bncs_stringlist sltBtns = getIdList(PANEL_MAIN, "source_");
	for (int i = 0; i < sltBtns.count(); i++)
	{
		textPut( "statesheet", "source_deselected", PANEL_MAIN, bncs_string("source_%1").arg(i + 1) );
	}
	textPut( "statesheet", "source_selected", PANEL_MAIN, btnId );
}

void router_shortcuts::setButtonsState(bncs_stringlist btnIds)
{
	bncs_stringlist sltBtns = getIdList(PANEL_MAIN, "source_");
	for (int i = 0; i < sltBtns.count(); i++)
	{
		if (btnIds.find(bncs_string(i + 1)) >= 0)
		{
			textPut( "statesheet", "source_selected", PANEL_MAIN, bncs_string("source_%1").arg(i + 1) );
		}
		else
		{
			textPut( "statesheet", "source_deselected", PANEL_MAIN, bncs_string("source_%1").arg(i + 1) );
		}
	}
	
}

int router_shortcuts::setListSizes()
{
	int numBtns = getIdList(PANEL_MAIN, "source_").count();
	listButtonAlias.setCount(numBtns);
	listButtonLabel.setCount(numBtns);
	listSourceIndex.setCount(numBtns);
	return numBtns;
}