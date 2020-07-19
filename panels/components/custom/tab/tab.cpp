#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "tab.h"

#define PNL_TABS	"tabs"

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( tab )

// constructor - equivalent to ApplCore STARTUP
tab::tab( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{

	// Init states/pixmaps
	m_selectedState      = "tab_selected";
	m_deselectedState    = "tab_deselected";
	m_bgSelectedPixmap   = "";
	m_bgDeselectedPixmap = "";
	m_firstTabShown		 = "tab_1";

	// show a panel from file p1.bncs_ui and we'll know it as our panel 1
	panelShow( PNL_TABS, "tabs/tab.bncs_ui" );

	// you may need this call to set the size of this component 
	//  if it's used in a popup window 
//	setSize( 1024,668 );		// set the size explicitly
//	setSize( 1 );				// set the size to the same as the specified panel
	m_layout = "tab";
}

// destructor - equivalent to ApplCore CLOSEDOWN
tab::~tab()
{
}

// all button pushes and notifications come here
void tab::buttonCallback( buttonNotify *b )
{
	//debug("tab: Button Commands recieved - b->panel():"+bncs_string(b->panel())+" b->command():"+bncs_string(b->command())+ " b->id():"+bncs_string(b->id())+ " b->value():"+bncs_string(b->value()));
	if( b->panel() == PNL_TABS )
	{
		if(m_tabList.find(b->id())>= 0)
		{
			debug( "tab: removing %1\n", m_lastTab );
			panelRemove(m_lastTab);
			deselectTab(m_lastTab);
			m_lastTab = b->id();
			selectTab(m_lastTab);
			m_lastUIShown = m_UIList[m_tabList.find(m_lastTab)];
			//debug( "tab: showing %1 / %2 from %3\n", m_lastTab, m_lastUIShown, m_UIList.toString(',') );
			panelShow(m_lastTab,panelPath(m_lastUIShown));
		}
	}
	else
	{	// all other panels
		if(b->command() == "alarm_state")
		{
			textPut("statesheet",b->value(), PNL_TABS,b->panel());
		}
		else
		{
			bncs_string subs;
			int i;
			for( i = 0; i < b->subs(); i++)
			{
				subs += bncs_string(".%1").arg( b->sub(i) );
			}
			hostNotify( bncs_string( "%1.%2%3.%4=%5" ).arg(b->panel()).arg(b->id()).arg(subs).arg(b->command()).arg(b->value() ));
			//debug( "tab: %1.%2%3.%4=%5\n", b->panel(), b->id(), subs, b->command(), b->value() );
		}
	}
}

// all revertives come here
int tab::revertiveCallback( revertiveNotify * r )
{
	return 0;
}

// all database name changes come back here
void tab::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string tab::parentCallback( parentNotify *p )
{
	bncs_string sSubs;
	for( int n = 0; n < p->subs(); n++ ) sSubs += bncs_string(".%1").arg( p->sub(n) );
	debug( "tab:  parentCallback: %1%2=%3\n", p->command(), sSubs, p->value() );

	if(p->command() == "instance")
	{
		m_instance = p->value();
	}
	else if(p->command() == "layout")
	{
		debug("tab: layout");
		if(m_layout != p->command())
		{
			m_layout = p->value();
			panelDestroy(PNL_TABS);
			panelShow(PNL_TABS,"tabs/"+m_layout + ".bncs_ui");
			m_tabList = getIdList(PNL_TABS,"tab");
			m_UIList.setCount(m_tabList.count());
			m_tabName.setCount(m_tabList.count());

			// Get statesheets/pixmaps from tabs 1 and 2
			textGet( "statesheet", PNL_TABS, "tab_1",    m_selectedState );
			textGet( "statesheet", PNL_TABS, "tab_2",    m_deselectedState );
			textGet( "pixmap",     PNL_TABS, "bg_tab_1", m_bgSelectedPixmap );
			textGet( "pixmap",     PNL_TABS, "bg_tab_2", m_bgDeselectedPixmap );

			for(int i = 0;i<m_tabList.count();i++)
			{
				deselectTab(m_tabList[i]);
			}
		}
	}
	else if(p->command() == "panel_location")
	{
		debug("tab: panelLocation");
		m_panelLocation = p->value();
	}
	else if(m_tabList.find(p->command()) >=0)
	{
		int index = m_tabList.find(p->command());
		debug("tab: tabList %1 %2=%3\n", index, p->command(), p->value());
		m_UIList[index] = p->value();
		//panelShow(2,"../../"+ m_panelLocation + "/"+ m_UIList[m_tabList.find(p->command())] +".bncs_ui");
		//panelTarget(p->command(),m_instance);
		//for(int i = 0;i<m_tabList.count();i++)
		//{
		//	debug(bncs_string("%1 =%2\n").arg(i).arg(m_tabList[i]));
		//	debug(bncs_string("%1 =%2\n").arg(i).arg(m_UIList[i]));
		//	//debug(bncs_string("%1").arg(m_tabList[i]));
		//}
	}
	else if(m_tabList.find(p->command().left(p->command().length()-5)) >=0)
	{	// tab_X_Name
		debug("tab: tabName %1=%2\n", p->command(), p->value());
		bncs_string m_tab = p->command().left(p->command().length()-5);
		int index = m_tabList.find(m_tab);
		m_tabName[index] = p->value();
		textPut("text",p->value(),PNL_TABS,m_tab);
		bncs_string panel = m_tabList[index];
		bncs_string panelUI = m_UIList[index];
		debug("tab: TAB panels to Show:%1 / %2\n", panel, panelPath(panelUI));
		panelShow(panel,panelPath(panelUI));
		panelTarget(panel,m_instance);
		panelRemove(panel);
		//debug("tab: TAB target panel:%1 with instance:%2",p->value(),m_instance);

		//for(int i = 0;i<m_tabList.count();i++)
		//{
		//	debug(bncs_string("%1 =%2\n").arg(i).arg(m_tabList[i]));
		//	debug(bncs_string("%1 =%2\n").arg(i).arg(m_UIList[i]));
		//	//debug(bncs_string("%1").arg(m_tabList[i]));
		//}
	}
	else if (p->command() == "first_tab_shown")
	{
		//debug("tab: first tab shown\n");
		m_firstTabShown  = p->value();
		panelRemove(m_lastTab);
		m_lastTab = m_firstTabShown;
		m_lastUIShown = m_UIList[m_tabList.find(m_firstTabShown)];
		//debug("tab: TAB FIRST panel to Show:%1",m_lastUIShown);
		panelShow(m_lastTab,panelPath(m_lastUIShown) );
		m_lastTab = m_tabList[m_tabList.find(m_firstTabShown)];
		selectTab(m_lastTab);
		//debug(bncs_string("first tab shown=%1\n").arg(m_firstTabShown));
		//debug(bncs_string("first tab shown=%1\n").arg(m_tabList.find(m_firstTabShown)));
		//debug(bncs_string("first tab shown=%1\n").arg(m_UIList[m_tabList.find(m_firstTabShown)]));
	}	

	else if( p->command() == "command" && p->subs() > 0)
	{	// CONNECTIONS commands from other components, in to our tabbed panels
		// as command.tab_X.id.sub=value
		// tab_X and id can also be 'all'
		int i;
		bncs_stringlist slSubs;
		bncs_string m_targetPanel = p->sub(0);
		bncs_string m_targetID    = p->sub(1);
		for( i = 2; i < p->subs(); i++ )
		{
			slSubs << p->sub(i);
		}
		if( m_tabList.find( m_targetPanel ) >= 0 )
		{
			textPutPanel( slSubs.toString('.'), p->value(), m_targetPanel, m_targetID );
		}
		else if( m_targetPanel == "all" )
		{
			bncs_stringlist::iterator it;
			for( it = m_tabList.begin(); it != m_tabList.end(); it++)
			{
				textPutPanel( slSubs.toString('.'), p->value(), *it, m_targetID );
			}
		}
	}

	else if(p->command()== "return")
	{
		if( p->value() == "all" )
		{
			debug("tab: return");
			bncs_stringlist ret;
			ret << "layout=" + m_layout;
			ret << "panel_location=" + m_panelLocation;
			int i, find;
			for( i = 1; i <= m_tabList.count(); i++ )
			{	// return tab list *in order* of name
				find = m_tabList.find( bncs_string( "tab_%1" ).arg(i) );
				ret << bncs_string("%1=%2").arg( m_tabList[find]).arg(m_UIList[find]);
				ret << bncs_string("%1_Name=%2").arg( m_tabList[find]).arg(m_tabName[find]);
			}

			ret << "first_tab_shown=" + m_firstTabShown;
			return ret.toString( '\n' );
		}
	}
	else if( p->command() == "_events" )
	{
		bncs_stringlist sl;
		bncs_stringlist::iterator it;
		for( it = m_tabList.begin(); it != m_tabList.end(); it++ )
		{
			sl << bncs_string( "%1.something=*" ).arg(*it);
		}
		return sl.toString('\n');
	}
	else if( p->command() == "_commands" )
	{
		bncs_stringlist sl;
		bncs_stringlist::iterator it;
		sl << bncs_string( "command.all.all.[command]=[value]" );
		for( it = m_tabList.begin(); it != m_tabList.end(); it++ )
		{
			sl << bncs_string( "command.%1.all.[command]=[value]" ).arg(*it);
		}
		return sl.toString('\n');
	}


	return "";
}

// timer events come here
void tab::timerCallback( int id )
{
}

bncs_string tab::panelPath(bncs_string panelName)
{
	return "../../../"+ m_panelLocation + "/"+ panelName +".bncs_ui";
}


void tab::selectTab( bncs_string m_tab )
{
	textPut( "pixmap",m_bgSelectedPixmap , PNL_TABS, "bg_"+m_tab );
	textPut( "statesheet", m_selectedState, PNL_TABS, m_tab );
}
void tab::deselectTab( bncs_string m_tab )
{
	textPut( "pixmap",m_bgDeselectedPixmap , PNL_TABS, "bg_"+m_tab );
	textPut( "statesheet", m_deselectedState, PNL_TABS, m_tab );
}

void tab::textPutPanel( bncs_string sProperty, bncs_string sValue, bncs_string sPanel, bncs_string sID )
{
	bncs_stringlist slIDs;
	bncs_stringlist::iterator it;
	if( sID != "all" )
	{
		textPut( sProperty, sValue, sPanel, sID );
	}
	else
	{
		slIDs = getIdList( sPanel );
		for( it = slIDs.begin(); it != slIDs.end(); it++ )
		{
			textPut( sProperty, sValue, sPanel, *it );
		}
	}
	return;
}