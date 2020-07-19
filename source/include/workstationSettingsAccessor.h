/********************************************/
/* Configuration Editor Plugin              */
/* Written by David Yates                   */
/* Copyright Siemens Business Services 2006 */
/********************************************/
#if !defined(AFX_workstationSettingsAccessor_H__F681FD3F_7C55_4B52_A413_A57320B80D3C__INCLUDED_)
#define AFX_workstationSettingsAccessor_H__F681FD3F_7C55_4B52_A413_A57320B80D3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef BNCS_CONFIG_EXPORTS
	#define DO_PSACC_EXPORT __declspec(dllexport) 
#else 
	#define DO_PSACC_EXPORT __declspec(dllimport) 
#endif


#include <qobject.h>
#include <qmutex.h>

class QDomDocument;
class QDomNode;


class DO_PSACC_EXPORT workstationSettingsItem
{
public:
	workstationSettingsItem( const QString & id=QString::null, const QString &value=QString::null )
	{
		m_id = id;
		m_value = value;
	};
	QString m_id;
	QString m_value;
};

typedef QValueList< workstationSettingsItem > workstationSettingsItemList;

class DO_PSACC_EXPORT workstationSettingsAccessor : public QObject
{
	Q_OBJECT
public:
	workstationSettingsAccessor();
	virtual ~workstationSettingsAccessor();
private:
	static workstationSettingsAccessor* workstationSettingsAcc;
	static int loadLock;
	QDomDocument *doc;
	QDomNode *childNode;
	bool setAttribute( const QString & attr, const QString & key, const QString &value );
	QString attribute( const QString & attr, const QString & key );
	QMutex mutex;
	bool m_undoStep;
	void setDirty( void );
public:
	static workstationSettingsAccessor* load( void )
	{
		loadLock++;
		if( !workstationSettingsAcc )
			workstationSettingsAcc = new workstationSettingsAccessor();
		return workstationSettingsAcc;
	};
	static bool unload( void )
	{
		loadLock--;
		if( !loadLock )
		{
			delete workstationSettingsAcc;
			workstationSettingsAcc = 0;
		}
		return workstationSettingsAcc;
	};
	bool isDirty( void );
	void lock( void );
	void unlock( void );
	double version( void );
	void setVersion( double );

	bool item( const QString & key, workstationSettingsItemList & list );
	bool nextItem( QString & key, workstationSettingsItemList & list  );
	
	bool insertItem( const QString & ws, const workstationSettingsItem & item );
	bool deleteItem( const QString & ws, const workstationSettingsItem & item );

	bool addWorkstation( const QString & ws, const workstationSettingsItemList & list = workstationSettingsItemList() );
	bool deleteWorkstation( const QString & ws );

	bool renameWorkstation( const QString & wsOld, const QString & wsNew );


	QString value( const QString & ws, const QString & param );
	bool setValue( const QString & ws, const QString & param, const QString & value );

	bool setCurrent( const QString & ws );
	bool setFirst( void );
	bool exists( const QString & ws );
	bool exists( const QString & ws, const QString & param );
	void setUndo( bool );
signals:
	void workstationAdded( const QString & ws, const workstationSettingsItemList & list );
	void workstationRenamed( const QString & wsOld, const QString & wsNew );
	void workstationDeleted( const QString & ws, const workstationSettingsItemList & list );
	void itemAdded( const QString & ws, const workstationSettingsItem & item);
	void itemDeleted( const QString & ws, const workstationSettingsItem & item);
	void dirty( bool );
};

#endif // !defined(AFX_workstationSettingsAccessor_H__F681FD3F_7C55_4B52_A413_A57320B80D3C__INCLUDED_)
