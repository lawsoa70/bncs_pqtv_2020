/****************************************************************************
*																									 *
*			BBC_MXC.H		:	BBC BNCS Message eXchange Constants					 *
*																									 *
*																									 *
* Copyright Atos 2019
****************************************************************************/

#ifndef BBC_MXC_INCLUDED 
	#define BBC_MXC_INCLUDED 

#ifndef WM_USER
#ifndef _WIN32
#define WM_USER	0x400
#endif
#endif

#define BBC_APPLCOREHWND		WM_USER+1001   // A message from ApplCore to AppleX or other external containing its handle
#define BBC_CLOSEDOWN			WM_USER+1002   // ApplCore internal closedown request
#define BBC_CLOSEDOWNACK		WM_USER+1003	// ApplCore internal closedown request acknowledge
#define BBC_RESOURCECHECK		WM_USER+1004	// ApplCore internal resource check
#define BBC_EXECBUTTON			WM_USER+1005	// Posted ExecButton command
#define BBC_IACRETRY			WM_USER+1006	// Posted IAC retry message
#define BBC_GETHWNDCSI			WM_USER+1007	// Local broadcast to ask CSI for its HWND
#define BBC_HWNDCSI				WM_USER+1008	//	Message from CSI returning its HWND
#define BBC_LAUNCH				WM_USER+1009	// Broadcast from CSI to Launch to commence boot sequence
#define BBC_REGAPPLCORE			WM_USER+1010	// Request from ApplCore to register or de-register with CSI
#define BBC_REGCLIENT			WM_USER+1010	// A more generic client message.  Same as BBC_REG_APPLCORE
#define BBC_GETCSIVERSION		WM_USER+1011	// Message from a client to CSI requesting its version number
#define BBC_CACHESWITCH			WM_USER+1012	// Message from client to CSI to switch its cache on or off.
#define BBC_CACHETEST			WM_USER+1013	// Message from client to CSI asking if cache for a device is valid.
#define BBC_CACHEFLUSH			WM_USER+1014	// Message from client to CSI asking it to flush the cache
#define BBC_CLIENTREQPKTMODE	WM_USER+1015	// Message from a client requesting packet mode
#define BBC_QUERYCACHEMODE		WM_USER+1016	// Message from client to CSI asking if cache is on or off.
#define BBC_GETDEVPROPERTIES	WM_USER+1017	// Message from client to CSI asking for device properties
#define BBC_HWNDDRV				WM_USER+1018	// Message from driver to external client delivering it handle
#define BBC_ACTIVATEAPP			WM_USER+1019	// Used for turning a WM_ACTIVATE sendmessage into a posted message

#define BBC_POSTEDRTRMSG		WM_USER+1020	// Posted message within CSI to allow packet disassembly
#define BBC_POSTEDGPIMSG		WM_USER+1021	// Posted message within CSI to allow packet disassembly
#define BBC_POSTEDINFOMSG		WM_USER+1022	// Posted message within CSI to allow packet disassembly
#define BBC_POSTEDUNIMSG		WM_USER+1023	// Posted message within CSI to allow packet disassembly
#define BBC_POSTEDDIRECTMSG	WM_USER+1024	// Posted message within CSI to allow direct message between panels and drivers
#define BBC_POSTEDCOMBINERMSG	WM_USER+1025	// Posted message within the combiner to return control to windows before processing package
#define BBC_POSTEDCOMBINERIWMSG	WM_USER+1026	// Posted message within the combiner to return control to windows before processing IW's

#define BBC_REGCLIENT32			WM_USER+1030	// Request from 32 bit client to register or de-register with CSI
#define BBC_COPYDATA          WM_USER+1050   // Used for the BNCS 16/32 and 32/32 bit messaging schema (DLL messsaging COPY DATA I/F) 

#define BBC_FBMSGSCHEME			WM_USER+1055	// Flag in CSI to hold the message scheme used by FBCSLINK
#define BBC_CLMSGSCHEME			WM_USER+1056	// Flag in CSI to hold the message scheme used by CSLINK

#define BBC_ACFASTCLOSE       WM_USER+1060   // Posted message within ApplCore to do a fast closedown
#define BBC_ACSETPARAMS			WM_USER+1061   // Message sent from one ApplCore panel to another carrying variables for S$ and S% prior to WAKEUP

