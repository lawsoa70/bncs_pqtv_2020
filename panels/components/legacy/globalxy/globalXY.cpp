#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "globalXY.h"

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( globalXY )
#define VIDEO_ROUTER 101
#define PANEL_MAIN	1
#define POPUP_DUMP	2
#define KEYBOARD	3
#define PNL_RENAME	4

#define MAX_ROUTERS 3

#define	DB_SOURCE_BUTTON	0
#define	DB_SOURCE_LONG		2
#define	DB_SOURCE_HARDWARE	4

#define	DB_DEST_BUTTON		1
#define	DB_DEST_LONG		3
#define	DB_DEST_HARDWARE	5

#define MIN_LENGTH_NONE		0
#define MIN_LENGTH_ONE		1

#define MAX_LENGTH_BUTTON	16
#define MAX_LENGTH_SOFT		32
#define MAX_LENGTH_HARDWARE	10
#define MAX_LENGTH_DETAILS  255

#define SORT_MODE_NUMERIC	1
#define SORT_MODE_ALPHA		2

#define RENAME_HARDWARE 0
#define RENAME_DETAILS  1
#define RENAME_NEITHER  2


// constructor - equivalent to ApplCore STARTUP
globalXY::globalXY( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	// show a panel from file p1.bncs_ui and we'll know it as our panel 1
	m_iDeviceNo=0;
	m_iDeviceLockNo = 0;
	m_iDestNo=0;
	m_iSourceNo=0;
	m_intCurrentDestCurrentSource = 0;
	m_intLastRouter = -1;
	m_intNumpadIndex = 0;
	m_blnShowUnusedPorts = false;
	m_intRenameDatabase = -1;
	m_blnAlphaSortSource = false;
	m_blnAlphaSortDest = false;
	
	panelShow( PANEL_MAIN, "main.bncs_ui" );

	/*
		m_sltRouterInstances = bncs_stringlist("stc_sdi");

		int intRouterCount = m_sltRouterInstances.count();

		switch(intRouterCount)
		{
			//disable aux bus and prompt router buttons
			case 1:
				controlDisable( PANEL_MAIN, "router_2" );
				controlDisable( PANEL_MAIN, "router_3" );
				break;
			//disable prompt router button
			case 2:
				controlDisable( PANEL_MAIN, "router_2" );
				break;
		}

		//initially disable database rename buttons
		controlDisable( 1, "renameDest" );
		controlDisable( 1, "renameSource" );
	*/

	//initially hide the undo button
	controlHide(PANEL_MAIN, "btnUndo");

	fIgnoreSelectionChange = false;

	//LastSce = new LastSceRouteHandler();

	//jump to first router.
	//highlightButton("router_1"); 
	//load(m_sltRouterInstances[0]);

}

// destructor - equivalent to ApplCore CLOSEDOWN
globalXY::~globalXY()
{
	//delete LastSce;	
}

