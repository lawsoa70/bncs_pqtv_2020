/********************************************/
/* Configuration Editor Plugin              */
/* Written by David Yates                   */
/* Copyright Siemens Business Services 2006 */
/********************************************/
#if !defined(AFX_launchAccessor_H__F681FD3F_7C55_4B52_A413_A57320B80D3C__INCLUDED_)
#define AFX_launchAccessor_H__F681FD3F_7C55_4B52_A413_A57320B80D3C__INCLUDED_

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

class DO_PSACC_EXPORT launchItem
{
public:
	launchItem( const QString & application=QString::null, const QString &arguments=QString::null, const QString & workingdir=QString::null, bool restartOnCrash = false, int delay = 0 )
	{
		m_application = application;
		m_arguments = arguments;
		m_workingdir = workingdir;
		m_restartOnCrash = restartOnCrash;
		m_delay = delay;
	};
	QString m_application;
	QString m_workingdir;
	QString m_arguments;
	bool m_restartOnCrash;
	int m_delay;
};

typedef QValueList< launchItem > launchItemList;


class DO_PSACC_EXPORT launchAccessor : public QObject
{
	Q_OBJECT
public:
	launchAccessor();
	virtual ~launchAccessor();
private:
	static launchAccessor* launchAcc;
	static int loadLock;
	QDomDocument *doc;
	QDomNode *childNode;
	bool setAttribute( const QString & key, const QString & attr, const QString &value );
	QString attribute( const QString & key, const QString & attr );
	QMutex mutex;
	bool m_undoStep;
	void setDirty( void );
public:
	static launchAccessor* load( void )
	{
		loadLock++;
		if( !launchAcc )
			launchAcc = new launchAccessor();
		return launchAcc;
	};
	static bool unload( void )
	{
		loadLock--;
		if( !loadLock )
		{
			delete launchAcc;
			launchAcc= 0;
		}
		return launchAcc;
	};
	bool isDirty( void );
	void lock( void );
	void unlock( void );
	double version( void );
	void setVersion( double );

	bool item( const QString & ws, QString & csi, bool & roc, launchItemList & list, int & pos  );

	bool insertItem( const QString & ws, const launchItem & item, int pos = -1);
	bool deleteItem( const QString & ws, const launchItem & item, int pos = -1 );

	bool getLaunchItem( const QString & ws, int pos, launchItem & item );

	bool promoteItem( const QString & ws, int pos);
	bool demoteItem( const QString & ws, int pos, bool direction = true );

	bool addWorkstation( const QString & ws, const QString & csi = "v4csi", bool roc = true, const launchItemList & list = launchItemList() );
	bool deleteWorkstation( const QString & ws );

	bool renamelaunch( const QString & launchOld, const QString & launchNew );

	QString csi( const QString & key );
	bool roc( const QString & key );
	bool setCSI( const QString & key, const QString & csi);
	bool setROC( const QString & key, bool roc );

	bool setCurrent( const QString & launch );
	bool setFirst( void );

	int itemPos( const QString & launch, const launchItem & item );
	bool exists( const QString & launch );
	
	void setUndo( bool );

	bool setChildAttribute( const QString & key, int pos, const QString & attr, const QString &value );
	QString childAttribute( const QString & key, int row, const QString & attr );
	QString application( const QString & key, int row );
	bool setApplication( const QString & key, int row, const QString & application );
	QString workingdir( const QString & key, int row );
	bool setWorkingdir( const QString & key, int row, const QString & workingdir );
	QString arguments( const QString & key, int row );
	bool setArguments( const QString & key, int row, const QString & arguments );
	int delay( const QString & key, int row );
	bool setDelay( const QString & key, int row, int delay);
	bool roc( const QString & key, int row );
	bool setRoc( const QString & key, int row, bool roc );
signals:
	void launchAdded( const QString & ws, const QString & csi, bool roc, const launchItemList & list );
	void launchRenamed( const QString & wsOld, const QString & wsNew );
	void launchDeleted( const QString & ws, const QString & csi, bool roc, const launchItemList & list );
	void itemAdded( const QString & ws, const launchItem & item, int pos );
	void itemDeleted( const QString & ws, const launchItem & item, int pos );
	void itemPromoted( const QString & ws, int pos );
	void itemDemoted( const QString & ws, int pos );
	void dirty( bool );
	void csiChanged( const QString & key, const QString & csi);
	void rocChanged( const QString & key, bool roc );
	void applicationChanged( const QString & key, int row, const QString & application );
	void workingdirChanged( const QString & key, int row, const QString & workingdir);
	void argumentsChanged( const QString & key, int row, const QString & arguments);
	void delayChanged( const QString & key, int row, int delay);
};

#endif // !defined(AFX_launchAccessor_H__F681FD3F_7C55_4B52_A413_A57320B80D3C__INCLUDED_)