#define BBC_NETSTATUSMSG		WM_USER+1094	// Message sent via CSI to all clients carryin an network messages to clients and drivers
#define BBC_GETWORKSTATIONNUM	WM_USER+1095	// Message from client or driver to CSI to retrieve workstation number
#define BBC_EXTCOMMANDMSG		WM_USER+1096	// Message from external driver to host driver delivering a command
#define BBC_DEVCOMMANDMSG		WM_USER+1097	// Message from client sent via CSI to all drivers carrying commands or requests
#define BBC_DEVREVERTIVEMSG	WM_USER+1098	// Message from client sent via CSI to all drivers carrying commands or requests
#define BBC_DEVSTATUSMSG		WM_USER+1099	// Message sent via CSI to all clients carryin an NS message.

#define BBC_TXRTRNETMSG			WM_USER+1100	//	Message from ApplCore to CSI for normal router TX over the network
#define BBC_TXGPINETMSG			WM_USER+1110	//	Message from ApplCore to CSI for GPI TX over the network
#define BBC_TXINFONETMSG		WM_USER+1120	//	Message from ApplCore to CSI for InfoDriver TX over the
#define BBC_TXDEVNETMSG			WM_USER+1125	// Message from CSI client to all CSI device driver handlers
#define BBC_TXBKGNETMSG			WM_USER+1130	//	Message from Booking Client to CSI for Booking Server TX over the network
#define BBC_TXCSINETMSG			WM_USER+1135	// Message from CSI client to all CSI's via revertive handler
#define BBC_TXREVPACKET			WM_USER+1136	// Message from device (CSLIENT) requesting to send a raw revertive packet via CSI

#define BBC_NETMODIFY			WM_USER+1137	// Network message sent via CSI to modify an INI file
#define BBC_INIFILECHANGE		WM_USER+1138	// Message sent via CSI to all clients and drivers indicating INI file change.
#define BBC_NETTXRXSTATUS		WM_USER+1139	// Message sent via CSI to all clients carryin an NI or NA message.

#define BBC_GETDBASENAME		WM_USER+1140	// Database source/destination name request from ApplCore to CSI
#define BBC_GETDBASEINDEX		WM_USER+1141	// Database source/destination index request from ApplCore to CSI
#define BBC_GETDBASEINDEXEXT	WM_USER+1142	// High order database index request from ApplCore to CSI

#define BBC_CLEARDATABASECACHE	WM_USER+1143	// if using memory databases this is a request to flush all or specific (device no in wParam) databases

#define BBC_GETCACHEDDATA	 	WM_USER+1145	// Get cached slot contents request from client to CSI
#define BBC_GETCACHEDINDEX	 	WM_USER+1146	// Get cached slot index request from client to CSI
#define BBC_DATABASECHANGE		WM_USER+1150	// Signal from CSI to ApplCore that the database has changed
#define BBC_CSICLOSING			WM_USER+1151	// Signal from CSI to clients that it is closing down
#define BBC_CSICLOSEWARNING	WM_USER+1152	// Signal from CSI to say that it will soon be closing down
#define BBC_DATABASEPOLL		WM_USER+1153	// Signal from CSI to say that driver should send full poll response
#define BBC_GETMAPPEDDEVID    WM_USER+1154   // Request from CSI client to CSI asking for database mapping if any
#define BBC_REQDEVGORXONLY    WM_USER+1155   // Request from CSI to device driver asking it to go RX only
#define BBC_REQDEVGOTXRX      WM_USER+1156   // Request from CSI to device driver asking it to go TX
#define BBC_INQDEVGOTXRX      WM_USER+1157   // Request from CSI to device driver asking it if it wants to go TX

#define BBC_SENDCSIDATAGRAM   WM_USER+1158   // Message from CSI client requesting a datagram to CSI be transmitted direct to the network
#define BBC_SENDDRVDATAGRAM   WM_USER+1159   // Message from CSI client requesting a datagram to drivers be transmitted direct to the network

