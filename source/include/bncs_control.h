#if !defined(AFX_BNCS_CONTROL_H__63762D33_B320_48E6_A067_C9BA90D5AF7F__INCLUDED_)
#define AFX_BNCS_CONTROL_H__63762D33_B320_48E6_A067_C9BA90D5AF7F__INCLUDED_

#include <qwidgetplugin.h>
#include <qwidget.h>
#include <qcolor.h>
#include <qpixmap.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class bncs_popup;
class bncs_control_funcs;

// this ensures that the control is exported for the dll but imported when used
//  called directly (i.e. not using QWidgetFactory)
/*
#ifndef BNCS_CONTROL_DLL_MAIN
	#undef QT_WIDGET_PLUGIN_EXPORT
	#define QT_WIDGET_PLUGIN_EXPORT
#endif
*/
#include "notifyCallbackClass.h"

class QT_WIDGET_PLUGIN_EXPORT bncs_control : public QWidget, public notifyCallbackClass
{
	Q_OBJECT
	// this ensures that the property editor for this class is able to access what is 
	//  otherwise private member variables but protects these variables from everyone else
	friend class bncs_control_edit;

public:
	bncs_control( QWidget * parent = 0, const char * name = 0, WFlags f = 0 );
	virtual ~bncs_control();
	void setCaption( const QString & );

	// these are for the property editors that things like 
	//  the database browser components need (reference Mark Baldry)
	void setText( const QString &intext );
	QString gettext( void ) const;
	void setEnabled( bool enable );
    bool isEnabled( void ) const;
//	int getLabelStyle( void ){ return labelstyle; };
	void notifyCallback( const QString & , const QString & );
private:
	void paintEvent( QPaintEvent * );
	void resizeEvent(  QResizeEvent * );
	void mousePressEvent ( QMouseEvent * e );
	void mouseReleaseEvent ( QMouseEvent * e );
	bncs_control_funcs *control;
signals:
	void notify( QString & );
public slots:
	void edit( bool * changed, bool * suppressInsertEventFilter );
};

#endif // !defined(AFX_BNCS_CONTROL_H__63762D33_B320_48E6_A067_C9BA90D5AF7F__INCLUDED_)
