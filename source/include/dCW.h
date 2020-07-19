/*************************************/
/* Original Version by David Yates   */
/* Copyright Siemens IT Systems 2007 */
/*************************************/

// dCW.h: interface for the dCW class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DCW_H__1234EA8B_B6D8_4EA1_8B19_9B3833D86B87__INCLUDED_)
#define AFX_DCW_H__1234EA8B_B6D8_4EA1_8B19_9B3833D86B87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qwidget.h>
#include <qdom.h>
#include <qfile.h>
#include <qwidgetlist.h>
#include <qdatetime.h>
#include <signalConnection.h>
#include <list>
using namespace std;


#ifdef DLL_VERSION
	#define DO_EXPORT __declspec(dllexport) 
#else
	#define DO_EXPORT
#endif

class  DO_EXPORT dCW  
{
public:
	dCW();
	QWidget * create (	const QString & uiFile, 
								QObject * connector = 0, 
								QWidget * parent = 0, 
								const char * name = 0,
								bool fCreateTopLevel= true,
								bool fSizeParent = true,
								double deltaW = 1.0,
								double deltaH = 1.0,
								bool isScaleRequired = false,
								bool isScaleFontRequired = false
								);
		
	QWidgetList widgetList( void ){ return m_widgetList;};
	QString bpixmap( void ){ return thisbpixmap;};
	QString bcolour( void ){ return thisbcolour;};
	QString scriptfile( void ){ return thisscriptfile;};
	virtual ~dCW();
	QWidget * parseXML( QDomDocument&, QWidget *, QPoint, bool fCreateTopLevel = true, 
	bool fSizeParent = true, QString path="");
	QWidget *parseFlat( QFile & fd, QWidget * inparent, QPoint offset, bool fCreateTopLevel, bool fSizeParent );
	static bool checkNameUnique( QWidget *w, const QString & name, QString & newname );

	QWidget * createWidget( const QString & widget, QWidget * parent, const QString & name, int x, int y, int w, int h );
	int round(double d);

public:
	QWidgetList m_widgetList;
	QString thisbcolour;
	QString thisbpixmap;
	QString thisscriptfile;
	int dlgpos_x;
	int dlgpos_y;
	bool hascaption;
	bool dialogtype;
	int gridSize;
	int gridEnabled;
	QString m_name;

	QString m_testInstance;
	QString m_panelmanSize;
	bool m_showTestDialog;

public:
	static void clearCache( void );
	static void enableCache( bool enabled=true );

	QWidget * parseXML(		QDomDocument&, 
							QWidget *, QPoint, 
							bool fForceVisible, 
							bool fCreateTopLevel = true, 
							bool fSizeParent = true, 
							QString path="");
private:
	static QStyle *m_pStyle;
	double dDeltaW;
	double dDeltaH;
	bool bIsScaleRequired;
	bool bIsScaleFontRequired;
	QString sFileName; //temp for tests
	signalConnectionRefList m_connectionList;
	static bool m_developerMode;
public:
	signalConnectionRefList connectionList( void );
	QString name( void );
	static void setDesignTime( bool enabled );
	static bool isDesignTime( void );
	static bool developerMode( void );
};

/*#ifdef dCW_main
	QWidgetList dCW::list;
#endif
*/
#endif // !defined(AFX_DCW_H__1234EA8B_B6D8_4EA1_8B19_9B3833D86B87__INCLUDED_)