#define BBC_REGDEVICEID			WM_USER+1160	// Message from Device Driver to CSI registering for a device
#define BBC_UNREGDEVICEID		WM_USER+1161	// Message from Device Driver to CSI unregistering a device
#define BBC_DEVDRVCLOSE			WM_USER+1162	// Message from Device Driver to CSI saying its closing down
#define BBC_DEVDRVREVMSG	  	WM_USER+1163	// Revertive message from Device Driver to CSI carrying update revertive
#define BBC_DEVDRVPOLLMSG		WM_USER+1164	// Revertive message from Device Driver to CSI carrying result of a poll request
#define BBC_CLIENTMSG		  	WM_USER+1165	// Client string message from CSI to device driver
#define BBC_DEVICEIDINUSE		WM_USER+1166   // Message from CSI to a driver saying that a device ID is already in use on the network
#define BBC_DEVDRVVERBATIM		WM_USER+1167	// Message from Device Driver to CSI carrying verbatim message to send to the network
#define BBC_REGALLDEVICEIDS	WM_USER+1168	// Message from an external client/driver such as CSCLIENT requesting to redirect all Ids
#define BBC_REGDEVICEID32		WM_USER+1169	// Message from Device Driver to CSI registering for a device  (DLL messsaging COPY DATA I/F)

#define BBC_DEVCONNECT			WM_USER+1170	// Message from ApplCore to CSI requesting session connection with remote device driver
#define BBC_DEVDISCONNECT 		WM_USER+1171	// Message from ApplCore to CSI requesting to drop connection with remote device driver
#define BBC_DEVWRITE				WM_USER+1172	// Message from ApplCore to CSI requesting to write on the session with remote device driver
#define BBC_DEVREAD				WM_USER+1173	// Message from ApplCore to CSI requesting to read on the session with remote device driver

#define BBC_SESSIONREQUEST		WM_USER+1180	// Client message via CSI to a driver requesting a session dialogue
#define BBC_SESSIONCLOSE		WM_USER+1181	// Driver message to CSI requesting closure a session

#define BBC_GETDEVTXRXMODE    WM_USER+1190   // Message from device to CSI requesting Tx/Rx mode
#define BBC_SETDEVTXRXMODE    WM_USER+1191   // Message from device to CSI setting Tx/Rx mode

#define BBC_RXRTRNETMSG			WM_USER+1200	//	Revertive router NetBIOS Rx datagram via CSI to ApplCore applications
#define BBC_RXGPINETMSG			WM_USER+1210	//	Revertive GPI NetBIOS Rx datagram via CSI to ApplCore applications
#define BBC_RXINFONETMSG		WM_USER+1220	//	Revertive InfoDriver NetBIOS Rx datagram via CSI to ApplCore applications
#define BBC_RXBKGNETMSG			WM_USER+1230	//	Revertive Booking System NetBIOS Rx datagram via CSI to Booking Client application
#define BBC_RXSESSIONRESULT	WM_USER+1240	// Revertive session result from CSI to client

#define BBC_REGDEVICE			WM_USER+1250	// Register for revertive with network device
#define BBC_UNREGDEVICE			WM_USER+1251	// Unregister client with CSI for network device
#define BBC_UNREGDEVICERANGE	WM_USER+1252	// Unregister client with CSI for network device between a min and max

#define BBC_POSTPAINT			WM_USER+1300	// Internal delayed paint message
#define BBC_INTERNALPOST		WM_USER+1310	// Internal message
#define BBC_ASYNCPOST			WM_USER+1311	// Internal asynch message
#define BBC_CSIPOSTEDPOLL	 	WM_USER+1320	// Internal CSI Message
#define BBC_CSIPOSTEDHANGUP	WM_USER+1330	// Internal CSI Message
#define BBC_CSIPOSTEDSEND	WM_USER+1331	// Internal CSI Message

