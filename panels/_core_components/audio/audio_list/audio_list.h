#ifndef audio_list_INCLUDED
	#define audio_list_INCLUDED

#include <bncs_script_helper.h>
#include "databaseMgr.h"

#ifdef WIN32
	#ifdef DOEXPORT_SCRIPT
		#define EXPORT_SCRIPT __declspec(dllexport) 
	#else
		#define EXPORT_SCRIPT __declspec(dllimport) 
	#endif
#else 
	#define EXPORT_SCRIPT
#endif
 
class audio_list : public bncs_script_helper
{
public:
	audio_list( bncs_client_callback * parent, const char* path );
	virtual ~audio_list();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
private:
	bncs_string m_sSilence_Text;
	bncs_string m_sSilence_Index;
	void parkSelected();
	bool m_bAreaSelected;
	bncs_string showIndex(int);
	void loadNames( bool maintainSelectedHighlight );
	bncs_string getAreaString(int iRtrIndex);
	int m_intIndxStart;
	int	m_intIndxEnd;
	databaseMgr dbm;
	bncs_string m_sInstance;
	int m_intDevice;
	int m_intDBIndex;
	int m_intDBIndexNames;
	int m_intDBAreas;
	int m_intDBAudioLeg;

	int m_iIndexNo;
	bool m_blnShowUnusedPorts;
	bool m_blnAlphaSort;
	bool m_bAllNameSpacesListed;
	bncs_stringlist m_slListedNameSpaces;
	bncs_stringlist m_slSelectableLegs;
	void setListboxSelectedItem(bncs_string strName);
	void selectArea(int );
	bncs_string m_sSelectedNamespace;
	int getRouterFromArea(bncs_string);
	bncs_stringlist m_slAllNamespaces;

	int m_iSelectedNamespaceButton;
	void highlightButton(int);

	bncs_stringlist m_slMinAndMaxRouterIndexs;
	bncs_stringlist getStartAndDestForArea(int iarea);

};


#endif // audio_list_INCLUDED