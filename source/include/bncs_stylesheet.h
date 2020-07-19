
#include <qmap.h>
#include <qstringlist.h>
#include <qdom.h>

#ifndef STYLESHEET_INCLUDED
	#define STYLESHEET_INCLUDED

class QWidget;

#ifdef STYLESHEET_EXPORTS
	#define DOEXPORT __declspec(dllexport) 
#else 
	#define DOEXPORT __declspec(dllimport) 
#endif


class DOEXPORT styleentry
{
public:
	enum styletype
	{
		state = 1,
		style,
		setting 
	};
	QStringList settinglist;
	QString suitablefor;
	QString description;
	enum styletype type;
};

typedef QMap< QString, styleentry > stylemap ;

#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
template class __declspec(dllexport) QMap< QString, styleentry >;
// MOC_SKIP_END
#endif


class DOEXPORT bncs_stylesheet 
{
public:
	bncs_stylesheet();
	~bncs_stylesheet();
	void get( QString & style, QWidget *w );
	static void getStyle( QString & style, QWidget *qW );
	const stylemap stylelist( void );
	const QStringList settingslist( QString & style );

	static bool load( void );
	static int count( void );
	static bool reLoad( void );

private:
	static QMap< QString, styleentry> styles;
//	static int lock;

	// Added by PL to allow stylesheet editor to work 14/02/07
	static QString m_sFilename;

public:
	// Added by PL to allow stylesheet editor to work 14/02/07
	void set( QString & sStyle, styleentry &stEntry );// Add if not in existence else update..
	void deleteStyle( QString sStyle );
	styleentry *getStyleEntry( QString sName ); 
	bool save( void );
	bool styleExist( QString sStyle );

};

#endif