#define BBC_GETBKGSERVERHWND	WM_USER+1400	// Message from BSLIBMOD to get handle of BSSERVER
#define BBC_BKGSERVERHWND		WM_USER+1401	// Message from BSSERVER to give handle to BSLIBMOD
#define BBC_GETBKGLIBMODHWND	WM_USER+1410	// Message from BSSERVER to get handle of BSLIBMOD
#define BBC_BKGLIBMODHWND		WM_USER+1411	// Message from BSLIBMOD to give handle to BSSERVER
#define BBC_DAYCHANGED			WM_USER+1420	// Message from BSLIBMOD to BSSERVER to indicate change of data on a particular day
#define BBC_DUMPREQPENDING		WM_USER+1421   // Message from BSSERVER to BSLIBMOD to say that a dump request file is pending
#define BBC_AMSMSGPENDING		WM_USER+1422	// Message from BSSERVER to BSLIBMOD to say that an outgoing AMS message file is pending
#define BBC_PRINTALLAMS			WM_USER+1423	// Message from BSSERVER to BSLIBMOD to say print all its AMS buffer
#define BBC_PRINT100AMS       WM_USER+1424	// Message from BSSERVER to BSLIBMOD to say print last 100 lines of its AMS buffer

#define BBC_WINSOCKRX			WM_USER+1500	// Message from WINSOCK.DLL
#define BBC_BNCSSOCKMSG			WM_USER+1501	// Message from BNCSSOCK.DLL

#define BBC_CLREGISTER 			WM_USER+1600	// Message from CSLINK to CSI requesting registration
#define BBC_CLUNREGISTER 		WM_USER+1601	// Message from CSLINK to CSI requesting de-registration
#define BBC_CLNETMSG				WM_USER+1602	// Network message from CSI to CSLINK.EXE

#define BBC_FBCSLINKREG			WM_USER+1605   // Registration request from FBCSLINK V2 to CSI
#define BBC_FBCSLINKUNREG	 	WM_USER+1606   // Unregistration request from FBCSLINK V2 to CSI

#define BBC_IDREVINFO			WM_USER+1698	// Message from Infodriver to external driver delivering Id, slot and contents
#define BBC_IDSENDSLOTS			WM_USER+1699	// Message from external driver to InfoDriver requesting it sends a range of slots to the network
#define BBC_IDCONNECT			WM_USER+1700	// Message from external driver to InfoDriver requesting handle
#define BBC_IDDISCONNECT		WM_USER+1701	// Message from external driver to InfoDriver disconnecting
#define BBC_IDSETSLOT			WM_USER+1702	// Message from external driver to InfoDriver requesting to set slot contents and send revertive
#define BBC_IDUPDATESLOT		WM_USER+1722	// Message from external driver to InfoDriver requesting to set slot contents and send revertive if the value has changed
#define BBC_IDGETSLOT			WM_USER+1703   // Message from external driver to InfoDriver requesting to get slot contents
#define BBC_IDSLOTMSG			WM_USER+1704	// Message from InfoDriver to external driver delivering slot contents
#define BBC_IDHANDLE				WM_USER+1705	// Message from Infodriver to external driver delivering it handle
#define BBC_IDUSERSTR         WM_USER+1706   // Message from Infodriver to external driver delivering a client string
#define BBC_IDSETSLOTONLY		WM_USER+1707	// Message from external driver to InfoDriver to set slot contents but not update network
#define BBC_IDGETTXRXMODE		WM_USER+1708	// Message from external driver to InfoDriver requesting its Rx/TxRx status
#define BBC_IDSETTXRXMODE		WM_USER+1709	// Message from external driver to InfoDriver requesting it to change its Rx/TxRx status

#define BBC_GRDCONNECT			WM_USER+1710	// Message from external driver to GRD requesting handle
#define BBC_GRDDISCONNECT		WM_USER+1711	// Message from external driver to GRD disconnecting
#define BBC_GRDSETDEST			WM_USER+1712	// Message from external driver to GRD telling it to update tally its table with new source
#define BBC_GRDGETDEST			WM_USER+1713   // Message from external driver to GRD requesting tally table information
#define BBC_GRDDESTMSG			WM_USER+1714	// Message from GRD to external driver requesting it to set a crosspoint
#define BBC_GRDHANDLE			WM_USER+1715	// Message from GRD to external driver delivering it handle
#define BBC_GRDUSERSTR			WM_USER+1716	// Message from GRD to external driver delivering a client string
#define BBC_GRDDESTINFO			WM_USER+1717	// Message from GRD to external driver delivering destination information
#define BBC_GRDREVINFO			WM_USER+1718	// Message from GRD to external driver : similar to BBC_GRDDESTINFO but with router number included
#define BBC_SETOUTPUTMODE     WM_USER+1719	// Message from external controller to GRD requesting it to set its output mode for a destination
#define BBC_GETOUTPUTMODE     WM_USER+1720	// Message from external controller to GRD requesting it return its output mode for a destination
#define BBC_GRDSETDESTONLY        WM_USER+1721    // Message from external driver to GRD telling it to update tally its table with new source, without updateing network

