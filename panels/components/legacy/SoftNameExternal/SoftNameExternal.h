#ifndef SoftNameExternal_INCLUDED
	#define SoftNameExternal_INCLUDED

#include <bncs_script_helper.h>
#include "SoftNameData.h"

#ifdef WIN32
	#ifdef DOEXPORT_SCRIPT
		#define EXPORT_SCRIPT __declspec(dllexport) 
	#else
		#define EXPORT_SCRIPT __declspec(dllimport) 
	#endif
#else 
	#define EXPORT_SCRIPT
#endif
 
class SoftNameExternal : public bncs_script_helper
{
private:

#define STARTROUTERSLOT 0
#define STARTSNCSSLOT 2000
#define FINALSNCSSLOT 2201

	bncs_string m_SoftNameInstance;

	int iLabelAutoDevice;
	int iRouterDevice;

	int iRouterSourceSize, iRouterDestinationSize;
	bncs_stringlist ssl_ExternalSources;

	int iCurrentSource;
	int iCurrentIndex;
	int iCurrentSNCSAliasIndex;
	int iCurrentEditingWhichName;
	bncs_string ssChosenCurrentStyle;
	bncs_string ssChosenCurrentForm;
	bool bPanelStarting;

	SoftNameData *allSoftData;

	int getDeviceNo( const bncs_string & );

	void registerForAutoRtrRevsAndPoll(void );
	void registerForAutoSncsRevsAndPoll(void );
	int isSourceExternal( int iSource );
	void updateListSourceData( int iSource );
	void ClearAllExternalSoftNames(void); 
	void ClearExternalSoftName(int iIndex); 
	void SaveExternalSoftName(int iIndex); 
	void ProcessNewSoftName(int iIndex);
	void ResetPanel( void );
	void DisplayChosenSourceData( bool );
	void ChangeCurrentStyle( int iSource );
	void ChangeCurrentForm( int iSource );
	void DeassignSNCSLabel( int iSource  );
	void AssignSNCSLabel( int iSource, bncs_string ssNewLabel  );
	void SetUpForIndex( int iIndex );
	void PopulateAssignPanel( int iSource );

public:
	SoftNameExternal( bncs_client_callback * parent, const char* path );
	virtual ~SoftNameExternal();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
};


#endif // SoftNameExternal_INCLUDED