// all button pushes and notifications come here
void globalXY::buttonCallback( buttonNotify *b )
{
	int iTemp=0;
	bncs_string strReturn;

	if( b->panel() == PANEL_MAIN )
	{

		bncs_stringlist sltButtonID = bncs_stringlist(b->id(), '_');

		if(sltButtonID[0] == "router")
		{
			int intRouterButton = sltButtonID[1].toInt();
			selectRouter(intRouterButton);
		}
		else if(sltButtonID[0] == "numpad")
		{
			if(sltButtonID[1] == "source")
			{
				if(m_intNumpadIndex > 0)
				{
					if(m_intNumpadIndex != m_iSourceNo)
					{
						//Clear any current selection
						m_iSourceNo = 0;
						textPut( "text", "", PANEL_MAIN, "sourceNo" );
						textPut( "text", "", PANEL_MAIN, "takeSource" );
						
						setListboxSelectedItem("sources", m_intNumpadIndex);
					}
					m_intNumpadIndex = 0;
					textPut( "text", "", PANEL_MAIN, "lblNumpad" );
				}
			}
			else if(sltButtonID[1] == "dest")
			{
				if(m_intNumpadIndex > 0)
				{
					if(m_intNumpadIndex != m_iDestNo)
					{
						//Clear any current selection
						m_iDestNo = 0;
						textPut( "text", "", PANEL_MAIN, "destNo" );
						textPut( "text", "", PANEL_MAIN, "takeDest" );

						setListboxSelectedItem("dests", m_intNumpadIndex);
					}
					m_intNumpadIndex = 0;
					textPut( "text", "", PANEL_MAIN, "lblNumpad" );
				}
			}
			else if(sltButtonID[1] == "clear")
			{
				m_intNumpadIndex = 0;
				textPut( "text", "", PANEL_MAIN, "lblNumpad" );
			}
			else
			{
				int intNumpadButton = sltButtonID[1].toInt();
				
				bncs_string strNumpadIndex = bncs_string(m_intNumpadIndex);
				strNumpadIndex += bncs_string(intNumpadButton);

				
				m_intNumpadIndex = strNumpadIndex.toInt();

				if(m_intNumpadIndex > 4096)
				{
					m_intNumpadIndex = 0;
				}
				textPut( "text", m_intNumpadIndex, PANEL_MAIN, "lblNumpad" );
			}


		}
		else if(sltButtonID[0] == "sourceJump")
		{
			if(m_blnAlphaSortSource)
			{
				bncs_string strJump;
				textGet("text", PANEL_MAIN, b->id(), strJump);
	
				for(int intChar = 0; intChar < strJump.length(); intChar++)
				{
					textPut( "selected.text.begins", strJump.mid(intChar,1), PANEL_MAIN, "sources");
					//check if an item was selected
					bncs_string strSelectedIndex;
					textGet( "selectedindex", PANEL_MAIN, "sources", strSelectedIndex);
					if(strSelectedIndex.toInt() > -1)
					{
						break;
					}
				}
			}
		}
		else if(sltButtonID[0] == "destJump")
		{
			if(m_blnAlphaSortDest)
			{
				bncs_string strJump;
				textGet("text", PANEL_MAIN, b->id(), strJump);
	
				for(int intChar = 0; intChar < strJump.length(); intChar++)
				{
					textPut( "selected.text.begins", strJump.mid(intChar,1), PANEL_MAIN, "dests");
					//check if an item was selected
					bncs_string strSelectedIndex;
					textGet( "selectedindex", PANEL_MAIN, "dests", strSelectedIndex);
					if(strSelectedIndex.toInt() > -1)
					{
						break;
					}
				}
			}
		}
		else if( b->id() == "dests")
			{
				if( b->command() == "selection" ) 
				{
					if( fIgnoreSelectionChange )
						return;

					//Get the name of the hightlighted dest from the list
					bncs_string strDestListItem = b->value();

					if(!m_blnAlphaSortDest)
					{
						strDestListItem = strDestListItem.right(strDestListItem.length() - 5);
					}

					textPut( "text", strDestListItem, PANEL_MAIN, "takeDest" );

					//Get the index of this destination
					m_iDestNo = routerIndex(m_iDeviceNo, DB_DEST_BUTTON, strDestListItem);
					textPut( "text", m_iDestNo, PANEL_MAIN, "destNo" );

					//Moving to a new destination - drop the current source
					textPut( "text", "", PANEL_MAIN, "lastSource" );
					m_intCurrentDestCurrentSource = 0;
					
					debug("globalXY::buttonCallback() polling for device=%1 slot=%2", m_iDeviceNo, m_iDestNo);
					
					//Poll to get the source this new destination currently has routed
					routerRegister(m_iDeviceNo, m_iDestNo, m_iDestNo);
					routerPoll(m_iDeviceNo, m_iDestNo, m_iDestNo);

					//Clear the current lock indication and state
					textPut("statesheet", "dest_unlocked", PANEL_MAIN, "btnUnlock");
					m_blnCurrentDestLocked = false;

					//Register and poll lock slot
					if(m_iDeviceLockNo == 0){
						//Don't attempt to register or the router registration would be lost
					}
					else
					{
						infoRegister(m_iDeviceLockNo, m_iDestNo, m_iDestNo);
						infoPoll(m_iDeviceLockNo, m_iDestNo, m_iDestNo);
					}
					
					if( m_iDestNo )
					{
						controlEnable( PANEL_MAIN, "renameDest" );
						controlEnable( PANEL_MAIN, "btnUnlock" );
					}
					else
					{
						controlDisable( PANEL_MAIN, "renameDest" );
						controlDisable( PANEL_MAIN, "btnUnlock" );
					}

					bncs_string strDB3, strDB5;
					routerName(m_iDeviceNo, 3, m_iDestNo, strDB3);
					routerName(m_iDeviceNo, 5, m_iDestNo, strDB5);
					bncs_string strDetails = bncs_string("%1 - %2").arg(strDB5.replace('|',' ')).arg(strDB3.replace('|',' '));
					textPut("text", strDetails.replace("!!!"," "), PANEL_MAIN, "dest_details" );
				}
			}

		else if( b->id() == "sources")
		{
			if( b->command() == "selection" ) 
			{
				//when a new source is selected hide the UNDO button
				controlHide(PANEL_MAIN, "btnUndo");

				//Get the text of the item to find the source name
				bncs_string strSourceListItem = b->value();

				if(!m_blnAlphaSortSource)
				{
					m_iSourceNo = strSourceListItem.left(4).toInt();
					strSourceListItem = strSourceListItem.right(strSourceListItem.length() - 5);
				}
				else
				{
					m_iSourceNo = routerIndex(m_iDeviceNo, DB_SOURCE_BUTTON, strSourceListItem);
				}
				
				//Put the source name into the take box
				textPut( "text", strSourceListItem, PANEL_MAIN, "takeSource" );

				
				//Update the source index label and rename button
				showSourceIndex(m_iSourceNo);
			}
		}
		else if( b->id() == "btnTake")	
		{
			if(m_iSourceNo &&  m_iDestNo){
				//Save undo source
				m_intUndoSource = m_intCurrentDestCurrentSource;
				
				//Make new crosspoint
				routerCrosspoint(m_iDeviceNo, m_iSourceNo, m_iDestNo);

				if(m_iSourceNo != m_intUndoSource){
					//Enable the UNDO facility
					controlShow(PANEL_MAIN, "btnUndo");
				}
			}
		}
		else if( b->id() == "btnUndo")	
		{
			//Route the Undo source if valid
			if(m_intUndoSource)
			{
				routerCrosspoint(m_iDeviceNo, m_intUndoSource, m_iDestNo);
			}

			//now remove the ability to undo the undo
			controlHide(PANEL_MAIN, "btnUndo");
			m_intUndoSource = 0;
		}
		else if( b->id() == "renameSource")
		{
			m_doRenameDest = false;
			renamePanel( m_doRenameDest);
		}
		else if( b->id() == "renameDest")
		{
			m_doRenameDest = true;
			renamePanel( m_doRenameDest);
			
			/*
			panelPopup( KEYBOARD, "renameKeyboard.bncs_ui" );

			// show the current value on the keyboard
			bncs_string s;
			routerName( m_iDeviceNo, DB_DEST_BUTTON, m_iDestNo, s );
			textPut( "text", s, KEYBOARD, "keyboard" );
			textPut( "setFocus", KEYBOARD, "keyboard" );
			*/
		}
		else if( b->id() == "dump" ) 
		{
			showDump();
		}
		else if( b->id() == "btnToggleUnused")
		{
			if(m_blnShowUnusedPorts)
			{
				textPut("text", "Show|Unused Ports", PANEL_MAIN, "btnToggleUnused");
				m_blnShowUnusedPorts = false;
			}
			else
			{
				textPut("text", "Hide|Unused Ports", PANEL_MAIN, "btnToggleUnused");
				m_blnShowUnusedPorts = true;
			}

			controlDisable( PANEL_MAIN, "renameSource" );
			controlDisable( PANEL_MAIN, "renameDest" );
			controlDisable( PANEL_MAIN, "btnUnlock" );

			loadSourceNames();
			loadDestNames();
		}
		else if(b->id() == "btnUnlock")
		{
			if(m_blnCurrentDestLocked)
			{
				infoWrite(m_iDeviceLockNo, "0", m_iDestNo);
			}
			else
			{
				infoWrite(m_iDeviceLockNo, "1", m_iDestNo);
			}
		}
		else if(b->id() == "sortSource")
		{
			if(m_blnAlphaSortSource)
			{
				m_blnAlphaSortSource = false;
				textPut("text", "ALPHA|SORT", PANEL_MAIN, "sortSource");
				for(int intJump = 1; intJump <= 11; intJump++)
				{
					textPut("text", "", PANEL_MAIN, bncs_string("sourceJump_%1").arg(intJump));
				}
			}
			else
			{
				m_blnAlphaSortSource = true;
				textPut("text", "NUMERIC|SORT", PANEL_MAIN, "sortSource");
				bncs_stringlist sltJumpList = bncs_stringlist("").fromString("AB,CD,EFG,HIJ,KL,MN,OP,QR,ST,UV,WXYZ");
				for(int intJump = 1; intJump <= 11; intJump++)
				{
					textPut("text", sltJumpList[intJump - 1], PANEL_MAIN, bncs_string("sourceJump_%1").arg(intJump));
				}
			}

			loadSourceNames();
		}
		else if(b->id() == "sortDest")
		{
			if(m_blnAlphaSortDest)
			{
				m_blnAlphaSortDest = false;
				textPut("text", "ALPHA|SORT", PANEL_MAIN, "sortDest");
				for(int intJump = 1; intJump <= 11; intJump++)
				{
					textPut("text", "", PANEL_MAIN, bncs_string("destJump_%1").arg(intJump));
				}
			}
			else
			{
				m_blnAlphaSortDest = true;
				textPut("text", "NUMERIC|SORT", PANEL_MAIN, "sortDest");
				bncs_stringlist sltJumpList = bncs_stringlist("").fromString("AB,CD,EFG,HIJ,KL,MN,OP,QR,ST,UV,WXYZ");
				for(int intJump = 1; intJump <= 11; intJump++)
				{
					textPut("text", sltJumpList[intJump - 1], PANEL_MAIN, bncs_string("destJump_%1").arg(intJump));
				}
			}
			loadDestNames();
		}
	}
	else if( b->panel() == KEYBOARD )
	{
		if( b->id() == "close" )
			panelDestroy( b->panel() );

		else if( b->id() == "keyboard" )
		{
			bncs_string name;

			textGet( "text", b->panel(), b->id(), name );
			
			if( m_doRenameDest )
			{
				routerModify( m_iDeviceNo, DB_DEST_BUTTON, m_iDestNo, name, false );
				setListboxSelectedItem("dests", m_iDestNo);
			}
			else
			{
				routerModify( m_iDeviceNo, DB_SOURCE_BUTTON, m_iSourceNo, name, false );
				setListboxSelectedItem("sources", m_iSourceNo);
			}
			panelDestroy( b->panel() );
		}
	}
	else if( b->panel() == PNL_RENAME )
	{
		if( b->id() == "save" ){
			
			//TODO update port databases
			if( m_doRenameDest )
			{
				bncs_string strButtonNameDB, strHardwareNameDB, strSoftNameDB;
				routerName( m_iDeviceNo, DB_DEST_BUTTON, m_iDestNo, strButtonNameDB );
				routerName( m_iDeviceNo, DB_DEST_LONG, m_iDestNo, strSoftNameDB );
				routerName( m_iDeviceNo, DB_DEST_HARDWARE, m_iDestNo, strHardwareNameDB );

				bncs_string strButtonNameEdit, strHardwareNameEdit,strSoftNameEdit;
				textGet("text", PNL_RENAME, "preview_button_name", strButtonNameEdit);
				textGet("text", PNL_RENAME, "preview_soft_name", strSoftNameEdit);
				if(m_iRenameDetails)
					textGet("text", PNL_RENAME, "preview_details", strHardwareNameEdit);
				else
					textGet("text", PNL_RENAME, "hidden_hardware_name", strHardwareNameEdit);

				//Check if button name has been modified
				if(strButtonNameDB != strButtonNameEdit)
				{
					routerModify( m_iDeviceNo, DB_DEST_BUTTON, m_iDestNo, strButtonNameEdit, false );
				}

				//Check if soft name has been modified
				if(strSoftNameDB != strSoftNameEdit)
				{
					routerModify( m_iDeviceNo, DB_DEST_LONG, m_iDestNo, strSoftNameEdit, false );
				}
				
				//Check if soft name has been modified
				if(strHardwareNameDB != strHardwareNameEdit)
				{
					routerModify( m_iDeviceNo, DB_DEST_HARDWARE, m_iDestNo, strHardwareNameEdit, false );
				}

				setListboxSelectedItem("dests", m_iDestNo);
			}
			else
			{
				bncs_string strButtonNameDB, strSoftNameDB, strHardwareNameDB;
				routerName( m_iDeviceNo, DB_SOURCE_BUTTON, m_iSourceNo, strButtonNameDB );
				routerName( m_iDeviceNo, DB_SOURCE_LONG, m_iSourceNo, strSoftNameDB );
				routerName( m_iDeviceNo, DB_SOURCE_HARDWARE, m_iSourceNo, strHardwareNameDB );

				bncs_string strButtonNameEdit, strSoftNameEdit, strHardwareNameEdit;
				textGet("text", PNL_RENAME, "preview_button_name", strButtonNameEdit);
				textGet("text", PNL_RENAME, "preview_soft_name", strSoftNameEdit);
				if(m_iRenameDetails)
					textGet("text", PNL_RENAME, "preview_details", strHardwareNameEdit);
				else
					textGet("text", PNL_RENAME, "hidden_hardware_name", strHardwareNameEdit);


				//Check if button name has been modified
				if(strButtonNameDB != strButtonNameEdit)
				{
					routerModify( m_iDeviceNo, DB_SOURCE_BUTTON, m_iSourceNo, strButtonNameEdit, false );
				}

				//Check if soft name has been modified
				if(strSoftNameDB != strSoftNameEdit)
				{
					routerModify( m_iDeviceNo, DB_SOURCE_LONG, m_iSourceNo, strSoftNameEdit, false );
				}
				
				//Check if soft name has been modified
				if(strHardwareNameDB != strHardwareNameEdit)
				{
					routerModify( m_iDeviceNo, DB_SOURCE_HARDWARE, m_iSourceNo, strHardwareNameEdit, false );
				}

				setListboxSelectedItem("sources", m_iSourceNo);
			}
			panelDestroy( b->panel() );
			panelShow( PANEL_MAIN, "main.bncs_ui" );
		}
		else if( b->id() == "cancel" ){
			panelDestroy( b->panel() );
			panelShow( PANEL_MAIN, "main.bncs_ui" );
		}
		else if( b->id() == "btnButtonName" )
		{
			textPut("numpadValidation", "Length", PNL_RENAME, "keyboard");
			textPut("minLength", MIN_LENGTH_ONE, PNL_RENAME, "keyboard");
			textPut("maxLength", MAX_LENGTH_BUTTON, PNL_RENAME, "keyboard");
			textPut("statesheet", "edit_name_selected", PNL_RENAME, "btnButtonName");
			textPut("statesheet", "edit_name_deselected", PNL_RENAME, "btnSoftName");
			textPut("statesheet", "edit_name_deselected", PNL_RENAME, "btnHardwareName");
			textGet("text", PNL_RENAME, "preview_button_name", strReturn);
			textPut("text", strReturn, PNL_RENAME, "keyboard");
			textPut("setFocus", "setFocus", PNL_RENAME, "keyboard");
			if(m_doRenameDest)
				m_intRenameDatabase = DB_DEST_BUTTON;
			else
				m_intRenameDatabase = DB_SOURCE_BUTTON;
		}
		else if( b->id() == "btnSoftName" )
		{
			textPut("numpadValidation", "Length", PNL_RENAME, "keyboard");
			textPut("minLength", MIN_LENGTH_NONE, PNL_RENAME, "keyboard");
			textPut("maxLength", MAX_LENGTH_SOFT, PNL_RENAME, "keyboard");
			textPut("statesheet", "edit_name_deselected", PNL_RENAME, "btnButtonName");
			textPut("statesheet", "edit_name_selected", PNL_RENAME, "btnSoftName");
			textPut("statesheet", "edit_name_deselected", PNL_RENAME, "btnHardwareName");
			textGet("text", PNL_RENAME, "preview_soft_name", strReturn);
			textPut("text", strReturn, PNL_RENAME, "keyboard");
			textPut("setFocus", "setFocus", PNL_RENAME, "keyboard");
			m_intRenameDatabase = DB_SOURCE_LONG;
		}
		else if( b->id() == "btnHardwareName" )
		{
			if(m_iRenameDetails)
			{
				textPut("numpadValidation", "Length", PNL_RENAME, "keyboard");
				textPut("minLength", MIN_LENGTH_NONE, PNL_RENAME, "keyboard");
				textPut("maxLength", MAX_LENGTH_DETAILS, PNL_RENAME, "keyboard");
				textPut("statesheet", "edit_name_deselected", PNL_RENAME, "btnButtonName");
				textPut("statesheet", "edit_name_deselected", PNL_RENAME, "btnSoftName");
				textPut("statesheet", "edit_name_selected", PNL_RENAME, "btnHardwareName");
				textGet("text", PNL_RENAME, "preview_details", strReturn);
				textPut("text", strReturn, PNL_RENAME, "keyboard");
				textPut("setFocus", "setFocus", PNL_RENAME, "keyboard");
				if(m_doRenameDest)
					m_intRenameDatabase = DB_DEST_HARDWARE;
				else
					m_intRenameDatabase = DB_SOURCE_HARDWARE;
			}
			else
			{
				textPut("numpadValidation", "Length", PNL_RENAME, "keyboard");
				textPut("minLength", MIN_LENGTH_ONE, PNL_RENAME, "keyboard");
				textPut("maxLength", MAX_LENGTH_HARDWARE, PNL_RENAME, "keyboard");
				textPut("statesheet", "edit_name_deselected", PNL_RENAME, "btnButtonName");
				textPut("statesheet", "edit_name_deselected", PNL_RENAME, "btnSoftName");
				textPut("statesheet", "edit_name_selected", PNL_RENAME, "btnHardwareName");
				textGet("text", PNL_RENAME, "hidden_hardware_name", strReturn);
				textPut("text", strReturn, PNL_RENAME, "keyboard");
				textPut("setFocus", "setFocus", PNL_RENAME, "keyboard");
				if(m_doRenameDest)
					m_intRenameDatabase = DB_DEST_HARDWARE;
				else
					m_intRenameDatabase = DB_SOURCE_HARDWARE;
			}
		}
		else if( b->id() == "keyboard" )
		{
			bncs_string strNewName;

			textGet( "text", b->panel(), b->id(), strNewName );

			if(m_intRenameDatabase == DB_DEST_BUTTON || m_intRenameDatabase == DB_SOURCE_BUTTON)
			{
				textPut( "text", strNewName, PNL_RENAME, "preview_button_name" );
			}
			else if(m_intRenameDatabase == DB_SOURCE_LONG)
			{
				textPut( "text", strNewName, PNL_RENAME, "preview_soft_name" );
			}
			else if(m_intRenameDatabase == DB_DEST_HARDWARE || m_intRenameDatabase == DB_SOURCE_HARDWARE)
			{
				if(m_iRenameDetails)
				{
					textPut( "text", strNewName, PNL_RENAME, m_previewHardDetails );	
				}
				else
				{
					textPut( "text", strNewName, PNL_RENAME, "hidden_hardware_name" );
					bncs_string strPreview = strNewName;
					/*
					if(strNewName.length() > 5)
					{
						strPreview = strNewName.left(5) + "|" + strNewName.mid(5,5);
					}*/
					textPut( "text", strPreview, PNL_RENAME, m_previewHardDetails );
				}
			}			
		}
	}

	//DUMP popup
	else if(b->panel() == POPUP_DUMP){
		if (b->id() == "close"){
			panelRemove(POPUP_DUMP);
		}
	}
}

