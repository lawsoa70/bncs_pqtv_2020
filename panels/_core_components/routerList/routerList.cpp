#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "routerList.h"

#define PNL_MAIN	1

#define TIMER_SETUP	1


// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( routerList )

// constructor - equivalent to ApplCore STARTUP
routerList::routerList( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	// show a panel from file p1.bncs_ui and we'll know it as our panel PNL_MAIN
	panelShow( PNL_MAIN, "p1.bncs_ui" );

	m_device = 0;
	m_database = -1;
	m_sort = false;
	m_remove_begins = "-1";
	filtered = 0;
	

}

// destructor - equivalent to ApplCore CLOSEDOWN
routerList::~routerList()
{
}

// all button pushes and notifications come here
void routerList::buttonCallback( buttonNotify *b )
{
	// cursary check to see that we're passing messages on from our listbox component
	if( b->id() == "Listbox" )
	{
		//  hostNotify( bncs_string( "%1.%2=%3" ).arg( b->command() ).arg( b->sub(0) ).arg( b->value() ));

		if (m_sort)
		{
			 int index = routerIndex(m_device, m_database, b->value());
		
			hostNotify(bncs_string("index=%1").arg(index));
		}
		
		else if (filtered)
		{
			int index = routerIndex(m_device, m_database, b->value());
		
			hostNotify(bncs_string("index=%1").arg(index));
		}
		
		else // If not filtered or sorted it is in router order so use index from the listbox + 1 as list counts from 0
		{
			int index = b->sub( 0 );
			hostNotify( bncs_string( "index=%1").arg( index + 1 ));
		}
	}
}

// all revertives come here
int routerList::revertiveCallback( revertiveNotify * r )
{
	return 0;
}

// all database name changes come back here
void routerList::databaseCallback( revertiveNotify * r )
{
	if( r->database() == m_database )
	{
		if( m_sort )
		{
			// sorted - so just go reload after a bit (this ensures that we don't reload multiple times if there's multiple changes)
			timerStart( TIMER_SETUP, 1 );
		}
		else
		{
			// note that listbox indexing is from zero - we can only practically slip changes in when not sorted
			bncs_string s = bncs_string( "replace.id.%1=%2" ).arg( r->index() -1 ).arg( r->sInfo() );
			textPut( s, 1, "Listbox" );
		}
	}
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string routerList::parentCallback( parentNotify *p )
{
	if( p->command() == "return" )
	{
		if( p->value() == "all" )
		{	// Persisting values for bncs_vis_ed
			bncs_stringlist sl;
			
			sl << bncs_string( "database=%1" ).arg( m_database );
			sl << bncs_string( "sort=%1" ).arg( m_sort );
			sl << bncs_string( "remove_begins=%1" ).arg( m_remove_begins );
			
			return sl.toString( '\n' );
		}
	}
	else if( p->command() == "instance" && p->value() != m_instance )
	{	// Our instance is being set/changed
		m_instance = p->value();

		int offset; // not used (typically) for routers
		getDev( m_instance, &m_device, &offset );

		loadListbox();
	}
	else if( p->command() == "database" )
	{	
		m_database = p->value();

		loadListbox();
	}
	else if( p->command() == "sort" )
	{	
		m_sort = atoi( p->value())?true:false;

		if( m_sort )
			textPut( "sort=true", 1, "Listbox" );
		else
		{
			// simply setting the sorting parameter is not enough - we have to reload the listbox to get it back in index order
			textPut( "sort=false", 1, "Listbox" );
			loadListbox();
		}
	}
	else if (p->command() == "size")
	{
		// when you send size alone it's relative to the base size (so can be negative)
		int size = atoi(p->value());

		if (size)
			textPut("font.size", size, 1, "Listbox");
	}
	else if (p->command() == "remove_begins")
	{
			// when you send remove_begins it removes all entries starting with the supplied string
			bncs_string find = (p->value());
			m_remove_begins = (p->value()); // store the updated seting in variable so that it stays put
				
			if (find == -1)
				{
					loadListbox(); // -1 represents Un filtered so re load listbox
				}
			else
				{
				textPut("remove.text.begins.all",find,  1, "Listbox");
				filtered = 1;
				}
			

	}

	else if (p->command() == "remove_exact")
	{
		// when you send remove _exact it removes all entries that match the supplied string (not case sensitive)
		bncs_string find = (p->value());

		if (find == -1) // -1 represents Un filtered so re load listbox
			{
			loadListbox();
			}

		else
			{
			textPut("remove.text.exact.all", find, 1, "Listbox");
			filtered = 1;
			}

		

	}

	else if (p->command() == "deselect")
	{
		// when you send deselect it removes the current selection from the listbox
					
			textPut("selected=none", 1, "Listbox");
			textPut("text", "cat", 1, 100);
			
	}

	else if (p->command() == "select")
	{
		// when you send select it Selects the text requiired if it is present
		bncs_string find_highlight = (p->value());
				
		textPut("selected.text.exact" ,find_highlight, 1, "Listbox");

	}

	// ***** CONNECTIONS EVENTS HELPER LIST *****
	else if( p->command() == "_events" )
	{	// Helper-list of everything in this component generated by hostNotify's
		bncs_stringlist sl;

		sl << "selection.[id]=[value]";
		
		return sl.toString( '\n' );
	}

	// ***** CONNECTIONS COMMANDS HELPER LIST *****
	else if( p->command() == "_commands" )
	{	// Helper-list of any commands/parameters you might want to set at run-time
		bncs_stringlist sl;
		
		sl << "database=[value]";
		sl << "sort=[value]";
		sl << "size=<relative size>";
		sl << "remove_begins=[value]";
		sl << "remove_exact=[value]";
		sl << "deselect";
		sl << "select=[text]";
		
		return sl.toString( '\n' );
	}

	return "";
}

// timer events come here
void routerList::timerCallback( int id )
{
	switch( id )
	{
	case TIMER_SETUP:
		timerStop(id);
		loadListbox();
		break;

	default:	// Unhandled timer event
		timerStop(id);
		break;
	}
}


void routerList::loadListbox( void )
{
	filtered = 0;
	textPut( "clear", 1, "Listbox" );
	if( m_device )
		routerUnregister( m_device );

	if( m_device && (m_database >= 0 ))
	{
		routerRegister( m_device, 4094, 4094 );
		int size = getRouterSize( m_device, m_database );

		for( int x = 1 ; x <= size ; x++ )
		{
			bncs_string name;
			routerName( m_device, m_database, x, name );

			textPut( "add", name, 1, "Listbox" );
		}
	}
}
