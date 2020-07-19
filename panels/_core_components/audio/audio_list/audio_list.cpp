#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "audio_list.h"

/*
<BNCSDOCS_ABOUT> Audio List
A layout configurable source or destination list component
for use with mapping buttons on audio router panels

Sorting can be alphabetical or by numeric index
</BNCSDOCS_ABOUT>
*/

/*
<BNCSDOCS_ABOUT> History
This component was originally developed with a namespace function
That allows named grouping of sources or destinations

That functionality has been disabled in this version as it is not required
</BNCSDOCS_ABOUT>
*/

/*
<BNCSDOCS_DEVELOPER>
Timers are used after values are set to allow the component to load correctly with all parameters present
</BNCSDOCS_DEVELOPER>
*/

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( audio_list )

#define PANEL_MAIN 1		// <BNCSDOCS_USE>Main panel id number</BNCSDOCS_USE>
#define CLEAR_INDEX 0		// <BNCSDOCS_USE>index used to clear a selection</BNCSDOCS_USE>
#define TIMER_INIT 1		// <BNCSDOCS_USE>Setup timer id number - ensures all commands are processed before setup starts</BNCSDOCS_USE>
#define TIMER_INIT_LEN 1	// <BNCSDOCS_USE>Setup timer delay value</BNCSDOCS_USE>

// #define FLOOR_START		2048	// <BNCSDOCS_USE>Unused</BNCSDOCS_USE>
// #define FLOOR_SIZE		512		// <BNCSDOCS_USE>Unused</BNCSDOCS_USE>
#define MAX_SRC_DEST	4096	// <BNCSDOCS_USE>maximum number of sources or destinations</BNCSDOCS_USE>

#define PARK_INDEX	"04096"		// <BNCSDOCS_USE>index for park source</BNCSDOCS_USE>
#define PARK_TEXT	"Silence"	// <BNCSDOCS_USE>text name for park source</BNCSDOCS_USE>

// constructor - equivalent to ApplCore STARTUP
audio_list::audio_list( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	// show a panel from file p1.bncs_ui and we'll know it as our panel 1
	panelShow( PANEL_MAIN, "audio_list.bncs_ui" );

	m_sInstance="";
	m_intDBAudioLeg=0;
	m_intDevice=0;
	m_blnAlphaSort=false;
	m_blnShowUnusedPorts = false;
	m_bAreaSelected = false;

	m_intDBIndexNames = 0;
	m_intDBIndex = 0;
	m_intDBAudioLeg = 0;
	m_intDBAreas = 0;

	m_sSilence_Text = "";
	m_sSilence_Index = "";

	m_bAllNameSpacesListed = true;
	m_slListedNameSpaces.fromString("all",',');
	m_sSelectedNamespace = "";
	m_slSelectableLegs.fromString("L,R,M,1,2,3,4,5,6,7,8",',');
	m_slAllNamespaces.fromString("S,G1,G2,G3,G4,G5,G6,G7,G8,F0,F1,F2,F3,F4,F5,F6,FN,F7,F8,T,C,E,L,X,Y",',');

	m_iSelectedNamespaceButton = 0;
	// you may need this call to set the size of this component 
	//  if it's used in a popup window 
//	setSize( 1024,668 );		// set the size explicitly
//	setSize( 1 );				// set the size to the same as the specified panel
}

// destructor - equivalent to ApplCore CLOSEDOWN
audio_list::~audio_list()
{
}

