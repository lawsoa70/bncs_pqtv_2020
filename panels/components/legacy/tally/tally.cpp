#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "tally.h"

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( tally )

#define TIMER_INIT		1
#define TIMEOUT_INIT	100

// constructor - equivalent to ApplCore STARTUP
tally::tally( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	// show a panel from file p1.bncs_ui and we'll know it as our panel 1
	panelShow( 1, "p1.bncs_ui" );
	m_idatabase=0;
}

// destructor - equivalent to ApplCore CLOSEDOWN
tally::~tally()
{
}

// all button pushes and notifications come here
void tally::buttonCallback( buttonNotify *b )
{
}

// all revertives come here
int tally::revertiveCallback( revertiveNotify * r )
{
	int intSource = 0;

	if(m_sinstance.startsWith("package"))
	{
		bncs_stringlist sltTally = bncs_stringlist().fromString(r->sInfo());
		intSource = sltTally.getNamedParam("index").toInt();
	}
	else
	{
		intSource = r->info();
	}

	
	bncs_string sname;
	routerName(m_iinstance, m_idatabase, intSource, sname);
 
	if(m_sinstance.startsWith("package") && intSource == 0)
	{
		sname = "PARKED";
	}
	
	if (m_bsingleline)
 		sname.replace('|',' ');

	// 	debug("tally::revertiveCallback()dest=%1 tally=%2 - this.dest=%3 this.database=%4 this.iinstance=%5 name=%6", 
	// 		r->index(), r->info(), m_sdestindex, m_sdatabase, m_iinstance, sname);

	if(m_iinstance)
 		textPut("text",sname.replace('\'', '`'), 1,"tally");
 	else 
		textPut("text","-", 1,"tally");
	
	return 0;
}

// all database name changes come back here
void tally::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string tally::parentCallback( parentNotify *p )
{
	//debug("tally::parentCallback() cmd=%1 value=%2", p->command(), p->value());
	if( p->command() == "panel" )
	{
		m_spanelname=p->value();
		panelDestroy(1);
		panelShow(1,m_spanelname);
		
	}
	else if( p->command() == "instance" )
	{
		m_sinstance = p->value();
		
		int offset; // not needed
		getDev( m_sinstance, &m_iinstance, &offset );
		
		if(!m_iinstance)
			textPut("text","---",1,"tally");
		
		timerStart(TIMER_INIT, TIMEOUT_INIT);
	}
	 else if( p->command() == "database" )
	 {
		 m_sdatabase = p->value();
		 m_idatabase = p->value().toInt();
		 
		 
	 }
	 else if( p->command() == "singleline" )
	 {
		 m_ssingleline = p->value();
		 if(m_ssingleline=="TRUE"|| m_ssingleline=="true"||m_ssingleline=="yes"||m_ssingleline=="1"){
			 m_bsingleline=true;
		 }
		 else (	m_bsingleline=false);
		 
		 
	 }
	 
	 else if( p->command() == "destindex" )
	 {
		 //your specialsource is set!!!!!!!!arh
		 m_sdestindex = p->value();
		 /*	m_iscsindex=m_scsindex.toInt();
		 
		   bncs_string sName;
		   routerName(m_iinstance,0, m_iscsindex, sName);
		   textPut("text",sName,1,"source");
		   //what on earth does it think it is connected to on the other router 
		   //and what is routed to these destinations
		 Register_with_the_SourceRouter(m_iscsindex);*/
		 
		 timerStart(TIMER_INIT, TIMEOUT_INIT);
		 
	 }
	 else if( p->command() == "return" )
	 {
		 bncs_stringlist sl;		// a means of returning the string
		 
		 //	sl= "destindex=" + m_sdestindex;
		 sl= "panel=" + m_spanelname;
		 sl+= "destindex=" + m_sdestindex;
		 sl+= "database=" + m_sdatabase;
		 sl+= "singleline=" + m_ssingleline;
		 return sl.toString( '\n' );	// return our stringlist as newline delimited single string
	 }
	 
	 return "";
}

// timer events come here
void tally::timerCallback( int id )
{
	if(id == TIMER_INIT)
	{
		timerStop(TIMER_INIT);

		int j=m_sdestindex.toInt();
		if(m_iinstance &&j){
			if(m_sinstance.startsWith("package"))
			{
				infoRegister(m_iinstance,j,j);
				infoPoll(m_iinstance,j,j);
			}
			else
			{
				routerRegister(m_iinstance,j,j);
				routerPoll(m_iinstance,j,j);
			}
		}
	}
}