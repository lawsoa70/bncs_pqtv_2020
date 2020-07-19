/********************************************/
/* Configuration Editor Plugin              */
/* Written by David Yates                   */
/* Copyright Siemens Business Services 2006 */
/********************************************/
#if !defined(AFX_panelsetsAccessor_H__F681FD3F_7C55_4B52_A413_A57320B80D3C__INCLUDED_)
#define AFX_panelsetsAccessor_H__F681FD3F_7C55_4B52_A413_A57320B80D3C__INCLUDED_

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


class DO_PSACC_EXPORT panelsetItem
{
public:
	panelsetItem( const QString & id=QString::null, const QString &itemClass=QString::null, const QString & param=QString::null )
	{
		m_id = id;
		m_itemClass = itemClass;
		m_param = param;
	};
	QString m_id;
	QString m_itemClass;
	QString m_param;
};

typedef QValueList< panelsetItem > panelsetItemList;

class DO_PSACC_EXPORT panelsetsAccessor : public QObject
{
	Q_OBJECT
public:
	panelsetsAccessor();
	virtual ~panelsetsAccessor();
private:
	static panelsetsAccessor* panelsetsAcc;
	static int loadLock;
	QDomDocument *doc;
	QDomNode *childNode;
	bool setAttribute( const QString & attr, const QString & key, const QString &value );
	QString attribute( const QString & attr, const QString & key );
	QMutex mutex;
	bool m_undoStep;
	void setDirty( void );
public:
	static panelsetsAccessor* load( void )
	{
		loadLock++;
		if( !panelsetsAcc )
			panelsetsAcc = new panelsetsAccessor();
		return panelsetsAcc;
	};
	static bool unload( void )
	{
		loadLock--;
		if( !loadLock )
		{
			delete panelsetsAcc;
			panelsetsAcc = 0;
		}
		return panelsetsAcc;
	};
	bool isDirty( void );
	void lock( void );
	void unlock( void );
	double version( void );
	void setVersion( double );

	bool item( const QString & key, QString & level, panelsetItemList & list, int & pos  );
	bool nextItem( QString & key, QString & level, panelsetItemList & list  );
	
	bool insertItem( const QString & panelset, const panelsetItem & item, int position = -1);
	bool deleteItem( const QString & panelset, const panelsetItem & item, int pos = -1 );

	bool promoteItem( const QString & panelset, const panelsetItem & item, int pos);
	bool demoteItem( const QString & panelset, const panelsetItem & item, int pos, bool direction = true );

	bool addPanelset( const QString & panelset, const QString & level = "user", const panelsetItemList & list = panelsetItemList() );
	bool deletePanelset( const QString & panelset );

	bool renamePanelset( const QString & panelsetOld, const QString & panelsetNew );

	bool setCurrent( const QString & panelset );
	bool setFirst( void );
	int itemPos( const QString & panelset, const panelsetItem & item );
	bool exists( const QString & panelset );
	void setUndo( bool );
signals:
	void panelsetAdded( const QString & panelset, const QString & level, const panelsetItemList & list );
	void panelsetRenamed( const QString & panelsetOld, const QString & panelsetNew );
	void panelsetDeleted( const QString & panelset, const QString & level, const panelsetItemList & list );
	void itemAdded( const QString & panelset, const panelsetItem & item, int position );
	void itemDeleted( const QString & panelset, const panelsetItem & item, int pos );
	void itemPromoted( const QString & panelset, const panelsetItem & item, int pos );
	void itemDemoted( const QString & panelset, const panelsetItem & item, int pos );
	void dirty( bool );
};

#endif // !defined(AFX_panelsetsAccessor_H__F681FD3F_7C55_4B52_A413_A57320B80D3C__INCLUDED_)