// all button pushes and notifications come here
void audio_list::buttonCallback( buttonNotify *b )
{

	bncs_stringlist sltButtonID = bncs_stringlist(b->id(), '_');
	
	/*
	if(sltButtonID[0] == "Jump")		{
		int intRouterButton = sltButtonID[1].toInt();
			selectArea(intRouterButton-1);
		}
	else*/ if( b->id() == "list")
	{
		if( b->command() == "selection") 
		{
			if(m_iIndexNo == CLEAR_INDEX)
				textPut("statesheet", "enum_deselected",PANEL_MAIN,"park");

			bncs_string strListItem = b->value();
			bncs_string strListIndex;
			if (m_blnAlphaSort==false) {
				strListIndex = strListItem.left(5);                         // rtr index
				strListItem = strListItem.right(strListItem.length() - 6);  // name
			}
			else {
				if(bncs_string(" %1").arg(PARK_TEXT) == strListItem)
				{
					strListIndex = PARK_INDEX;
				}
				else
				{
					strListIndex = dbm.getIndex(m_intDevice, m_intDBIndexNames, strListItem );
				 //routerName(m_intDevice, m_intDBAudioLeg, strListItem, strListIndex);		
				}
			}
			
			m_iIndexNo = strListIndex.toInt();
			debug("audio_list::buttonCallback selected name=%1 from (%2) index=%3 dbsrcnum=%4 \n", strListItem, b->value(), strListIndex, m_iIndexNo);
			bncs_string audioType = showIndex(m_iIndexNo);
			hostNotify(bncs_string("audioType=%1").arg(audioType));
			hostNotify( bncs_string( "index=%1").arg( m_iIndexNo ) );
			
		}
	}
	else if( b->id() == "park")
	{
		parkSelected();
	}
	else if(b->id() == "sort")	{
		if(m_blnAlphaSort)	{
			m_blnAlphaSort = false;
			textPut("text", "ALPHA|SORT", PANEL_MAIN, "sort");
		}
		else	{
			m_blnAlphaSort = true;
			textPut("text", "NUMERIC|SORT", PANEL_MAIN, "sort");
		}
		loadNames(true);
	}
}

// all revertives come here
int audio_list::revertiveCallback( revertiveNotify * r )
{
	return 0;
}

