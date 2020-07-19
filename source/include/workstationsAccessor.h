/********************************************/
/* Configuration Editor Plugin              */
/* Written by David Yates                   */
/* Copyright Siemens Business Services 2006 */
/********************************************/
#if !defined(AFX_workstationsAccessor_H__F681FD3F_7C55_4B52_A413_A57320B80D3C__INCLUDED_)
#define AFX_workstationsAccessor_H__F681FD3F_7C55_4B52_A413_A57320B80D3C__INCLUDED_

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

class DO_APPACC_EXPORT workstationsAccessor : public QObject
{
	Q_OBJECT
public:
	workstationsAccessor();
	virtual ~workstationsAccessor();
private:
	static workstationsAccessor* workstationsAcc;
	static int loadLock;
	QDomDocument *doc;
	QDomNode *childNode;
	bool setAttribute( const QString & attr, const QString & key, const QString &value );
	QString attribute( const QString & attr, const QString & key );
	QMutex mutex;
	void setDirty( void );
	bool m_undoStep;

public:
	static workstationsAccessor* load( void )
	{
		loadLock++;
		if( !workstationsAcc )
			workstationsAcc = new workstationsAccessor();
		return workstationsAcc;
	};
	static bool unload( void )
	{
		loadLock--;
		if( !loadLock )
		{
			delete workstationsAcc;
			workstationsAcc = 0;
		}
		return workstationsAcc;
	};
	void lock( void );
	void unlock( void );
	double version( void );
	void setVersion( double );


// getters and setters
	QString name( const QString & key = QString::null );
	QString user( const QString & key = QString::null );
	bool setName( const QString & key, const QString & caption );
	bool setUser( const QString & key, const QString & title );

// addition and deletion
	bool addWorkstation( const QString & key, const QString & name, const QString & user );
	bool deleteWorkstation( const QString & key );
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
	void nameChanged( const QString & key, const QString & caption );
	void userChanged( const QString & key, const QString & title );
	void itemAdded( const QString & key, const QString & name, const QString & user );
	void itemDeleted( const QString & key );
	void itemRenamed( const QString & oldname, const QString & newname );
	void dirty( bool );
};

#endif // !defined(AFX_workstationsAccessor_H__F681FD3F_7C55_4B52_A413_A57320B80D3C__INCLUDED_)
