/********************************************/
/* Configuration Editor Plugin              */
/* Written by David Yates                   */
/* Copyright Siemens Business Services 2006 */
/********************************************/
#if !defined(AFX_APPLICATIONSACCESSOR_H__F681FD3F_7C55_4B52_A413_A57320B80D3C__INCLUDED_)
#define AFX_APPLICATIONSACCESSOR_H__F681FD3F_7C55_4B52_A413_A57320B80D3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef BNCS_CONFIG_EXPORTS
	#define DO_APPACC_EXPORT __declspec(dllexport) 
#else 
	#define DO_APPACC_EXPORT __declspec(dllimport) 
#endif


#include <qobject.h>
#include <qmutex.h>

class QDomDocument;
class QDomNode;

class DO_APPACC_EXPORT applicationsAccessor : public QObject
{
	Q_OBJECT
public:
	applicationsAccessor();
	virtual ~applicationsAccessor();
private:
	static applicationsAccessor* applicationsAcc;
	static int loadLock;
	QDomDocument *doc;
	QDomNode *childNode;
	bool setAttribute( const QString & attr, const QString & key, const QString &value );
	QString attribute( const QString & attr, const QString & key );
	QMutex mutex;
	void setDirty( void );
	bool m_undoStep;

public:
	static applicationsAccessor* load( void )
	{
		loadLock++;
		if( !applicationsAcc )
			applicationsAcc = new applicationsAccessor();
		return applicationsAcc;
	};
	static bool unload( void )
	{
		loadLock--;
		if( !loadLock )
		{
			delete applicationsAcc;
			applicationsAcc=0;
		}
		return applicationsAcc;
	};
	void lock( void );
	void unlock( void );
	double version( void );
	void setVersion( double );
	QString type( const QString & key = QString::null );
	QString caption( const QString & key = QString::null );
	QString title( const QString & key = QString::null );
	QString icon( const QString & key = QString::null );
	bool setCaption( const QString & key, const QString & caption );
	bool setTitle( const QString & key, const QString & title );
	bool setType( const QString & key, const QString & type );
	bool setIcon( const QString & key, const QString & icon );
	bool addItem( const QString & key, const QString & type, const QString & caption, const QString & title, const QString & icon );
	bool deleteItem( const QString & key );
	bool item( const QString & key, QString & type, QString & caption, QString & title, QString & icon );
	bool nextItem( QString & key, QString & type, QString & caption, QString & title, QString & icon );
	bool rename( const QString & oldname, const QString newname );
	bool setCurrent( const QString & key );
	bool setFirst( void );
	bool exists( const QString & application );
	bool isDirty( void );
	void setUndo( bool );
signals:
	void captionChanged( const QString & key, const QString & caption );
	void titleChanged( const QString & key, const QString & title );
	void typeChanged( const QString & key, const QString & type );
	void iconChanged( const QString & key, const QString & icon );
	void itemAdded( const QString & key, const QString & type, const QString & caption, const QString & title, const QString & icon );
	void itemDeleted( const QString & key );
	void itemRenamed( const QString & oldname, const QString & newname );
	void dirty( bool );
};

#endif // !defined(AFX_APPLICATIONSACCESSOR_H__F681FD3F_7C55_4B52_A413_A57320B80D3C__INCLUDED_)
