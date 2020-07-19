#include <qstring.h>
#include <qdatetime.h>
#include <qptrlist.h>

#if !defined(AFX_FILELIST_H__64BCCEDF_C1EE_457D_A600_BA59D877C18E__INCLUDED_)
#define AFX_FILELIST_H__64BCCEDF_C1EE_457D_A600_BA59D877C18E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class file
{
public:
	file( const QString & inname, const QDateTime & indate, long insize )
	{
//#pragma message ("Message for Dave: upper() call required for Win32 only")
		name = inname;//.upper();
		date = indate;
		size = insize;
	};
	QString name;
	QDateTime date;
	long size;
};

class filelist  
{
public:
	filelist();
	void go( const QString & root, const QString & path="" );
	QPtrList< file > list;
	QPtrList< file > additions;
	QPtrList< file > changes;
	QPtrList< file > deletions;
	QPtrList< file > identical;

	void updateSingleFile( const QString & root, const QString & filename );
	void compare( filelist &, bool forgetIdentical = false );
	bool saveToFile( const QString & filename );
	void loadFromFile( const QString & filename );
	void load( const QString & in );
	void clear( void );
	void add( file * );
	void saveToStream( QTextStream & ts, bool correct = false );
	void saveXMLToStream( QTextStream & ts);
	bool isValid( void );
	void setExclusions( const QStringList & exc );
	QString & correct( QString & in );
	void setValid( void );
	unsigned int count( void );
private:
	QString root;
	bool valid;
	QStringList m_exclusions;
	bool m_forgetIdentical;
};

#endif // !defined(AFX_FILELIST_H__64BCCEDF_C1EE_457D_A600_BA59D877C18E__INCLUDED_)
 