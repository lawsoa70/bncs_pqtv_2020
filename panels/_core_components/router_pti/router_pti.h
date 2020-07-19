#ifndef router_pti_INCLUDED
	#define router_pti_INCLUDED

#include <bncs_script_helper.h>
#include <map>
#include <vector>
#include "DestIndex.h"
#include "SourceIndex.h"


#ifdef WIN32
	#ifdef DOEXPORT_SCRIPT
		#define EXPORT_SCRIPT __declspec(dllexport) 
	#else
		#define EXPORT_SCRIPT __declspec(dllimport) 
	#endif
#else 
	#define EXPORT_SCRIPT
#endif
 
class router_pti : public bncs_script_helper
{
public:
	void updatePTIPackage();
	router_pti( bncs_client_callback * parent, const char* path );
	virtual ~router_pti();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
private:
	void pollRelevantIndexes();
	bool m_bSettingsLoaded;
	SourcePackage* m_spPackageSource_Selected;
	void showPackageRouting(int iSourcePackage = -1);
	bncs_string m_instance_PackagePtiDevice;
	bncs_string m_instance_PackageRtrDevice;
	int m_intPackagePtiDevice;
	inline int getSDIIndex(bncs_string s);

	void init();
	void updatePTI(DestIndex*, int oldIndex);
	void showPTI();
	int m_intCurrentSource;
	bool m_bPackager;
	int m_intDevice;

	int m_intPackageRtrDevice;

	vector<SourceIndex*> m_vSourceIndex;
	vector<DestIndex*> m_vDestIndex;

	vector<SourcePackage*> m_vSourcePackages;
	vector<DestPackage*> m_vDestPackages;

};


#endif // router_pti_INCLUDED