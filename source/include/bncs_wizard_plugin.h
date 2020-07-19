#include <qpixmap.h>
#include <qdialog.h>
#include <qstring.h>
// bncs_wizard_plugin.h: interface for the bncs_wizard_plugin class.
//
//////////////////////////////////////////////////////////////////////
 
#if !defined(AFX_BNCS_WIZARD_PLUGIN_H__98669D44_65B3_4A71_B38A_4C015BF5339A__INCLUDED_)
#define AFX_BNCS_WIZARD_PLUGIN_H__98669D44_65B3_4A71_B38A_4C015BF5339A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class bncs_wizard_plugin  
{
public:
	bncs_wizard_plugin(){};

	virtual QPixmap icon( void ) = 0;
	virtual QString description( void ) = 0; 
	virtual void go( const QString & path, const QString & name, const QStringList & args ) = 0;
	virtual QStringList fileList( void ){return QStringList();}; 
};

#define EXPORT __declspec( dllexport )

extern "C"
{
	 EXPORT bncs_wizard_plugin *load_bncs_wizard_plugin( void );
	 EXPORT void free_bncs_wizard_plugin( bncs_wizard_plugin *a ){ delete a; }
}

#define BNCS_WIZARD_PLUGIN( a ) EXPORT bncs_wizard_plugin *   load_bncs_wizard_plugin(){ return new a; }



#endif // !defined(AFX_BNCS_WIZARD_PLUGIN_H__98669D44_65B3_4A71_B38A_4C015BF5339A__INCLUDED_)