#define BBC_GPIDCONNECT			WM_USER+1730	// Message from external driver to GPID requesting handle
#define BBC_GPIDDISCONNECT		WM_USER+1731	// Message from external driver to GPID disconnecting
#define BBC_GPIDSETIO			WM_USER+1732	// Message from external driver to GPID telling it to update tally its table with new state
#define BBC_GPIDGETIO  			WM_USER+1733   // Message from external driver to GPID requesting tally table information
#define BBC_GPIDHANDLE			WM_USER+1734	// Message from GPID to external driver delivering its handle
#define BBC_GPIDREVINFO			WM_USER+1735	// Message from GPID to external driver carrying revertive information
#define BBC_GPIDIOINFO			WM_USER+1736	// Message from GPID to external driver delivering IO information
#define BBC_GPIDUSERSTR			WM_USER+1737	// Message from GPID to external driver delivering a client string
#define BBC_GPIDIOMSG			WM_USER+1738	// Message from GPID to external driver requesting it to make a switch
#define BBC_GPIDSETIOONLY        WM_USER+1739    // Message from external driver to GPID telling it to update tally its table with new state, without updating network

#define BBC_CTRLCONNECT			WM_USER+1750	// Message from external client to driver requesting handle
#define BBC_CTRLDISCONNECT		WM_USER+1751	// Message from external client to driver disconnecting
#define BBC_CTRLDRIVERSTR 		WM_USER+1752	// Message from driver external client delivering command
#define BBC_CTRLCLIENTSTR		WM_USER+1753	// Message from external client to driver delivering command
#define BBC_CTRLGETDATA			WM_USER+1754	// Message from external client to driver asking for dest/IO/Slot data

#define BBC_VDSLOTUPDATE		WM_USER+1791	// Internal message within VIDEODRV.EXE

#define BBC_FBNEWBKG			WM_USER+1800	// Broadcast message from FBAMSLIB.EXE to indicate new booking
#define BBC_GETFBSCHEDHWND		WM_USER+1801	// Message requesting the handle to FBSECHED.EXE
#define BBC_HWNDFBSCHED			WM_USER+1802   // Message returning the handle to FBSECHED.EXE

#define BBC_NAGETACHWND			WM_USER+1900   // NPANGEL.DLL
#define BBC_NAACHWND			WM_USER+1901   // NPANGEL.DLL
#define BBC_NAINITAC			WM_USER+1902	// NPANGEL.DLL
#define BBC_NACLOSEDOWN			WM_USER+1911	// NPANGEL.DLL
#define BBC_NADISPLAYHWND		WM_USER+1921	// NPANGEL.DLL

#define BBC_RXNETMSG				WM_USER+1950	// Intername message within BNCS Active X Applications

#define BBC_COPYDATABACK		WM_USER+1960	// BNCS 32 message return

#define BBC_AXACCONNECT			WM_USER+1970	// Message from Applex to ApplCore requesting connection
#define BBC_AXACDISCONNECT 	WM_USER+1971	// Message from Applex to ApplCore requesting disconnection
#define BBC_AXACSETSTMEM		WM_USER+1972	// Request, typically from AppleX to AppleCore to set a string table line
#define BBC_AXACGETSTMEM		WM_USER+1973	// Request, typically from AppleX to AppleCore the return of a string table line
#define BBC_AXACGETNEXTSTMEM	WM_USER+1974	// Request, typically from AppleX to AppleCore the return of the next string table line
#define BBC_AXACEXECSTMEM		WM_USER+1975	// Request, typically from AppleX to AppleCore to execute a stringtable line from memeory
#define BBC_AXACERASESTMEM		WM_USER+1976	// Request, typically from AppleX to AppleCore to erase all stringtable memories
#define BBC_AXACFILECMD			WM_USER+1977	// Request, typically from AppleX to AppleCore to save, load or append an AST file

