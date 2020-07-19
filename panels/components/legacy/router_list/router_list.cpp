#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "router_list.h"
#define PANEL_MAIN 1
#define KEYBOARD 2

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( router_list )

// constructor - equivalent to ApplCore STARTUP
router_list::router_list( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	m_database = -1;
	m_ExtraDatabase = -1;
	m_start = 0;
	m_end = 0;
	m_device = 0;
	m_indexOffset = 0;
	m_rangeAll = false;
	m_bSortAZ = false;
	m_bFilter = false;
	m_blnShowUnusedPorts = false;
	bncs_string m_inputSearch ="";
}

// all button pushes and notifications come here
void router_list::buttonCallback( buttonNotify *b )
{
	
	if(b->panel() == PANEL_MAIN){

		if( b->id() == "list" )
		{
			textPut( "colour.background=#bfbfbf", PANEL_MAIN, "selectNone" ); //decolour Select None button
			bncs_stringlist sl( b->value(), ':' );
			hostNotify( bncs_string( "index=%1").arg( sl[ 0 ].toInt() ));
		}
		else if( b->id() == "selectNone" )
		{
			textPut( "colour.background=#1bff13", PANEL_MAIN, "selectNone" ); //colour Select None button
			textPut( "selected=none", PANEL_MAIN, "list" );
			hostNotify( "index=0");
		}
		else if(b->id() == "sort")
		{
			m_bSortAZ = !m_bSortAZ;
			displayListbox(m_bSortAZ, m_bFilter, m_inputSearch);
		}
		else if (b->id() == "btnToggleUnused")
		{
			
			if(m_blnShowUnusedPorts)	{
				textPut("text", "Show|Unused Ports", PANEL_MAIN, "btnToggleUnused");
				m_blnShowUnusedPorts = false;
			}
			else	{
				textPut("text", "Hide|Unused Ports", PANEL_MAIN, "btnToggleUnused");
				m_blnShowUnusedPorts = true;
			}
			displayListbox(m_bSortAZ, m_bFilter, m_inputSearch);			
		}
		else if(b->id() == "btnSearch")
		{
			panelPopup(KEYBOARD,"keyboard.bncs_ui");
			textPut("setFocus", KEYBOARD, "keyboard");
			textPut("text", m_inputSearch, KEYBOARD, "keyboard");
			
		}
	}
	else if(b->panel() == KEYBOARD) {
		b->dump("keyboard");		
		if(b->command() == "button" && b->sub(0) == "return")
		{
			bncs_string temp_Input;
			textGet("text", KEYBOARD, "keyboard", temp_Input);
			if(temp_Input.length() != 0) 
			{
				m_inputSearch = temp_Input;
				m_bFilter = true;
				panelDestroy(KEYBOARD);
				textPut("text", m_inputSearch, PANEL_MAIN, "search_criteria");
				textPut("colour.background=#ffaa00",PANEL_MAIN, "btnSearch");		
				displayListbox(m_bSortAZ, m_bFilter, m_inputSearch);
			}
			else if(temp_Input.length() == 0)
			{
				m_inputSearch = temp_Input;
				m_inputSearch = "";
				m_bFilter = false;
				panelDestroy(KEYBOARD);
				textPut("text", m_inputSearch, PANEL_MAIN, "search_criteria");
				textPut("colour.background=#bfbfbf",PANEL_MAIN, "btnSearch");
				displayListbox(m_bSortAZ, m_bFilter, m_inputSearch);
			}

			
		}
		else if(b->id() == "Reset")
		{
			m_inputSearch = "";
			m_bFilter = false;
			panelDestroy(KEYBOARD);
			textPut("text", m_inputSearch, PANEL_MAIN, "search_criteria");
			textPut("colour.background=#bfbfbf",PANEL_MAIN, "btnSearch");
			displayListbox(m_bSortAZ, m_bFilter, m_inputSearch);
		}
		else if(b->id() == "Cancel")
		{
			panelDestroy(KEYBOARD);
		}
	}	
	
}

// all revertives come here
int router_list::revertiveCallback( revertiveNotify * r )
{
	return 0;
}