// all database name changes come back here
void audio_list::databaseCallback( revertiveNotify * r )
{
	debug("audio_xy_list::databaseCallback device=%1 database %2 dindx %3 sinfo %4\n", r->device(), r->database(),r->index(), r->sInfo() );
	if( r->device() == m_intDevice)	{
		if ((r->database()==m_intDBIndexNames)||(r->database()==m_intDBAreas)||(r->database()==m_intDBAudioLeg)||(r->database()==m_intDBIndex)) {
			dbm.setName(m_intDevice, r->database(), r->index(), LPCSTR(r->sInfo()),  false );
			if ( r->database() == m_intDBIndexNames) loadNames( true );
		}
	}
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string audio_list::parentCallback( parentNotify *p )
{
	if(p->command() == "instance")
		// <BNCSDOCS_TYPE>Mandatory</BNCSDOCS_TYPE>
		// <BNCSDOCS_USE>Sets the instance of the router</BNCSDOCS_USE>
	{
		m_sInstance = p->value();
		if(m_sInstance.length())
		{
			getDev(m_sInstance, &m_intDevice);
			infoRegister(m_intDevice,1,1);
		}
		else m_intDevice = 0;
		timerStart(TIMER_INIT,TIMER_INIT_LEN);
	}
	else if(p->command() == "DB_index")
		// <BNCSDOCS_TYPE>Mandatory</BNCSDOCS_TYPE>
		// <BNCSDOCS_USE>Sets the BNCS database number, 0 for sources and 1 for dests</BNCSDOCS_USE>
	{
		m_intDBIndex = p->value();
		timerStart(TIMER_INIT,TIMER_INIT_LEN);
	}
	else if(p->command() == "DB_index_names")
		// <BNCSDOCS_TYPE>Mandatory</BNCSDOCS_TYPE>
		// <BNCSDOCS_USE>Sets the BNCS database number for names in the listbox</BNCSDOCS_USE>
	{
		m_intDBIndexNames = p->value();
		timerStart(TIMER_INIT,TIMER_INIT_LEN);
	}
	else if(p->command() == "DB_index_areas")
		// <BNCSDOCS_TYPE>Unused</BNCSDOCS_TYPE>
		// <BNCSDOCS_USE>Not used</BNCSDOCS_USE>
	{
		m_intDBAreas = p->value();
		timerStart(TIMER_INIT,TIMER_INIT_LEN);
	}
	else if(p->command() == "DB_audio_leg" )
		// <BNCSDOCS_TYPE>Mandatory</BNCSDOCS_TYPE>
		// <BNCSDOCS_USE>Sets the BNCS database number for audio leg information</BNCSDOCS_USE>
	{
		m_intDBAudioLeg = p->value();
		timerStart(TIMER_INIT,TIMER_INIT_LEN);
	}
	else if(p->command() == "Listed_Namespaces")
		// <BNCSDOCS_TYPE>Unused</BNCSDOCS_TYPE>
		// <BNCSDOCS_USE>Not used</BNCSDOCS_USE>
	{
		/*
		for( bncs_stringlist::iterator it = m_slAllNamespaces.begin() ; it != m_slAllNamespaces.end() ; ++it )
		{
			controlDisable( PANEL_MAIN,bncs_string("Jump_%1").arg(getRouterFromArea(*it)+1) );
		}
		if(p->value().length() < 1||p->value().lower() == "all")
		{
			m_bAllNameSpacesListed = true;
			m_slListedNameSpaces =m_slAllNamespaces;
			for( bncs_stringlist::iterator it = m_slAllNamespaces.begin() ; it != m_slAllNamespaces.end() ; ++it )
			{
                controlEnable( PANEL_MAIN,bncs_string("Jump_%1").arg(getRouterFromArea(*it)+1) );
			}
		}
		else
		{
			m_bAllNameSpacesListed = false;
			m_slListedNameSpaces.fromString(p->value().upper(),',');
			for( bncs_stringlist::iterator it = m_slListedNameSpaces.begin() ; it != m_slListedNameSpaces.end() ; ++it )
			{
                controlEnable( PANEL_MAIN,bncs_string("Jump_%1").arg(getRouterFromArea(*it)+1) );
			}		
		}
		timerStart(TIMER_INIT,TIMER_INIT_LEN);
		*/
	}
	else if(p->command() == "Selected_Namespace")
		// <BNCSDOCS_TYPE>Unused</BNCSDOCS_TYPE>
		// <BNCSDOCS_USE>Not used</BNCSDOCS_USE>
	{
		if(p->value().length() < 1|| p->value().lower() == "all")
		{
			m_sSelectedNamespace = "all";
		}
		else
		{
			m_sSelectedNamespace = p->value();
		}
		timerStart(TIMER_INIT,TIMER_INIT_LEN);
	}
	else if(p->command() == "Useable_Legs")
		// <BNCSDOCS_TYPE>Mandatory</BNCSDOCS_TYPE>
		// <BNCSDOCS_USE>A comma deliminated list of the audio legs that can be used in the selection</BNCSDOCS_USE>
	{
		m_slSelectableLegs = p->value();
	}
	else if(p->command() == "Show_Unused_Ports")
		// <BNCSDOCS_TYPE>Mandatory</BNCSDOCS_TYPE>
		// <BNCSDOCS_USE>
		// If set to false then any source or destination with a blank database name
		// (length of 0 or starts with "N_U_" or contains "---" or "!!!")
		// will not show in the list
		// </BNCSDOCS_USE>
	{
		m_blnShowUnusedPorts = p->value() == "true"?true:false;
		timerStart(TIMER_INIT,TIMER_INIT_LEN);
	}
	else if(p->command() == "Alpha_Sorted")
		// <BNCSDOCS_TYPE>Mandatory</BNCSDOCS_TYPE>
		// <BNCSDOCS_USE>
		// If set to true then the source or destination list will be ordered alphabetically
		// otherwise in numerical index order
		// </BNCSDOCS_USE>
	{
		m_blnAlphaSort = p->value()== "true"?true:false;
		timerStart(TIMER_INIT,TIMER_INIT_LEN);
	}
	else if(p->command() == "Silence_Index")
		// <BNCSDOCS_TYPE>Mandatory</BNCSDOCS_TYPE>
		// <BNCSDOCS_USE>Sets the index for silence</BNCSDOCS_USE>
	{
		//Set the Index for Silence to be shown in all Areas at the top of the panel
		if(m_sSilence_Index != p->value())
		{
			m_sSilence_Index  = bncs_string(p->value().toInt(),'0',5);
			timerStart(TIMER_INIT,TIMER_INIT_LEN);
		}
		
	}
	else if(p->command() == "selected")
		// <BNCSDOCS_TYPE>
		// Mandatory
		// Nominall at runtime</BNCSDOCS_TYPE>
		// <BNCSDOCS_USE>
		// Controls the selected index
		// Valid options are "next", "previous" or "none"
		// and will action that selection in the listbox
		// </BNCSDOCS_USE>
	{
		bncs_string ret;
		textGet( "selectedindex", PANEL_MAIN, "list", ret );
		
		if( (int) ret > -1 )
		{
			if( p->value().lower() == "next" )
				textPut( "selected=next", 1, "list" );
			else if( p->value().lower() == "previous" )
				textPut( "selected=previous", 1, "list" );
			else if( p->value().lower() == "none" )
			{
				parkSelected();
			}
		}
		else if( p->value().lower() == "none" )
		{
			parkSelected();
		}
	}
	else if( p->command() == "_events" )
	{
		bncs_stringlist sl;
		
		sl << "index=<Router Index>";
		
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
	else if(p->command() =="return")
	{
		bncs_stringlist sl;
		sl << bncs_string("DB_index=%1").arg(m_intDBIndex);
		sl << bncs_string("DB_index_names=%1").arg(m_intDBIndexNames);
		sl << bncs_string("DB_index_areas=%1").arg(m_intDBAreas);
		sl << bncs_string("DB_audio_leg=%1").arg(m_intDBAudioLeg);
		sl << bncs_string("Listed_Namespaces=%1").arg(m_slListedNameSpaces);
		sl << bncs_string("Selected_Namespace=%1").arg(	m_sSelectedNamespace);
		sl << bncs_string("Useable_Legs=%1").arg(	m_slSelectableLegs);
		sl << bncs_string("Show_Unused_Ports=%1").arg(m_blnShowUnusedPorts?"true":"false");
		sl << bncs_string("Alpha_Sorted=%1").arg(m_blnAlphaSort?"true":"false");
		sl << bncs_string("Silence_Index=%1").arg(m_sSilence_Index);
		return sl.toString('\n');
	}
	return "";
}

// timer events come here
void audio_list::timerCallback( int id )
{
	
	if(id = TIMER_INIT)
	{
		timerStop(TIMER_INIT);
		//If Silence has been selected set the Text for Silence
		m_sSilence_Text = dbm.getName(m_intDevice, m_intDBIndex, m_sSilence_Index.toInt() );

		if(m_sSelectedNamespace == "all")
			selectArea(17);
		else
			selectArea(getRouterFromArea(m_sSelectedNamespace));
	}
}


bncs_string audio_list::showIndex(int intIndex)
{
	if (intIndex > 0)	{
		textPut("text", intIndex, PANEL_MAIN, "indexNo" );
	}
	else	{
		textPut("text", "", PANEL_MAIN, "indexNo" );
	}
	// having got the source show audio leg
	for (int i=1;i<10;i++) textPut("colour.background", "#888888", PANEL_MAIN, bncs_string("Leg%1").arg(i));
	bncs_string s = dbm.getName(m_intDevice, m_intDBAudioLeg, intIndex );
	//routerName(m_intDevice, m_intDBAudioLeg, intIndex , s);
	s = s.upper();
	if (s=="L") textPut("colour.background", "#eeee77", PANEL_MAIN, "Leg1" );
	else if (s=="R") textPut("colour.background", "#eeee77", PANEL_MAIN, "Leg2" );
	else if (s=="M") textPut("colour.background", "#eeee77", PANEL_MAIN, "Leg3" );
	else if (s=="1") textPut("colour.background", "#eeee77", PANEL_MAIN, "Leg4" );
	else if (s=="2") textPut("colour.background", "#eeee77", PANEL_MAIN, "Leg5" );
	else if (s=="3") textPut("colour.background", "#eeee77", PANEL_MAIN, "Leg6" );
	else if (s=="4") textPut("colour.background", "#eeee77", PANEL_MAIN, "Leg7" );
	else if (s=="5") textPut("colour.background", "#eeee77", PANEL_MAIN, "Leg8" );
	else if (s=="6") textPut("colour.background", "#eeee77", PANEL_MAIN, "Leg9" );

	return s;
}



void audio_list::loadNames( bool maintainSelectedHighlight )
{
	debug("audio_list::loadNames Called");
	char szRtrIndx[6], szUnused[64];
	bncs_string selectedIndex;
	bncs_string strname, strarea, strleg;
	
	if( !maintainSelectedHighlight )	{
		m_iIndexNo = -1;	
	}
	for (int i=1;i<10;i++) textPut("colour.background", "#888888", PANEL_MAIN, bncs_string("Leg%1").arg(i));
	
	textPut( "sort", "false", PANEL_MAIN, "list" );//added by Ruth to speed things up and save resources
	textPut( "clear", PANEL_MAIN, "list" );
	textPut( "text", "", PANEL_MAIN, "indexNo" );
	
	//Left as 0 to as this database is set to the correct size
	//int numIndexs = getRouterSize( m_intDevice, 0 );

	if(m_sSilence_Index.length() > 0)
	{
		if (m_blnAlphaSort==true) {
			textPut( "add", bncs_string(" %1").arg(m_sSilence_Text), PANEL_MAIN, "list" );
		}
		else { // numeric sort
			textPut( "add", bncs_string("%1 :%2").arg(m_sSilence_Index).arg(m_sSilence_Text), PANEL_MAIN, "list" );
		}
	}

	debug("audio_list::loadNames m_intIndxStart(%1) m_intIndxEndm_intDBIndex(%2) ",m_intIndxStart,m_intIndxEnd);
	//if (m_bAreaSelected==true) numIndexs = MAX_SRC_DEST; // any of the lower order databases
	for( bncs_stringlist::iterator it = m_slMinAndMaxRouterIndexs.begin() ; it != m_slMinAndMaxRouterIndexs.end() ; ++it )
	{
		bncs_string smin, smax;
		it->split(':',smin,smax);
		int min = smin.toInt();
		int max = smax.toInt();
		debug(bncs_string("audio_list::loadNames Area min:%1 max:%2").arg(min).arg(max));
		for( int x = min ; x <= max ; x++ ) 	{
			//debug("audio_list::loadNames m_intDevice(%1), m_intDBAreas(%2) iRealNum(%3)",m_intDevice, m_intDBAreas,iRealNum);		
			strarea = dbm.getName(m_intDevice, m_intDBAreas, x );
			if(m_bAllNameSpacesListed || m_slListedNameSpaces.find(strarea) != -1)
			{
				strleg = dbm.getName(m_intDevice, m_intDBAudioLeg,x);
				//routerName(m_intDevice, m_intDBAudioLeg,iRealNum, s );
				if(m_slSelectableLegs.find(strleg) != -1)
				{
					strname = dbm.getName(m_intDevice, m_intDBIndexNames, x );
					sprintf_s(szRtrIndx, sizeof(szRtrIndx), "%05d", x);
					//debug("strname(%1)",strname);
					if ((strname.length()==0)||(strname.left(4)=="N_U_")||(strname.find("---")==0)||(strname.find("!!!")==0)) {
						if (m_blnShowUnusedPorts==TRUE) {
								if ((strname.length()==0)||(strname.find("---")==0)||(strname.find("!!!")==0))
									wsprintf(szUnused, "N_U_%05d", x );
								else
									wsprintf(szUnused, "%s", LPCSTR(strname));
								if (m_blnAlphaSort==true) {
									textPut( "add", bncs_string("%1:%2").arg(strarea).arg(szUnused), PANEL_MAIN, "list" );
								}
								else { // numeric sort
									textPut( "add", bncs_string("%1 %2:%3").arg(szRtrIndx).arg(strarea).arg(szUnused), PANEL_MAIN, "list" );
								}
							}
					}
					else {
						if (m_blnAlphaSort==true) {
							textPut( "add", strname, PANEL_MAIN, "list" );
						}
						else { // numeric sort
							textPut( "add", bncs_string("%1 %2").arg(szRtrIndx).arg(strname), PANEL_MAIN, "list" );
						}
					}
				}
			}
		}
	}
	textPut( "sort", "true", PANEL_MAIN, "list" );
	
	if( maintainSelectedHighlight )	{
		//routerName( HYDRA_ROUTER, SOURCE_DB_NAME, m_iSourceNo, strname );
		strname = dbm.getName(m_intDevice, m_intDBIndexNames, m_iIndexNo );
		//routerName(m_intDevice, m_intDBIndexNames, m_iIndexNo ,strname);
		setListboxSelectedItem(strname);
	}
	else
	{
		textPut( "selected=none", 1, "list" );
		hostNotify(bncs_string("audioType=%1").arg("L"));
		hostNotify( "index=0");
	}
}




bncs_string audio_list::getAreaString( int iRtrIndex )
{
	/*
	if (iRtrIndex>0) {
		int iarea = (iRtrIndex-1)/4096;
		switch ( iarea ) {
		case 1: return "G1"; break;
		case 2: return "G2"; break;
		case 3: return "G3"; break;
		case 4: return "G4"; break;
		case 5: return "G5"; break;
		case 6: return "G6"; break;
		case 7: return "G7"; break;
		case 8: return "G8"; break;
		case 9: {
			if (iRtrIndex<37121) return "F0"; 
			else if (iRtrIndex<37377) return "F1"; 
			else if (iRtrIndex<37633) return "F2"; 
			else if (iRtrIndex<37889) return "F3"; 
			else if (iRtrIndex<38145) return "F4"; 
			else if (iRtrIndex<38401) return "F5"; 
			else if (iRtrIndex<38627) return "F6"; 
			else if (iRtrIndex<38883) return "N"; 
			else return "F??";
				}
			break;  
		case 10: return "T"; break;
		case 11: return "C"; break;
		case 12: return "E"; break;
		case 13: return "L"; break;
		case 14: return "xx"; break;
		case 15: return "xx"; break;
		default:
			return "S";
		}
	}*/
	return "";
}

int audio_list::getRouterFromArea(bncs_string AreaName)
{
	/*
	if(AreaName.length())
	{
		bncs_string first = AreaName.left(1).upper();
		bncs_string second = AreaName.right(1).upper();
		//debug("audio_list::getRouterFromArea %1 %2",first,second);
		if(first == "S")
			return 0;
		else if(first == "G")
			return second.toInt();
		else if(first == "F" || first == "N")
			return 9;
		else if(first == "T")
			return 10;
		else if(first == "C")
			return 11;
		else if(first == "E")
			return 12;
		else if(first == "L")
			return 13;
		//Currently 14 and 15 are not used, if they are to be this will need to be changed
		else if(first == "X")
			return 14;
		else if(first == "Y")
			return 15;
		else
			return 17;
	}
return 17;
	*/
return 17;
}

void audio_list::setListboxSelectedItem(bncs_string strName)
{
	debug("audio_list::setListboxSelectedItem name=%1 \n", strName );
	textPut("selected.text.contains", strName, PANEL_MAIN, "list");
}


bncs_stringlist audio_list::getStartAndDestForArea(int iarea)
{
	
	bncs_stringlist sl;
	/*
		if(iarea < 9)
		{
			sl << bncs_string("%1:%2").arg((iarea * MAX_SRC_DEST)+1).arg((iarea * MAX_SRC_DEST) + MAX_SRC_DEST);
			m_bAreaSelected = true;
		}
		else if( iarea == 9)
		{
			for(int i = 1;i <= 8; i++)
			{
				sl << bncs_string("%1:%2").arg(((i * MAX_SRC_DEST)+1) + FLOOR_START).arg((i * MAX_SRC_DEST) + FLOOR_START + FLOOR_SIZE);
				m_bAreaSelected = true;
			}
		}
		else if (iarea < 16)
		{
			sl << bncs_string("%1:%2").arg((iarea * MAX_SRC_DEST)+1).arg((iarea * MAX_SRC_DEST) + MAX_SRC_DEST);
			m_bAreaSelected = true;
		}
		else
		{*/
			sl << bncs_string("%1:%2").arg(1).arg(65536);
			m_bAreaSelected = false;
		/*}
	//debug("audio_list::getStartAndDestForArea() sl=%1 ", sl.toString());
	return sl;	
	*/
	return sl;
}

void audio_list::selectArea(int intRouter)
{
/*
	debug("intRouter(%1)",intRouter);
	for (int i=1;i<10;i++) textPut("colour.background", "#888888", PANEL_MAIN, bncs_string("Leg%1").arg(i));
	if ((intRouter >=1)&&(intRouter<17)) {
		//intRouter = m_intDevice + intRouter -1;
		m_intIndxStart = (MAX_SRC_DEST*(intRouter-1))+1;
		debug("m_intIndxStart(%1)",m_intIndxStart);
		m_intIndxEnd = m_intIndxStart + MAX_SRC_DEST - 1;
		debug("m_intIndxEnd(%1)",m_intIndxEnd);
		controlEnable(PANEL_MAIN, "sort");
		m_bAreaSelected = true;
	}
	else */if (intRouter==17) {
		m_intIndxStart = 1;
		m_intIndxEnd = 65536;
		//m_blnAlphaSort = false;
		//textPut("text", "ALPHA|SORT", PANEL_MAIN, "sort");
		//controlDisable(PANEL_MAIN, "sort");
		m_bAreaSelected = false;
	}
	else
		return;
	
	m_slMinAndMaxRouterIndexs = getStartAndDestForArea(intRouter);
	highlightButton(intRouter+1); 
	loadNames(false);

	//Set initial Source to Park
	parkSelected();
	
}

void audio_list::highlightButton(int intRouter)
{
	if(m_iSelectedNamespaceButton > 0)
	{
		textPut("statesheet", "enum_deselected", PANEL_MAIN, bncs_string("Jump_%1").arg(m_iSelectedNamespaceButton));
		textPut("text", "", PANEL_MAIN, "area");
	}
	bncs_string name;
	textPut("statesheet", "enum_selected", PANEL_MAIN, bncs_string("Jump_%1").arg(intRouter));
	textGet( "text", PANEL_MAIN, bncs_string("Jump_%1").arg(intRouter), name );
	name.replace("|", " " );
	textPut("text", name, PANEL_MAIN, "area");
	m_iSelectedNamespaceButton = intRouter;
}


void audio_list::parkSelected()
{
	textPut( "text", "NONE", PANEL_MAIN, "indexNo" );
	m_iIndexNo = CLEAR_INDEX;				
	for (int i=1;i<10;i++) textPut("colour.background", "#888888", PANEL_MAIN, bncs_string("Leg%1").arg(i));
	textPut( "selected=none", 1, "list" );
	hostNotify(bncs_string("audioType=%1").arg("L"));
	hostNotify( "index=0");
	textPut("statesheet", "enum_selected",PANEL_MAIN,"park");
}