#define BBC_ACAXEXECCMD			WM_USER+1980	// Command message from ApplCore to Applex

#define BBC_GETEXTAPPINFO		WM_USER+1990   // Message from FBSERVER to a none BNCS app requesting information item
#define BBC_SETEXTAPPINFO		WM_USER+1991   // Message from FBSERVER to a none BNCS app sending information item

#define BBC_SPLITCONRANGE		WM_USER+2100	// range of splitter connect (Tim Alden addition)

#define WM_GETLOCALTEXT			WM_USER+2101
#define BBC_SMARTPRESSED		WM_USER+2111 		//Smart button sends this to group master as new group selection
#define BBC_SMARTDESELECT		WM_USER+2112		//Group master sends this to last smart pressed to deselect it
#define BBC_SMARTTALLY			WM_USER+2113		//Group master sends this to control which receives CSI tally
#define BBC_IDDRIVERNUM			WM_USER+2114		//

#define BBC_GETHWNDSHIM			WM_USER+2141	// same as BBC_GETHWNDCSI but for custom control shim bbc_smcc.exe
#define BBC_REGSHIM				WM_USER+2142	// same as BBC_REGAPPLCORE but for custom control shim bbc_smcc.exe

#define BBC_GETHWNDSHIM_ALT			WM_USER+2143	// same as BBC_GETHWNDCSI but for custom control shim bbc_smcc.exe

#define	BBC_IS_APPLCORE_WINDOW	WM_USER+2150	// responds with "magic" number, if applcore window
#define APPLCORE_CONFIG_MAGIC	(0xf1d3b597)	// reply value for BBC_IS_APPLCORE_WINDOW (a piece of antiquity)  :-) */
#define	BBC_APPLCORE_STARTUP	WM_USER+2151	// delayed startup mechanism. 



#define WM_BNCS					0xC000

#define BBC_GETHWNDCSI32		WM_BNCS+1	// Local broadcast to ask CSI for its HWND
#define BBC_GETHWNDCSI32_ALT	WM_BNCS+111	// Local broadcast to ask CSI for its HWND
#define BBC_IDCONNECT32			WM_BNCS+2	// Message from external driver to InfoDriver requesting handle
#define BBC_GRDCONNECT32		WM_BNCS+3	// Message from external driver to GRD requesting handle

#define BBC_APPREADY			WM_BNCS+9	// Broadcast from BNCS application to say that it has initialised. (Used by V3Launch)
#define BBC_APPREADY2			WM_BNCS+11	// Broadcast from BNCS application to say that it has initialised. (Used by V3Launch)
#define BBC_CTRLCONNECT32	 	WM_BNCS+12	// Message from external client to driver requesting handle (Send using SendMessage and only use WM_COPYDATA once connected)
#define BBC_CTRLCONNECT32_DLL 	WM_BNCS+112	// Message from external client to driver requesting handle (Send using SendMessage and only use WM_COPYDATA once connected)
#define BBC_CTRLDISCONNECT32	WM_BNCS+13	// Message from external client to driver disconnecting
#define BBC_LAUNCHAPPLICATION	WM_BNCS+14	// Message from logic engine to GUI requesting that it launch another application
#define BBC_QUERYCLIENTREG		WM_BNCS+15	// Message from client to CSI registering its name.
#define BBC_REGCLIENTNAME		WM_BNCS+16	// Message from client to CSI registering its name.
#define BBC_RUT					WM_BNCS+17	// Message from one app to another to see if it still there
#define BBC_NETMSGUPDATE		WM_BNCS+18		// Message from V4CSI to clients carrying verbatim N series messages

#define BBC_GUICOMMANDID		WM_BNCS+20   // Command Id from GUI to logic dll
#define BBC_GUIUPDATE			WM_BNCS+21   // Message from GUI that any updating should be done now
#define BBC_GUIPARAMVALUE		WM_BNCS+22   // Inter-GUI Parameter=Value message
#define BBC_GUIPANELMESSAGE		WM_BNCS+23   // Inter-GUI panel message
#define BBC_GUIPANELVARIABLE	WM_BNCS+24   // Inter-GUI panel variable copy message