// all database name changes come back here
void router_list::databaseCallback( revertiveNotify * r )
{
	if( r->database() == m_database )
	{
		if( r->index() >= m_start )
		{
			if( r->index() <= m_end )
			{
				//bncs_string formattedName = bncs_string( "%1: %2" ).arg( r->index() + m_indexOffset , '0', 4 ).arg( r->sInfo() );
				//bncs_string entry = bncs_string( "%1: " ).arg( r->index()+ m_indexOffset, '0', 4 );
				
				//textPut( bncs_string( "replace.text.begins.%1=%2").arg( entry ).arg( formattedName ), 1, "list" );
				updateListbox( r->index()+ m_indexOffset, r->sInfo(), m_bSortAZ);
			}
		}
	}
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string router_list::parentCallback( parentNotify *p )
{
	if( p->command() == "instance" )
	{
		if( m_device )
			routerUnregister( m_device );
		
		m_instance = p->value();
		
		int offset;
		getDev( m_instance, &m_device, &offset );
		
		// it doesn't matter what we register with....so long as it's something so as to get router name changes
		if( m_device )
			routerRegister( m_device, 4096, 4096 );
		
		timerStart( 1, 1 );
	}
	else if( p->command() == "database" )
	{
		m_database = p->value();
		timerStart( 1, 1 );
	}
	else if( p->command() == "panel" )
	{
		m_panel = p->value();
		
		panelShow( 1, m_panel + ".bncs_ui" );
		setSize( 1 );				// set the size to the same as the specified panel
		timerStart( 1, 1 );
	}
	if(p->command() == "ExtraDatabase")
	{
		if(p->value().length() != 0)
		{
			m_ExtraDatabase = p->value();
		}
		else
		{
			m_ExtraDatabase = -1;
		}
	}
	else if( p->command() == "range" )
	{
		if( p->value() == "all" )
		{
			m_start = 1;
			m_end = getRouterSize( m_device, m_database );
			m_rangeAll = true;
		}
		else
		{
			bncs_stringlist sl( p->value(), '-' );
			
			m_start = sl[ 0 ];
			m_end = sl[ 1 ];
			m_rangeAll = false;
		}
		
		timerStart( 1, 1 );
	}
	else if( p->command() == "selected" )
	{
		// this is a small workaround because listbox treats "next" as first in the list if nothing is selected
		bncs_string ret;
		textGet( "selectedindex", 1, "list", ret );
		
		if( (int) ret > -1 )
		{
			if( p->value() == "next" )
				textPut( "selected=next", 1, "list" );
			else if( p->value() == "previous" )
				textPut( "selected=previous", 1, "list" );
			else if( p->value() == "none" )
			{
				textPut( "selected=none", 1, "list" );
				hostNotify( "index=0");
			}
		}
	}
	else if( p->command() == "offset" )
	{
		m_indexOffset = p->value();
	}
	else if( p->command() == "_events" )
	{
		bncs_stringlist sl;
		
		sl << "index=<router index>";
		
		return sl.toString( '\n' );
	}
	else if( p->command() == "_commands" )
	{
		bncs_stringlist sl;
		
		sl << "selected=next";
		sl << "selected=previous";
		sl << "selected=none";
		
		return sl.toString( '\n' );
	}
	else if( p->command() == "return" )
	{
		bncs_stringlist sl;
		
		sl << bncs_string( "database=%1" ).arg( m_database );
		sl << bncs_string("ExtraDatabase=%1").arg(m_ExtraDatabase);
		if( m_rangeAll )
			sl << "range=all";
		else
			sl << bncs_string( "range=%1-%2" ).arg( m_start ).arg( m_end );
		sl << "panel=" + m_panel;
		
		sl << bncs_string( "offset=%1" ).arg( m_indexOffset );
		
		return sl.toString( '\n' );
	}
	return "";
}

// timer events come here
void router_list::timerCallback( int id )
{
	timerStop( id );
	load();
}


void router_list::load( void )
{
	if( m_database < 0 )
		return;
	
	if( m_rangeAll )
	{
		m_start = 1;
		m_end = getRouterSize( m_device, m_database );
	}
	
	textPut( "clear", 1, "list" );
	int intMaxLength = bncs_string(m_end).length();
	
	for( int x = m_start ; x <= m_end ; x++ )
	{
		bncs_string name;
		
		routerName( m_device, m_database, x, name );
		if(m_ExtraDatabase >= 0)
		{
			bncs_string extraDB;
			routerName( m_device, m_ExtraDatabase, x, extraDB );
			if(extraDB.length() > 0 && extraDB != " ")
				name = name + " - " + extraDB;
		}
		//Special feature for Riedel Artist Database
		if(m_instance.find("riedel") > -1)
		{
			//Get the port type
			bncs_string strPortType;
			routerName( m_device, 9, x, strPortType );
			if(strPortType == "3" )
			{ 
				name = name + " (split)";
			}
			else if(strPortType == "4")
			{
				name = name + "  (4w)";
			}
			else if(strPortType == "5" || strPortType == "6")
			{
				name = name + "  (panel)";
			}
		}

		m_mSortedAZ.insert(pair<bncs_string,bncs_string>(bncs_string(name).simplifyWhiteSpace(),bncs_string("%1").arg( x + m_indexOffset, '0', intMaxLength)));
		m_mSortedIndex.insert(pair<bncs_string,bncs_string>(bncs_string("%1").arg( x + m_indexOffset, '0', intMaxLength),bncs_string(name).simplifyWhiteSpace()));
	}
	displayListbox(m_bSortAZ, m_bFilter, m_inputSearch);
	
	//bncs_string formattedName = bncs_string( "%1: %2" ).arg( x + m_indexOffset, '0', 4).arg( name );
	
	//textPut( "add", formattedName, 1, "list" );
}



void router_list::updateListbox( bncs_string index, bncs_string newText, bool bSortAZ )
{
	int intMaxLength = bncs_string(m_end).length();
	index = bncs_string("%1").arg(index, '0', intMaxLength);
	bncs_string oldText;
	oldText = m_mSortedIndex.find(index)->second;
	m_mSortedIndex.erase(index);
	m_mSortedIndex.insert(pair<bncs_string,bncs_string>(bncs_string(index),bncs_string(newText).simplifyWhiteSpace()));
	
	multimap<bncs_string,bncs_string>::iterator it =m_mSortedAZ.find(oldText);
	if( it != m_mSortedAZ.end() )
	{
		do
		{
			if(it->second == index)
			{
				m_mSortedAZ.erase(it);
				m_mSortedAZ.insert(pair<bncs_string,bncs_string>(bncs_string(newText).simplifyWhiteSpace(),bncs_string(index)));
				break;
			}
			
		} while( ++it != m_mSortedAZ.end() );
	}
	
	bncs_string formattedName = bncs_string( "%1: %2" ).arg(index).arg(newText);
	if(bSortAZ)
	{
		//If text is sorted A to Z refill the listbox, if an item was selected select it again, if not select the item that was selected before
		bncs_string retIndex, retText;
		textGet( "selectedindex", 1, "list", retIndex );
		textGet("selected", 1, "list", retText);
		bncs_stringlist sl( retText, ':' );
		int iRetIndex = retIndex.toInt();
		retIndex = sl[0];
		retText = sl[1].simplifyWhiteSpace();
		//debug(bncs_string("router_list::updateListbox retText=%1 retIndex=%2 iRetIndex=%3").arg(retText).arg(retIndex).arg(bncs_string(iRetIndex)));
		//debug(bncs_string("router_list::updateListbox oldText=%1 newText=%2 retText=%3").arg(oldText).arg(newText).arg(retText));
		displayListbox(bSortAZ, m_bFilter, m_inputSearch);
		
		if(iRetIndex > -1)
		{
			//debug(bncs_string("router_list::0"));
			if (retIndex == index)
			{
				//debug(bncs_string("router_list::1"));
				textPut(bncs_string("selected.text.exact=%1").arg(formattedName),1,"list");
			}
			else if(oldText < retText)
			{
				debug(bncs_string("router_list::2"));
				if(retText < newText )
				{
					iRetIndex--;
					textPut( bncs_string("selected.id=%1").arg(iRetIndex), 1, "list" );	
				}
				else
				{
					textPut( bncs_string("selected.id=%1").arg(iRetIndex), 1, "list" );	
				}
			}
			else if(retText < oldText)
			{
				//debug(bncs_string("router_list::3"));
				if(newText < retText)
				{
					iRetIndex++;
					textPut( bncs_string("selected.id=%1").arg(iRetIndex), 1, "list" );
				}
				else
				{
					textPut( bncs_string("selected.id=%1").arg(iRetIndex), 1, "list" );	
				}
			}		
		}
	}
	else
	{
		textPut( bncs_string( "replace.text.begins.%1=%2").arg(index).arg( formattedName ), 1, "list" );		
	}
}


void router_list::displayListbox(bool bSortAZ, bool bFilter, bncs_string m_inputSearch)
{
	/*textPut( "clear", 1, "list" );
	if(bSortAZ){
		for(multimap<bncs_string,bncs_string>::iterator it =m_mSortedAZ.begin(); it !=m_mSortedAZ.end(); ++it)
		{
			if (m_blnShowUnusedPorts == false) {
				if(it->first.find("N_U_")==-1) textPut( "add", it->second + ": " + it->first, 1, "list" );
			}
			else textPut( "add", it->second + ": " + it->first, 1, "list" ); // else it means m_blnShowUnusedPorts was true
			}
		textPut("text", "Numeric|Sort", 1, "sort");
		}
	else {
		for(multimap<bncs_string,bncs_string>::iterator it =m_mSortedIndex.begin(); it !=m_mSortedIndex.end(); ++it)
		{
			if (m_blnShowUnusedPorts == false) {
				if(it->second.find("N_U_")==-1) textPut( "add", it->first + ": " + it->second, 1, "list" );
			}
			else textPut( "add", it->first + ": " + it->second, 1, "list" ); // else it means m_blnShowUnusedPorts was true
		}			
		textPut("text", "Alpha|Sort", 1, "sort");
	}*/

	bncs_string sFilter = m_inputSearch;
	bncs_string firstUpper;
	bncs_string secondUpper;
	bncs_string unusedPorts = "N_U_";
	textPut( "clear", 1, "list" );
	
	if(bSortAZ){
		for(multimap<bncs_string,bncs_string>::iterator it =m_mSortedAZ.begin(); it !=m_mSortedAZ.end(); ++it)
		{
			firstUpper = it->first;
			secondUpper = it->second;

			if (m_blnShowUnusedPorts == true && bFilter == true) {
				if(firstUpper.upper().find(sFilter.upper())!=-1) textPut( "add", it->second + ": " + it->first, 1, "list" );
			}
			else if (m_blnShowUnusedPorts == true && bFilter == false) {
				textPut( "add", it->second + ": " + it->first, 1, "list" );
			}
			else if (m_blnShowUnusedPorts == false && bFilter == true) {
				if(firstUpper.find(unusedPorts)==-1 && firstUpper.upper().find(sFilter.upper())!=-1) textPut( "add", it->second + ": " + it->first, 1, "list" );
			}		
			else {
				if(firstUpper.find(unusedPorts)==-1) textPut( "add", it->second + ": " + it->first, 1, "list" );
			}
		}
		textPut("text", "Numeric|Sort", 1, "sort");
	}
	else {
		for(multimap<bncs_string,bncs_string>::iterator it =m_mSortedIndex.begin(); it !=m_mSortedIndex.end(); ++it)
		{
			firstUpper = it->first;
			secondUpper = it->second;
		
			if (m_blnShowUnusedPorts == true && bFilter == true) {
				if (secondUpper.upper().find(sFilter.upper())!=-1) textPut( "add", it->first + ": " + it->second, 1, "list" );
			}
			else if (m_blnShowUnusedPorts == true && bFilter == false) {
				textPut( "add", it->first + ": " + it->second, 1, "list" );
			}
			else if (m_blnShowUnusedPorts == false && bFilter == true) {
				if(secondUpper.find(unusedPorts)==-1 && secondUpper.upper().find(sFilter.upper())!=-1) textPut( "add", it->first + ": " + it->second, 1, "list" );
			}		
			else {
				if(secondUpper.find(unusedPorts)==-1) textPut( "add", it->first + ": " + it->second, 1, "list" );
			}
		}				
		textPut("text", "Alpha|Sort", 1, "sort");
	}
}