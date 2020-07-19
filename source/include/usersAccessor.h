/********************************************/
/* Configuration Editor Plugin              */
/* Written by David Yates                   */
/* Copyright Siemens Business Services 2006 */
/********************************************/
#if !defined(AFX_usersAccessor_H__F681FD3F_7C55_4B52_A413_A57320B80D3C__INCLUDED_)
#define AFX_usersAccessor_H__F681FD3F_7C55_4B52_A413_A57320B80D3C__INCLUDED_

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

class DO_APPACC_EXPORT usersAccessor : public QObject
{
	Q_OBJECT
public:
	usersAccessor();
	virtual ~usersAccessor();
private:
	static usersAccessor* usersAcc;
	static int loadLock;
	QDomDocument *doc;
	QDomNode *childNode;
	bool setAttribute( const QString & attr, const QString & key, const QString &value );
	QString attribute( const QString & attr, const QString & key );
	QMutex mutex;
	void setDirty( void );
	bool m_undoStep;

public:
	static usersAccessor* load( void )
	{
		loadLock++;
		if( !usersAcc )
			usersAcc = new usersAccessor();
		return usersAcc;
	};
	static bool unload( void )
	{
		loadLock--;
		if( !loadLock )
		{
			delete usersAcc;
			usersAcc=0;
		}
		return usersAcc;
	};
	void lock( void );
	void unlock( void );
	double version( void );
	void setVersion( double );


// getters and setters
	QString panelset( const QString & key = QString::null );
	QString engset( const QString & key = QString::null );
	bool setPanelset( const QString & key, const QString & panelset );
	bool setEngset( const QString & key, const QString & engset );

// addition and deletion
	bool addItem( const QString & key, const QString & name, const QString & user );
	bool deleteItem( const QString & key );
	bool rename( const QString & oldname, const QString newname );

// iteration
	bool item( const QString & key, QString & name, QString & user );
	bool nextItem( QString & key, QString & name, QString & user );
	bool setCurrent( const QString & key );
	bool setFirst( void );
	
	bool exists( const QString & workstation );

	bool isDirty( void );
	void setUndo( bool );
signals:
	void panelsetChanged( const QString & key, const QString & panelset );
	void engsetChanged( const QString & key, const QString & engset );
	void itemAdded( const QString & key, const QString & name, const QString & user );
	void itemDeleted( const QString & key );
	void itemRenamed( const QString & oldname, const QString & newname );
	void dirty( bool );
};

#endif // !defined(AFX_usersAccessor_H__F681FD3F_7C55_4B52_A413_A57320B80D3C__INCLUDED_)
