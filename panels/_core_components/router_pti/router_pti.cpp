#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "router_pti.h"

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( router_pti )

#define PANEL_MAIN		1
#define POPUP_PTI_LIST	2


// constructor - equivalent to ApplCore STARTUP
router_pti::router_pti( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	//init Vars
	m_intDevice = 0;
	m_intPackageRtrDevice = 0;
	m_intCurrentSource = 0;

	m_intPackagePtiDevice = 0;

	m_bPackager = false;

	// End pacakger calculation 	

	
	// show a panel from file p1.bncs_ui and we'll know it as our panel 1
	panelShow( PANEL_MAIN, "main.bncs_ui" );

	controlDisable(PANEL_MAIN, "btnPTI");

	// you may need this call to set the size of this component 
	//  if it's used in a popup window 
//	setSize( 1024,668 );		// set the size explicitly
	setSize( 1 );				// set the size to the same as the specified panel

	

}

// destructor - equivalent to ApplCore CLOSEDOWN
router_pti::~router_pti()
{
}

// all button pushes and notifications come here
void router_pti::buttonCallback( buttonNotify *b )
{
	if( b->panel() == PANEL_MAIN )
	{
		if( b->id() == "btnPTI")
		{
			showPTI();
		}
	}
	else if( b->panel() == POPUP_PTI_LIST )
	{
		if( b->id() == "close")
		{
			panelRemove(POPUP_PTI_LIST);
		}
		else if(b->id() == "package_listview")
		{
			b->dump("Package_listview");
			if("selection" == b->command())
				showPackageRouting(b->sub(0));
	
		}
	}
}

// all revertives come here
int router_pti::revertiveCallback( revertiveNotify * r )
{
	if(m_bSettingsLoaded)
	{
		if(r->device() == m_intDevice)
		{
			DestIndex* d = m_vDestIndex[r->index()];
			
			//Get old SourceIndex
			int oldSourceIndex = d->SourceIndexRouted;

			//Set new SourceIndex
			d->SourceIndexRouted = r->info();

			//If this Dest has a previous Source routed to it remove the Dest from that source
			if(oldSourceIndex > 0)
			{
				m_vSourceIndex[oldSourceIndex]->RemoveDest(d);
			}

			//Add to new Source
			m_vSourceIndex[d->SourceIndexRouted]->AddDest(d);
			updatePTI(d, oldSourceIndex);
		}
		else if(r->device() == m_intPackagePtiDevice)
		{
			SourcePackage* s = m_vSourcePackages[r->index()];

			s->AssociatedDestPackage(r->sInfo());
			if(m_spPackageSource_Selected == s)
			{
				updatePTIPackage();
				showPackageRouting();
			}
		}
	}
	return 0;
}