#define BBC_SETLIBRARYID		WM_BNCS+30   // Message to a library allocating it a unique library Id.
#define BBC_GETLIBRARYID		WM_BNCS+31   // Message to a library retrieving its unique library Id.
#define BBC_SETDEVICEID			WM_BNCS+32   // Message to a library allocating it a unique device Id.
#define BBC_GETDEVICEID			WM_BNCS+33   // Message to a library retrieving its unique device Id.

#define BBC_SETDEBUGMODE		WM_BNCS+40   // Message to set debug mode
#define BBC_DEBUGMESSAGE		WM_BNCS+41   // Debug message string 

#define BBC_ROUTERCONNECT    	WM_BNCS+50
#define BBC_ROUTERINTERROGATE	WM_BNCS+51

#define BBC_VIDEODISABLE		WM_BNCS+60   // Message to overlay application to closedown

#define BBC_PKTSERNUMERR_REG		(WM_BNCS+70)	// register for unrecoverable packet errors
#define BBC_PKTSERNUMERR_UNREG		(WM_BNCS+71)
#define BBC_PKTSERNUMERR			(WM_BNCS+72)	// notification of unrecoverable packet error

#define BBC_NETTXRXSTATUS_REG		(WM_BNCS+73)	// registration for BBC_NETTXRXSTATUS
#define BBC_NETTXRXSTATUS_UNREG		(WM_BNCS+74)
#define BBC_NETTXRXSTATUS_BROADCAST_ENABLED		(WM_BNCS+77)

#define BBC_NETMSGUPDATE_REG		(WM_BNCS+75)	// registration for BBC_NETMSGUPDATE
#define BBC_NETMSGUPDATE_UNREG		(WM_BNCS+76)
#define BBC_NETMSGUPDATE_BROADCAST_ENABLED		(WM_BNCS+78)

#define BBC_GETCLIENTREGEXTENT       (WM_BNCS+80)      // means of querying range of destinations current registered for

#define BBC_GETMAXDEVICES			(WM_BNCS+93)	// get the maximum number of devices that CSI can control

#define BBC_GETMAXDESTS				(WM_BNCS+81)	// get the maximum number of destinations that CSI can control
#define BBC_GETMAXSOURCES			(WM_BNCS+82)	// get the maximum number of sources that CSI can control
#define BBC_GETMAXSOURCENAMES		(WM_BNCS+83)	// get the maximum number of source names that CSI supports
#define BBC_GETMAXDESTNAMES			(WM_BNCS+84)	// get the maximum number of dest names that CSI supports

#define BBC_SENDQUEUEDCOMMANDS		(WM_BNCS+85)	// force immediate send of queued commands (rather than waiting or setting flag on last command)
#define BBC_SENDQUEUEDREVERTIVES	(WM_BNCS+86)	// force immediate send of queued revertives (rather than waiting or setting flag on last revertive)

#define BBC_CONFIGCHANGE_NOTIFY		(WM_BNCS+87)
#define BBC_CONFIGCHANGE_REQUEST	(WM_BNCS+88)
#define BBC_CONFIGCHANGE_REG		(WM_BNCS+89)
#define BBC_CONFIGCHANGE_UNREG		(WM_BNCS+90)

#define BBC_GETMAXDATABASES			(WM_BNCS+91)	// get the maximum number of databases that CSI can load (was 2, then 10, now 65536)
#define BBC_SETREDUNDANCYMASTER		(WM_BNCS+92)
#define BBC_GETMAXSTRINGSIZE		(WM_BNCS+94)
#define BBC_GETMAXWORKSTATIONS	    (WM_BNCS+95)

#define BBC_GATEWAYREGISTER			(WM_BNCS+601)	// wParam is window handle to register, LOWORD(lParam) is true to register, false to deregister
#define BBC_GATEWAYSEND				(WM_BNCS+602)	// wParam is driver number, lParam is command to send
#define BBC_GATEWAYRECEIVE			(WM_BNCS+603)	// LOWORD( wParam ) is target ws, HIWORD( wParam ) is originating ws, lParam is string command/revertive