// all revertives come here
int globalXY::revertiveCallback( revertiveNotify * r )
{
	debug("globalXY::revertiveCallback device=%1 index=%2 info=%3 sInfo=%4", r->device(), r->index(), r->info(), r->sInfo());
	if( r->device() == m_iDeviceNo)
	{
 		//Update the tally above the dest list
		textPut("text", r->sInfo().replace('|', ' '), PANEL_MAIN, "lastSource" );

		//Remember the current tally
		m_intCurrentDestCurrentSource = r->info();

		//Highlight this source in the listbox or clear if no source
		if(m_intCurrentDestCurrentSource)
		{
			setListboxSelectedItem("sources", m_intCurrentDestCurrentSource);
		}
		else
		{
			textPut("selected=none", PANEL_MAIN, "sources");
		}

		//Update the source index label and rename button
		showSourceIndex(m_intCurrentDestCurrentSource);
	}
	else if( r->device() == m_iDeviceLockNo)
	{
		debug("globalXY::revertiveCallback device=%1 index=%2 info=%3 sInfo=%4", r->device(), r->index(), r->info(), r->sInfo());
		if(r->sInfo() == "1")
		{
			textPut("statesheet", "dest_locked", PANEL_MAIN, "btnUnlock");
			m_blnCurrentDestLocked = true;
			controlDisable( PANEL_MAIN, "btnTake" );
			controlDisable( PANEL_MAIN, "btnUndo" );

		}
		else
		{
			textPut("statesheet", "dest_unlocked", PANEL_MAIN, "btnUnlock");
			m_blnCurrentDestLocked = false;
			controlEnable( PANEL_MAIN, "btnTake" );
			controlEnable( PANEL_MAIN, "btnUndo" );
		}
	}
	return 0;
}