// all database name changes come back here
void router_pti::databaseCallback( revertiveNotify * r )
{
	//r->dump("router_pti::databaseCallback");
	if(m_bSettingsLoaded)
	{
		if(r->device() == m_intDevice)
		{
			if( r->database() == 1)
			{
				//Change the name if the DestName changes
				m_vDestIndex[r->index()]->Name = r->sInfo();
			}
			else if(r->database() == 0)
			{
				m_vSourceIndex[r->index()]->Name = r->sInfo();
			}
		}
		else if(r->device() == m_intPackageRtrDevice)
		{
			if(r->database() == 1)
			{
				m_vDestPackages[r->index()]->Name = r->sInfo();
			}
			else if(r->database() == 0)
			{
				m_vSourcePackages[r->index()]->Name = r->sInfo();
			}
			else if(r->database() == 6)
			{
				SourcePackage* s = m_vSourcePackages[r->index()];

				int oldSourceIndex = s->SDIIndex;
				s->SDIIndex = getSDIIndex(r->sInfo());


				//Check if the source Package was in a PTI
				if(oldSourceIndex != 0 && s->SDIIndex < (int)m_vSourceIndex.size())
					m_vSourceIndex[oldSourceIndex]->RemoveSourcePackage(s);

				if(s->SDIIndex > 0 && s->SDIIndex < (int) m_vSourceIndex.size())
					m_vSourceIndex[s->SDIIndex]->AddSourcePackage(s);

				if(oldSourceIndex != s->SDIIndex)
				{
					if(m_intCurrentSource == oldSourceIndex || m_intCurrentSource == s->SDIIndex)
					{
						//Update the package here display here
						updatePTIPackage();
					}
				}
			}
		}
	}
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string router_pti::parentCallback( parentNotify *p )
{
	if(p->command() == "instance")
	{
		if(getDev(p->value(), &m_intDevice))
			timerStart(1,1);
	}
	else if(p->command() == "instance_packager_rtr")
	{
		m_instance_PackageRtrDevice = p->value();
		if(getDev(m_instance_PackageRtrDevice, &m_intPackageRtrDevice))
			timerStart(1,1);
	}
	else if(p->command() == "instance_packager_pti")
	{
		m_instance_PackagePtiDevice = p->value();
		if(getDev(m_instance_PackagePtiDevice, &m_intPackagePtiDevice))
			timerStart(1,1);
	}
	else if(p->command() == "source")
	{
		m_intCurrentSource = p->value().toInt();

		if(m_intCurrentSource > 0)
		{
			controlEnable(PANEL_MAIN, "btnPTI");
			pollRelevantIndexes();
				
		}
		else
		{
			controlDisable(PANEL_MAIN, "btnPTI");
		}
	}
	else if(p->command() == "return")
	{
		bncs_stringlist sl;
		sl << bncs_string("instance_packager_rtr=%1").arg(m_instance_PackageRtrDevice);
		sl << bncs_string("instance_packager_pti=%1").arg(m_instance_PackagePtiDevice);
		return sl.toString('\n');
	}
	return "";
}

// timer events come here
void router_pti::timerCallback( int id )
{
	if(id = 1)
	{
		timerStop(1);
		init();
	}
}

void router_pti::showPTI()
{
	if(m_bSettingsLoaded)
	{
		bncs_string strName;
		m_spPackageSource_Selected = NULL;
		//Initialise popup
		if (m_bPackager)

		{
			panelPopup(POPUP_PTI_LIST, "popup_pti_listview_mcr.bncs_ui");

		}
		else
		{
			panelPopup(POPUP_PTI_LIST, "popup_pti_listview.bncs_ui");

		}
		
		SourceIndex* Source = m_vSourceIndex[m_intCurrentSource];
		vector<DestIndex*>Dests = Source->GetDestIndexes();
		textPut("clear", "clear", POPUP_PTI_LIST, "destination_listview");
		textPut("text", bncs_string("Destination List for Source %1 - %2").arg(m_intCurrentSource).arg(Source->Name).replace('|', ' '), 
			POPUP_PTI_LIST, "grpPTI");
		if(m_intCurrentSource > 0)
		{
			vector<DestIndex*>::iterator it;
			for(it = Dests.begin(); it != Dests.end(); ++it)
			{
				textPut("add", bncs_string("%1;%2").arg((*it)->Index,' ',4).arg((*it)->Name.replace('|', ' ')), 
						POPUP_PTI_LIST, "destination_listview");
			}
		}

		updatePTIPackage();
	}

}

void router_pti::updatePTI(DestIndex* dest, int oldIndex)
{
	if(m_bSettingsLoaded)
	{	
		if(m_intCurrentSource > 0)
		{
			bncs_string strReturn;
			textGet("count", POPUP_PTI_LIST, "destination_listview", strReturn);
			int intRowCount = strReturn.toInt();

			//Checking this first, I assume if the popup is not loaded this will return 0 so we don't bother doing anything else
			if(intRowCount > 0)
			{
				//Updating the PTI, check if the dest was in the currently displayed popup
				if(m_intCurrentSource == oldIndex && dest->SourceIndexRouted != m_intCurrentSource)
				{
					//find the row that displays this dest
					for(int intRow = 0; intRow < intRowCount; intRow++)
					{
						//The dest needs to be removed from the current being displayed

						//remove the dest from the listview
						textGet(bncs_string("row.%1").arg(intRow), POPUP_PTI_LIST, "destination_listview", strReturn); 

						bncs_string strIndex, strName;
						strReturn.split(';', strIndex, strName);

						if(strIndex.toInt() == dest->Index)
						{
							textPut("removeRow", intRow, POPUP_PTI_LIST, "destination_listview");
							break;
						}
					}
				}
				//Check if our dest needs to be added to the List, and that it was not already added
				if(dest->SourceIndexRouted == m_intCurrentSource && oldIndex != m_intCurrentSource)
				{
					//Add dest to listview
					bncs_string strDestName;
					textPut("add", bncs_string("%1;%2").arg(dest->SourceIndexRouted,' ',4).arg(dest->Name.replace('|', ' ')), 
						POPUP_PTI_LIST, "destination_listview");
					
				}
			}
		}
	}
}




void router_pti::init()
{
	m_bSettingsLoaded = false;
	debug("router_pti::init() Started Loading");
	if(m_intPackagePtiDevice > 0 && m_intPackageRtrDevice > 0)
	{
		m_bPackager = true;
	}
	else
	{
		m_bPackager = false;
	}

	if(m_intDevice >0)
	{
		int intDestRouterSize = getRouterSize(m_intDevice, 1);
		int intSourceRouterSize = getRouterSize(m_intDevice, 0);

		//Tidy up the List, Empty, than add the same number of items as the size of the router, plus as inital item of 0, so the array relates to the index 1 to 1 (Index 1 can be found at [1])
		
		//Clear vector
		vector<DestIndex*>::iterator dest_it;
		for(dest_it = m_vDestIndex.begin(); dest_it != m_vDestIndex.end(); ++dest_it)
		{
			delete *dest_it;
		}
		m_vDestIndex.clear();
		
		//All of the source indexes
		//Clear map
		vector<SourceIndex*>::iterator source_it;
		for(source_it = m_vSourceIndex.begin(); source_it != m_vSourceIndex.end(); ++source_it)
		{
			delete *source_it;
		}
		m_vSourceIndex.clear();


		//Create new dest indexes
		for(int iDest = 0; iDest <= intDestRouterSize; ++iDest)
		{
			DestIndex* d = new DestIndex();
			d->Index = iDest;
			
			if(iDest != 0)
			{
				//Get Router name
				bncs_string sName;
				routerName(m_intDevice,1,iDest,sName);
				d->Name = sName;
				//debug("Loading router_pti Dest:%1 Name:%2",d->Index,d->Name );
			}
				
			m_vDestIndex.push_back(d);
		}

		
		for(int iSource = 0; iSource <= intSourceRouterSize; ++iSource)
		{
			SourceIndex* s = new SourceIndex();
			s->Index = iSource;

			if(iSource != 0)
			{
				//Get Router name
				bncs_string sName;
				routerName(m_intDevice,0,iSource,sName);
				s->Name = sName;
				//debug("Loading router_pti Sources:%1 Name:%2",s->Index,s->Name );
			}

			m_vSourceIndex.push_back(s);
		}
		debug("Loading Sources Finished");

	
		if(m_bPackager)
		{
			
			int intDestPackageRouterSize = getRouterSize(m_intPackageRtrDevice, 1);
			int intSourcePackageRouterSize = getRouterSize(m_intPackageRtrDevice, 0);

			//SOURCES

			//Clear vector
			vector<SourcePackage*>::iterator source_pack_it;
			for(source_pack_it = m_vSourcePackages.begin(); source_pack_it != m_vSourcePackages.end(); ++source_pack_it)
			{
				delete *source_pack_it;
			}
			m_vSourcePackages.clear();

			//Load Vector
			for(int iSourcePackage = 0; iSourcePackage <= intSourcePackageRouterSize; ++iSourcePackage)
			{
				//debug("Loading router_pti SourcesPackages:%1",iSourcePackage );
				SourcePackage* s = new SourcePackage();

				s->Index = iSourcePackage;

				if(iSourcePackage >0)
				{
					bncs_string sName;
					routerName(m_intPackageRtrDevice, 0,iSourcePackage,sName);
					s->Name = sName;

					bncs_string sSDI;
					routerName(m_intPackageRtrDevice, 6,iSourcePackage,sSDI);
					s->SDIIndex = getSDIIndex(sSDI);

					bncs_string sTitle;
					routerName(m_intPackageRtrDevice, 2,iSourcePackage,sTitle);
					s->Title = sTitle;

				}
					m_vSourcePackages.push_back(s);
					
				//Add our sourcePackage to the correct SDI Input as long as its not 0
				if(s->SDIIndex != 0)
					m_vSourceIndex[s->SDIIndex]->AddSourcePackage(s);
				

			}

			//DESTS
			vector<DestPackage*>::iterator dest_pack_it;
			for(dest_pack_it = m_vDestPackages.begin(); dest_pack_it != m_vDestPackages.end(); ++dest_pack_it)
			{
				delete *dest_pack_it;
			}
			m_vDestPackages.clear();


			//Load Vector
			for(int iDestPackage = 0; iDestPackage <= intSourcePackageRouterSize; ++iDestPackage)
			{
				DestPackage* d = new DestPackage();
				d->Index = iDestPackage;
				
				if(iDestPackage > 0)
				{
					bncs_string sName;
					routerName(m_intPackageRtrDevice, 1,iDestPackage,sName);
					d->Name = sName;
				}
				m_vDestPackages.push_back(d);
			}


			debug("Registering with infoDrivers: RtrDevice:%1",intDestPackageRouterSize);
			//Register for the last index, just to get database callbacks
			infoRegister(m_intPackageRtrDevice, 4096, 4096);

			debug("Registering with infoDrivers: PtiDevice:%1 Size:%2",m_intPackagePtiDevice,intDestPackageRouterSize);
			infoRegister(m_intPackagePtiDevice, 1, intDestPackageRouterSize, true);
			//infoPoll(m_intPackagePtiDevice, 1, intDestPackageRouterSize);

		}
	

		if(m_bPackager)
			infoRegister(m_intDevice, 1, intDestRouterSize, true);
		else 
			infoRegister(m_intDevice, 1, intDestRouterSize, false);
		routerPoll(m_intDevice, 1, intDestRouterSize);
		m_bSettingsLoaded = true;
	}
	debug("router_pti::init() Ended Loading");
}

int router_pti::getSDIIndex(bncs_string s)
{
	bncs_string sSDIIndex = bncs_stringlist(s,',').getNamedParam("hd");
	return sSDIIndex.toInt();
}

void router_pti::showPackageRouting(int iSourcePackage)
{

	textPut("clear",POPUP_PTI_LIST,"package_directly_listview");
	textPut("clear",POPUP_PTI_LIST,"package_finally_listview");

	if(m_bSettingsLoaded)
	{	
		bool bShowPackage = false;

		if(iSourcePackage == -1)
		{
			if(m_spPackageSource_Selected != NULL)
				bShowPackage = true;
		}
		else if(m_intCurrentSource != -1)
		{
			m_spPackageSource_Selected = m_vSourceIndex[m_intCurrentSource]->GetSourcePackages()[iSourcePackage];
			bShowPackage = true;
		}

		if(bShowPackage)
		{	
			debug("Showing");
			//package_directly_listview
			bncs_stringlist slDirectly = m_spPackageSource_Selected->slDirectly;
			
			bncs_stringlist::iterator itDir;
			for(itDir = slDirectly.begin(); itDir != slDirectly.end(); ++itDir)
			{
				debug("Directly:%1",(*itDir));
				DestPackage* d = m_vDestPackages[*itDir];
				textPut("add", bncs_string("%1;%2").arg(d->Index,' ',4).arg(d->Name.replace('|', ' ')),POPUP_PTI_LIST,"package_directly_listview");
			}

			//package_finally_listview
			bncs_stringlist slFinally = m_spPackageSource_Selected->slVirtual;

			bncs_stringlist::iterator itFin;
			for(itFin = slFinally.begin(); itFin != slFinally.end(); ++itFin)
			{
				debug("Finally:%1",(*itFin));
				DestPackage* d = m_vDestPackages[*itFin];
				textPut("add",bncs_string("%1;%2").arg(d->Index,' ',4).arg(d->Name.replace('|', ' ')),POPUP_PTI_LIST,"package_finally_listview");
			}
			debug("Showing Dir:%1 Fin:%2",slDirectly.toString(','), slFinally.toString(','));
		}
	}
}

void router_pti::updatePTIPackage()
{
	if(m_bPackager)
	{
		textPut("clear",POPUP_PTI_LIST,"package_listview");
		SourceIndex* Source = m_vSourceIndex[m_intCurrentSource];

		vector<SourcePackage*>SourcePackages = Source->GetSourcePackages();
		
		vector<SourcePackage*>::iterator itSourcePackage;
		for(itSourcePackage = SourcePackages.begin(); itSourcePackage != SourcePackages.end(); ++itSourcePackage)
		{
			textPut("add", bncs_string("%1;%2;%3;%4").arg((*itSourcePackage)->Index,' ',4).arg((*itSourcePackage)->Name.replace('|', ' ')).arg((*itSourcePackage)->Title).arg((*itSourcePackage)->Usage), 
				POPUP_PTI_LIST, "package_listview");
		}
	
	}
}

void router_pti::pollRelevantIndexes()
{
	if(m_bSettingsLoaded)
	{
		if(m_bPackager)
		{
			SourceIndex* Source = m_vSourceIndex[m_intCurrentSource];

			vector<SourcePackage*>SourcePackages = Source->GetSourcePackages();
			
			vector<SourcePackage*>::iterator itSourcePackage;
			for(itSourcePackage = SourcePackages.begin(); itSourcePackage != SourcePackages.end(); ++itSourcePackage)
			{
				if((*itSourcePackage)->bAssociatedPackagePolled == false)
				{
					infoPoll(m_intPackagePtiDevice, (*itSourcePackage)->Index, (*itSourcePackage)->Index);
				}
			}
			
		}
	}
}
