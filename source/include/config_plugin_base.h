#include <qguardedptr.h>
#include <qwidget.h>
#include "toolbarlist.h"
#include "popupmenulist.h"
#include <config_plugin_response.h>

#if !defined(AFX_PLUGIN_BASE_H__DDFA129E_659B_49A6_94DC_14810EDA5422__INCLUDED_)
#define AFX_PLUGIN_BASE_H__DDFA129E_659B_49A6_94DC_14810EDA5422__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// abstract base class for all plugin editors
class config_plugin_base : public QWidget
{
//	Q_OBJECT
public:
	config_plugin_base( const QString & path, const QString & inkey, const QString &inuserLevel )
	{
		filepath = path;
		key = inkey;
		userLevel = inuserLevel;
		setName( key );
	};
	virtual ~config_plugin_base(){};
	virtual QDockWindowList toolbars( void )
	{
		return QDockWindowList();
	};
	virtual QPopupMenuList menus( void )
	{
		return QPopupMenuList();
	};

	virtual bool validate( void )
	{
		return 0;
	};
	virtual bool isDirty( void )
	{
		return false;
	};
	virtual void activate( bool me ){};
	virtual void navigate( const QString & key, const QString & context ){};
	virtual void reply( response * r ){};
	virtual void closeEvent( QCloseEvent *ce )
	{
		ce->accept();
	};
	virtual void apply( void )
	{
	};

	QString getKey( void ) 
	{
		return key;
	};
	virtual QString about( void ) 
	{
		return "This is the default \"About\" text for \"" 
					+ key 
					+ "\" - the author should have defined some \"About\" information here";
	};
protected:
	QString filepath;
	QString key;
	QString userLevel;
};

// abstract base class for describing plugin editors
class config_plugin_desc_base
{
public:
	config_plugin_desc_base(){};
	virtual ~config_plugin_desc_base(){};
	virtual QStringList keys( const QString &  path, const QString & userLevel ) = 0;
	virtual QPixmap pixmap( const QString & key ) = 0;
	virtual config_plugin_base* create( const QString & path, const QString & key, const QString & userLevel ) = 0;
};


typedef config_plugin_desc_base* (*load_desc)() ;

#define EXPORT __declspec( dllexport )

extern "C"
{
	 EXPORT config_plugin_desc_base* load_bncs_config_plugin_desc( void );
}

#define BNCS_CONFIG_PLUGIN_DESC( a ) EXPORT config_plugin_desc_base * load_bncs_config_plugin_desc()\
									{\
										return new a; \
									}

#endif // !defined(AFX_PLUGIN_BASE_H__DDFA129E_659B_49A6_94DC_14810EDA5422__INCLUDED_)