// all database name changes come back here
void globalXY::databaseCallback( revertiveNotify * r )
{
	if( r->device() == m_iDeviceNo)
	{
		if( r->database() == 0)
			loadSourceNames( true );
		else if( r->database() == 1)
			loadDestNames( true );
	}
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string globalXY::parentCallback( parentNotify *p )
{

	if( p->command() == "instance")
	{
		load( p->value() );
	}
	else if( p->command() == "router_list" )
	{
		if(p->value().length() > 0){
			m_strRouterList = p->value();
			setRouterList(m_strRouterList);
			//Add object settings look up here, to tell which Rename panels should be shown
			setRenameTypes(m_strRouterList);
		}
		
	}
	
	//Return Commands
	else if( p->command() == "return" )
	{
		//Called by visual editor to persist settings in bncs_ui
		if(p->value() == "all")
		{
			return  
				bncs_string( "router_list=" ) + bncs_string(m_strRouterList) + "\n";
		}
	}
	return "";
}

// timer events come here
void globalXY::timerCallback( int id )
{
}

void globalXY::load( int intRouter )
{
	bncs_string strInstance = m_sltRouterInstances[intRouter];


	//check router type - if nexus show the toggle unused button
	bncs_config cfgInstance = bncs_config(bncs_string("instances.%1").arg(strInstance));
	if(cfgInstance.attr("type") == "router_nexus")
	{
		controlShow(PANEL_MAIN, "btnToggleUnused");
		textPut("text", "Show|Unused Ports", PANEL_MAIN, "btnToggleUnused");
	}
	else
	{
		//controlHide(PANEL_MAIN, "btnToggleUnused");
	}
	m_blnShowUnusedPorts = false;	//always set the display of unused ports to off

	int device;
	getDev(strInstance, &device);

	if(m_iDeviceNo > 0)
	{
		routerUnregister(m_iDeviceNo);
	}

	m_iDeviceNo=device;
	m_iDestNo=m_iSourceNo=0;
	textPut( "text", "", PANEL_MAIN, "sourceNo" );
	textPut( "text", "", PANEL_MAIN, "destNo" );
	textPut( "text", "", PANEL_MAIN, "takeSource" );
	textPut( "text", "", PANEL_MAIN, "takeDest" );
	textPut( "text", "", PANEL_MAIN, "lastSource" );

	controlDisable( PANEL_MAIN, "renameSource" );
	controlDisable( PANEL_MAIN, "renameDest" );
	controlDisable( PANEL_MAIN, "btnUnlock" );

	loadSourceNames();
	loadDestNames();

	// this line is largely redundant but ensures that we get name change notifications
	routerRegister( m_iDeviceNo, 4096, 4096 );

	//get lock device
	getDev(strInstance + "_lock", &m_iDeviceLockNo);
}
	

void globalXY:: loadSourceNames( bool maintainSelectedHighlight )
{
	bncs_string selectedIndex;
	if( maintainSelectedHighlight )
	{
		textGet( "selectedindex", PANEL_MAIN, "sources", selectedIndex);
	}
	
	textPut( "sort", "false", "sources" );//added by Ruth to speed things up and save resources
	textPut( "clear", PANEL_MAIN, "sources" );
	textPut( "text", "", PANEL_MAIN, "sourceNo" );
	textPut( "text", "", PANEL_MAIN, "takeSource" );
	m_iSourceNo = 0;	

	int noSources = getRouterSize( m_iDeviceNo, DB_SOURCE_BUTTON );
	for( int x = 1 ; x <= noSources ; x++ ) // altered to less than or equal to to get the last source
	{
		bncs_string strName;
		routerName( m_iDeviceNo, DB_SOURCE_BUTTON, x, strName );
		bool blnShowPort = true;

		if(!m_blnShowUnusedPorts && strName.left(4) == "N_U_")
		{
			//Don't show this port	
		}
		else
		{
			if( m_blnAlphaSortSource )
			{
				textPut( "add", strName, PANEL_MAIN, "sources" );
			}
			else 
			{
				char szPrefix[5];
				sprintf(szPrefix, "%04d", x);
				textPut( "add", bncs_string("%1 %2").arg(szPrefix).arg(strName), PANEL_MAIN, "sources" );
			}
		}
	}

	if( m_blnAlphaSortSource )
	{
		textPut( "sort", "true", PANEL_MAIN, "sources" );
	}
	else
	{
		textPut( "sort", "false", PANEL_MAIN, "sources" );
	}


	if( maintainSelectedHighlight )
	{
		textPut( "selected.id", selectedIndex, PANEL_MAIN, "sources");
	}
}

void globalXY:: loadDestNames( bool maintainSelectedHighlight )
{
	fIgnoreSelectionChange = true;
	bncs_string selectedIndex;
	if( maintainSelectedHighlight )
	{
		textGet( "selectedindex", PANEL_MAIN, "dests", selectedIndex);
	}
	else
	{
		textPut( "text", "", PANEL_MAIN, "destNo" );
		m_iDestNo = 0;	
		textPut( "text", "", PANEL_MAIN, "takeDest" );
	}

	textPut( "sort", "false", "dests" );//added by Ruth to speed things up and save resources
	textPut( "clear", PANEL_MAIN, "dests" );

	int noDests = getRouterSize( m_iDeviceNo, 1 );
	for(  int x = 1 ; x <= noDests ; x++ )// altered to less than or equal to to get the last destination - Ruth
	{
		bncs_string strName;
		routerName( m_iDeviceNo, DB_DEST_BUTTON, x, strName );
		bool blnShowPort = true;

		strName.replace('\'', '`');

		if(!m_blnShowUnusedPorts && strName.left(4) == "N_U_")
		{
			//Don't show this port	
		}
		else
		{
			if( m_blnAlphaSortDest )
			{
				textPut( "add", strName, PANEL_MAIN, "dests" );
			}
			else 
			{
				char szPrefix[5];
				sprintf(szPrefix, "%04d", x);
				textPut( "add", bncs_string("%1 %2").arg(szPrefix).arg(strName), PANEL_MAIN, "dests" );
			}
		}
	}
	if( m_blnAlphaSortDest )
	{
		textPut( "sort", "true", PANEL_MAIN, "dests" );
	}
	else
	{
		textPut( "sort", "false", PANEL_MAIN, "dests" );
	}

	if( maintainSelectedHighlight )
	{
		textPut( "selected.id", selectedIndex, PANEL_MAIN, "dests");
		bncs_string strName;
		routerName(m_iDeviceNo, DB_DEST_BUTTON, m_iDestNo, strName);

		//Get the current name for the dest
		textPut( "text", strName, PANEL_MAIN, "takeDest" );
	}

	fIgnoreSelectionChange = false;
}

void globalXY::highlightButton(int intRouter)
{
	if(m_intLastRouter > 0)
	{
		textPut("statesheet", "enum_deselected", PANEL_MAIN, bncs_string("router_%1").arg(m_intLastRouter));
	}
	textPut("statesheet", "enum_selected", PANEL_MAIN, bncs_string("router_%1").arg(intRouter));
	m_intLastRouter = intRouter;

	textPut( "text", "Take", PANEL_MAIN, "Take");
}

void globalXY::dump(const bncs_string & var, const bncs_string & value) {
	if((var == "clear") && (value == "clear")){
		textPut("clear", "", POPUP_DUMP, "list_dump");
	}
	else {
		bncs_string strDump = bncs_string("%1 = \"%2\"").arg(var).arg(value);
		textPut("add", strDump, POPUP_DUMP, "list_dump");
	}
}

void globalXY::showDump(void){
	
	panelPopup(POPUP_DUMP, "popup_dump.bncs_ui");
	
	dump( "clear", "clear" );
	dump( "workstation", workstation() );

	dump ( "current_router device_id", m_iDeviceNo );

	bncs_string strSource;
	textGet( "text", PANEL_MAIN, "takeSource", strSource );
	dump ("Current Source", strSource);
	dump ( "current_source index", m_iSourceNo );

	bncs_string strDest;
	textGet( "text", PANEL_MAIN, "takeDest", strDest );
	dump("Current Dest", strDest);
	dump ( "current_dest index", m_iDestNo );

}

void globalXY::showSourceIndex(int intSource)
{
	if(intSource)
	{
		textPut("text", intSource, PANEL_MAIN, "sourceNo" );
		bncs_string strDB2, strDB4;
		routerName(m_iDeviceNo, 2, intSource, strDB2);
		routerName(m_iDeviceNo, 4, intSource, strDB4);
		bncs_string strDetails = bncs_string("%1 - %2").arg(strDB4.replace('|',' ')).arg(strDB2.replace('|',' '));
		textPut("text", strDetails.replace("!!!"," "), PANEL_MAIN, "source_details" );
		controlEnable( PANEL_MAIN, "renameSource" );
	}
	else
	{
		textPut("text", "", PANEL_MAIN, "sourceNo" );
		textPut("text", "", PANEL_MAIN, "source_details" );
		controlDisable( PANEL_MAIN, "renameSource" );
	}
}

void globalXY::setRouterList(bncs_string strRouterList)
{
	//load new routers list
	m_sltRouterInstances = bncs_stringlist(m_strRouterList);

	for(int intRouter = 0; intRouter < MAX_ROUTERS; intRouter++)
	{
		//update the router buttons
		bncs_string strRouterInstance = "";
		if(intRouter < m_sltRouterInstances.count())
		{
			strRouterInstance = m_sltRouterInstances[intRouter];
			//TODO get alt_id for router and display this friendly name on the button
			bncs_config cfgInstance("instances." + strRouterInstance);
			bncs_string strLabel = cfgInstance.attr("alt_id");
			textPut("text", strLabel, PANEL_MAIN, bncs_string("router_%1").arg(intRouter + 1));
			controlEnable(PANEL_MAIN, bncs_string("router_%1").arg(intRouter + 1));
		}
		else
		{
			controlDisable(PANEL_MAIN, bncs_string("router_%1").arg(intRouter + 1));
		}
	}
	
	//load the first
	selectRouter(1);
}

void globalXY::selectRouter(int intRouter)
{
	//Note intRouter is 1 based
	if(intRouter <= m_sltRouterInstances.count())
	{
		highlightButton(intRouter); 
		load(intRouter - 1);
	}
}

void globalXY::setListboxSelectedItem(bncs_string strControl, int intPortIndex)
{
	char szPrefix[5];
	
	if(strControl == "sources" && m_blnAlphaSortSource)
	{
		bncs_string strSourceName;
		routerName( m_iDeviceNo, DB_SOURCE_BUTTON, intPortIndex, strSourceName );
		textPut( "selected.text.exact", strSourceName, PANEL_MAIN, strControl);
	}
	else if(strControl == "dests" && m_blnAlphaSortDest)
	{
		bncs_string strDestName;
		routerName( m_iDeviceNo, DB_DEST_BUTTON, intPortIndex, strDestName );
		textPut( "selected.text.exact", strDestName, PANEL_MAIN, strControl);
	}
	else
	{
		//Jump to the supplied prefix
		sprintf(szPrefix, "%04d", intPortIndex);
		textPut("selected.text.begins", szPrefix, PANEL_MAIN, strControl);
	}
	
	/*
	//Non-working attempts to make a selection in the middle of the list
	//Get the position that the item appears at
	bncs_string strListBoxPosition;
	textGet( "selectedindex", PANEL_MAIN, strControl, strListBoxPosition);

	//Then set the listbox to display the item that is 8 lines lower
	textPut("selected.id", bncs_string(strListBoxPosition.toInt() + 8), PANEL_MAIN, strControl);
		
	//Then select nothing
	textGet( "selected", "none", strControl, strListBoxPosition);

	//Then reselect the correct item
	textPut("selected.text.begins", szPrefix, PANEL_MAIN, strControl);
	*/
}

void globalXY::setRenameTypes(bncs_string sInstance)
{

	if (getObjectSetting("globalXY_Rename_Options_"+sInstance, "editSourceSoftName").lower() == "false")
		m_bRenameSoftNames = false;
	else
		m_bRenameSoftNames = true;

	if(getObjectSetting("globalXY_Rename_Options_"+sInstance, "editHardware/Details").lower() == "false")
		m_iRenameDetails = RENAME_NEITHER;
	else if(getObjectSetting("globalXY_Rename_Options_"+sInstance, "editHardware/Details").lower() == "details")
		m_iRenameDetails = RENAME_DETAILS;
	else //if(getObjectSetting("globalXY_Rename_Options_"+sInstance, "editHardware/Details").lower() == "hardware")
		m_iRenameDetails = RENAME_HARDWARE;

}

void globalXY::renamePanel( bool bDest)
{
	//debug("GLOBALXY::renamePanel == bDest:%1 bDetails:%2");
		panelRemove( PANEL_MAIN);
		panelShow( PNL_RENAME, "rename.bncs_ui" );
		int iRenameIndexNo;
		bncs_string strLabel, strButtonName, strSoftName, strHardwareName;
			
		if(!bDest)
		{
			textPut("text", "Rename Source", PNL_RENAME, "lblRenameAction");
			iRenameIndexNo = m_iSourceNo;

			if(m_bRenameSoftNames)
			{		
				controlShow(PNL_RENAME,"btnSoftName");
				controlShow(PNL_RENAME,"preview_soft_name");
			}
			else
			{
				controlHide(PNL_RENAME,"btnSoftName");
				controlHide(PNL_RENAME,"preview_soft_name");
			}
			routerName( m_iDeviceNo, DB_SOURCE_BUTTON, iRenameIndexNo, strButtonName );
			routerName( m_iDeviceNo, DB_SOURCE_LONG, iRenameIndexNo, strSoftName );
			routerName( m_iDeviceNo, DB_SOURCE_HARDWARE, iRenameIndexNo, strHardwareName );			
		}
		else
		{
			textPut("text", "Rename Destination", PNL_RENAME, "lblRenameAction");
			iRenameIndexNo = m_iDestNo;
			
			if(m_bRenameSoftNames)
			{		
				controlShow(PNL_RENAME,"btnSoftName");
				controlShow(PNL_RENAME,"preview_soft_name");
			}
			else
			{
				controlHide(PNL_RENAME,"btnSoftName");
				controlHide(PNL_RENAME,"preview_soft_name");
			}
			routerName( m_iDeviceNo, DB_DEST_BUTTON, iRenameIndexNo, strButtonName );
			routerName( m_iDeviceNo, DB_DEST_LONG, iRenameIndexNo, strSoftName );
			routerName( m_iDeviceNo, DB_DEST_HARDWARE, iRenameIndexNo, strHardwareName );
		}
		//Put current source details onto panel header
		textPut( "text", iRenameIndexNo, PNL_RENAME, "header_index" );
		strLabel = strButtonName;
		textPut( "text", strLabel.replace('|', ' '), PNL_RENAME, "header_button_name" );
		strLabel = strSoftName;
		textPut( "text", strLabel.replace('|', ' '), PNL_RENAME, "header_soft_name" );
		
		//Put current source details onto panel preview
		textPut( "text", strButtonName, PNL_RENAME, "preview_button_name" );
		textPut( "text", strSoftName, PNL_RENAME, "preview_soft_name" );
		if(m_iRenameDetails == RENAME_DETAILS)
		{
			m_previewHardDetails = "preview_details";
			controlShow(PNL_RENAME,"btnHardwareName");
			textPut("text","Details",PNL_RENAME,"btnHardwareName");
			controlHide(PNL_RENAME,"preview_hardware_name");
			controlShow(PNL_RENAME,"preview_details");
			textPut( "text", strHardwareName, PNL_RENAME, m_previewHardDetails );
		}
		else if(m_iRenameDetails == RENAME_HARDWARE)
		{
			controlShow(PNL_RENAME,"btnHardwareName");
			textPut("text","UMD Prefix", PNL_RENAME,"btnHardwareName");
			controlShow(PNL_RENAME,"preview_details");
			controlHide(PNL_RENAME,"preview_hardware_name");
			textPut( "text", strHardwareName, PNL_RENAME, "hidden_hardware_name" );	

			strLabel = strHardwareName;
			m_previewHardDetails = "preview_details";
			/*
			m_previewHardDetails = "preview_hardware_name";
			if(strHardwareName.length() > 5)
			{
				strLabel = strHardwareName.left(5) + "|" + strHardwareName.mid(5,5);
			}
			else
			{
				strLabel = strHardwareName;
			}
			*/
			textPut( "text", strLabel, PNL_RENAME, m_previewHardDetails );
		}
		else
		{
			controlHide(PNL_RENAME,"preview_hardware_name");
			controlHide(PNL_RENAME,"preview_details");
			controlHide(PNL_RENAME,"btnHardwareName");
		}
}