#define BBC_PREINITIALISE			WM_BNCS+2019   // Generic pre-initialise command
#define BBC_INITIALISE				WM_BNCS+2020   // Generic initialise command
#define BBC_TIMER					WM_BNCS+2021   // Message delivered by the CBBC_timer class
#define BBC_SETIPCMODE				WM_BNCS+2022   // Sets the the IPC mode used by the BBC_csi_if library
#define BBC_SETIPCDATASIZE			WM_BNCS+2023   // Sets the data size for lParam in IPC comms *


#define BBC_DPI_LOAD				WM_BNCS+100
#define BBC_DPI_UNLOAD				WM_BNCS+101
#define BBC_DPI_DATA				WM_BNCS+102

#define BBC_DCI_LOAD				WM_BNCS+300	// Loads a DCI library
#define BBC_DCI_UNLOAD				WM_BNCS+301	// Unloads a DCI library
#define BBC_DCI_OPEN				WM_BNCS+302	// Opens a connection on the DCI's port
#define BBC_DCI_CLOSE				WM_BNCS+303	// Closes a connection on the DCI's port
#define BBC_DCI_WRITE				WM_BNCS+304	// Writes data
#define BBC_DCI_READ				WM_BNCS+305	// Reads data
#define BBC_DCI_LSEEK				WM_BNCS+306   // Seeks to a postion within the file or stream
#define BBC_DCI_READDIR				WM_BNCS+307	// Unlikely to be implememented. Included for completeness
#define BBC_DCI_POLL				WM_BNCS+308	// Unlikely to be implememented. Included for completeness
#define BBC_DCI_IOCTL				WM_BNCS+309   // Used to set parameters on on a port
#define BBC_DCI_FLUSH				WM_BNCS+310   // Flushes Tx and/or Rx buffers
#define BBC_DCI_FSYNC				WM_BNCS+311	// Used to ensure I/O operations are complete
#define BBC_DCI_FASYNC				WM_BNCS+312	// Unlikely to be implememented. Included for completeness
#define BBC_DCI_CHKMEDIACHANGE		WM_BNCS+0313	// Unlikely to be implememented. Included for completeness
#define BBC_DCI_MMAP				WM_BNCS+314	// Unlikely to be implememented. Included for completeness
#define BBC_DCI_REVALIDATE			WM_BNCS+315	// Unlikely to be implememented. Included for completeness
#define BBC_DCI_LOCK				WM_BNCS+316	// Unlikely to be implememented. Included for completeness

#define BBC_NOTWPARAM				WM_BNCS+400	// can be used for checking that this app is CSI - will simply NOT the value in wParam and return it

#define BBC_ALM_UNIREVERTIVE		WM_BNCS+401
#define BBC_ALM_DRIVERREG			WM_BNCS+402	// wParam contains driver number
#define BBC_ALM_DRIVERUNREG			WM_BNCS+403	// wParam contains driver number
#define BBC_ALM_REG					WM_BNCS+404
#define BBC_ALM_UNREG				WM_BNCS+405



/* these are to go from the DCI -> DPI when events occur there may be more*/
#define BBC_DCI_READ_EVENT			0x0   // Signal from a DCI to its parent that data is available for reading
#define BBC_DCI_ERROR_EVENT			0x2   // Signal from a DCI to its parent that an error has occurred
#define BBC_DCI_CONNECTED_EVENT 	0x3   // Signal from a DCI to its parent that the DCI is connected


/* this one must be unique to each DCI */
#define BBC_DCI_IOCTL_SERIAL		0x0
#define BBC_DCI_IOCTL_USB			0x2
#define BBC_DCI_IOCTL_IP			0x3
#define BBC_DCI_IOCTL_TCP			0x4
#define BBC_DCI_IOCTL_SNMP			0x5
#define BBC_DCI_IOCTL_ISA			0x6
#define BBC_DCI_IOCTL_PCI			0x7

/***************************/

#define BBC_GRD_MODE_ROUTER		0x0000
#define BBC_GRD_MODE_DRIVER		0x0001

#define BBC_APPCLOSEDOWN			0x9001
#define BBC_ABOUTBOX					0x9002
#define BBC_MESSAGEBOX				0x9003
#define BBC_DEBUGMODE				0x9004

#define BNCS_INFOWRITE	(WM_BNCS+501)

#endif // BBC_MXC_INCLUDED
