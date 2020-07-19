/*************************************/
/* Original Version by David Yates   */
/* Copyright Atos 2007 */
/*************************************/
#if !defined(AFX_BNCS_CLIENT_RUNTIME_H__DB93E75C_BEFE_4ED0_9D61_525A10E3E45C__INCLUDED_)
#define AFX_BNCS_CLIENT_RUNTIME_H__DB93E75C_BEFE_4ED0_9D61_525A10E3E45C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qwidget.h>
#include <qwidgetlist.h>
#include "signalConnection.h"
#include <ccTimer.h>

#include <list>
using namespace std;

/*! \brief Run time class */
/*! Run time client class */

#ifdef DLL_VERSION 
	#define DO_BNCS_CLIENT_RUNTIME_EXPORT __declspec(dllexport) 
#else
	#define DO_BNCS_CLIENT_RUNTIME_EXPORT __declspec(dllimport) 
#endif


class connectionDebugDlg;

class DO_BNCS_CLIENT_RUNTIME_EXPORT bncs_client_runtime : public QWidget, private IccTimerCallback
{
	Q_OBJECT
public:
	bncs_client_runtime( QWidget * parent = 0, const char * name = 0, WFlags f = 0 );
	virtual ~bncs_client_runtime();
	void contextMenuEvent ( QContextMenuEvent * e );
	void mousePressEvent( QMouseEvent * e ){};
	void mouseReleaseEvent( QMouseEvent * e ){};
	void paintEvent(QPaintEvent *pe );
	void setCaption( const QString & in );
	void setWidgetList( const QWidgetList & list );
	QWidgetList widgetList( void ) const;
	void setFilePath( const QString & f );
	QString filePath( void );
	void addWidgetToList(QWidget*);
	bool removeWidgetFromList(QWidget *);
private: 
	int decoration;
//	void resizeEvent(  QResizeEvent * );
	QWidgetList m_widgetList;
	void connectionTriggerEvent( const QString & event, const QString & source );
signals:
	void notify( QString & );
private:
	QString m_lastInstance;
	signalConnectionRefList m_connectionList;
public slots:
	void notifyIn( QString & in );
public:
	void addConnection( const QString & name, const QString & source, const QString & dest, const QString & sourceEvent, const QString & destEvent );
	void addConnection( QWidget * w );
private:
	connectionDebugDlg *cdb;
	QString m_path;
	QString m_filePath;
private slots:
	void cdbDestroyed( void );
	void doShowIncomingEvent( bool in );
private:
	bool m_showIncomingEvent;
	ccTimer * m_startupTimer;
public:
	const signalConnectionRefList & connectionList( void )
	{
		return m_connectionList;
	};
protected:
	virtual void ccTimerCallback( int timerId );
};

#endif // !defined(AFX_BNCS_CLIENT_RUNTIME_H__DB93E75C_BEFE_4ED0_9D61_525A10E3E45C__INCLUDED_)
