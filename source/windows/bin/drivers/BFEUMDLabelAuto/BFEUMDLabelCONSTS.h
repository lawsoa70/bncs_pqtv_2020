
//////////////////////////////////////////////////////////////////////////
//   BFEUMDLabel  Constants file for use across the project
//////////////////////////////////////////////////////////////////////////


#if !defined(AFX_BFEUMDLABELCONSTS_H__INCLUDED_)
#define AFX_BFEUMDLABELCONSTS_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#pragma warning(disable : 4786 4996 4018)

#define		INFO_DISCONNECTED  0	// infodriver not there or whatever
#define		INFO_RXMODE               1		// rx mode only - dual redundancy
#define		INFO_TXRXMODE          2		// connected, and in true rxtx mode


#define ONE_SECOND_TIMER_ID		1	 // could be used to relook for comms port / infodrivers -- for more intelligent driver
#define COMMAND_TIMER_ID		 2							// 

#define ONE_SECOND_TIMER_INTERVAL	 1000 // 1.5 sec timer used for housekeeping - mainly for re connects to info driver, comport connection failures, device interrogation 
#define COMMAND_TIMER_INTERVAL			200 // 0.2 sec interval on commands being sent from the fifobuffer 

#define  CLOCK_TIMER_ID             3
#define	 CLOCK_TIMER_INTERVAL	30000   // to get system date and resolve "NEW" entries once a day -- to maintain space in sncs labels table

#define  UMDS_TIMER_ID             4
#define	 UMDS_TIMER_INTERVAL	5000   // to send all umds now - after initial processing

#define FORCETXRX_TIMER_ID      7					// 

#define BUFFER_CONST			256

#define MAX_COMMS_ERRORS_COUNT		 3 //3

#define MAXSLOTSUSED		4000 // 1..3999   -- also same value for MAX LABELS, MAX ALIAS 
#define MAXDESTINATIONS		257 //   -- should equal or be slightly larger than router size -- 200 sncs labels 200 rtr dests
#define MAXSOURCES		     257 //   -- should equal or be slightly larger than router size -- 200 sncs labels 200 rtr dests
#define MAXSNCSLABELS		200 //   -- should equal or be slightly larger than router size -- 200 sncs labels 200 rtr dests

#define STARTROUTERSLOTS 0			//  slot 1 really, slot number for start of router based data in ext infodriver for panels
#define STARTSNCSSLOTS     2000		// slot 2001 really, slot number for start of sncs based data in ext infodriver for panels

// slot usage
// 1..3999 for data for destinations - labels etc -- as cdl most likely
#define COMM_STATUS_SLOT				4001

#define STUDIO_A 1
#define STUDIO_B 2
#define STUDIO_C 3
#define STUDIO_D 4


enum eRevertives{
	eNoRevertives, // signifies info driver not exist
	eSNCSData,     // SNCS revertives data
	eRouterData, // Router revertives Data
	eCCPanelsData, // these revertives linked to Automatic info ??
	eBFEDriverData  // BFE UMD driver revertives data
};

enum eAliasSourceType {
	ePlainType,           // just plain router
	eLocalType,          // from the local list in object settings -- eg cameras   (( but could be all plain too -- ie srcname|<softname> ))
	eCameraType,			// cameras -- subset of local
	eExternalType,      // all sncs and user defined alias and labels can use these type
	eLoopedType       // from the looped list in object settings -- eg previews and other re-enterant src / dests
};

enum eAliasPermTempForm {
	eUnknownForm,
	eTemporaryForm, // most are temporary
	ePermamentForm // some will be perm -- cannot be deleted from panels 
};

enum eAliasAutoManuStyle {
	eNoStyle,		// will mean no label / alias used so db0 src name is only one  used
	eAutomaticStyle,		// automatic version -- same as router src name 
	eManualStyle // secondary version -- basically a src could have 2 different labels - may be a temporary label for line up etc
};

enum ePanelCommandType {
	eNoCommand,
	eAmendCommand,
	eAddCommand,
	eRemoveCommand,
	eDeleteCommand,
	eClearCommand
};


#define SPACE 32  // space char
#define UNDERSCORE 95 // _ char -- used to replace spaces and  visa versa
#define TILDA 126 // ~ char  used as delimiter for cdlists.


#endif // !defined(AFX_BFEUMDLABELCONSTS_H__INCLUDED_)
