// bncs_instance_lookup.h: interface for the bncs_instance_lookup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BNCS_INSTANCE_LOOKUP_H__08D36E82_0B42_4C7A_AD66_EAB89EF84F44__INCLUDED_)
#define AFX_BNCS_INSTANCE_LOOKUP_H__08D36E82_0B42_4C7A_AD66_EAB89EF84F44__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qmap.h>
#include "instanceInfo.h"
#include "deviceTypeInfo.h"
#include "complexType.h"
#include <QStringList.h>

#ifdef STYLESHEET_EXPORTS
	#define DOEXPORT __declspec(dllexport) 
#else 
	#define DOEXPORT __declspec(dllimport) 
#endif

#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
template class __declspec(dllexport) QMap< QString, instanceInfo>;
template class __declspec(dllexport) QMap< QString, deviceTypeInfo>;
template class __declspec(dllexport) QMap< QString, complexType>;
// MOC_SKIP_END
#endif
 

class DOEXPORT bncs_instance_lookup  
{
public:
	bncs_instance_lookup();
	virtual ~bncs_instance_lookup();
	static bool lookup( const QString & instance, QString & altId, int * device, int * offset, char *drivertype );
	static bool lookup( const QString & instance, const QString & param, QString & altId, int * device, int * offset, int * slot, char *drivertype );
	static bool lookupComplex( const QString & complex, const QString & instance, const QString & param, QString & altId, int *device, int *offset, int *slot, char *drivertype );

	static bool lookup( const QString & instance, QString & altId, QString & location, QString & type, QString & ref, bool & composite );

	static bool lookupComposite( const QString & instance, const QString & group, QString & groupInstance );
	static bool lookupComposite( const QString & instance, bool & isComposite, QMap< QString, QString > & groupList );
	
	static bool lookupType( const QString & instance, QString & typeReturn );
	static bool lookupInstanceOfType( const QString & type, QStringList & ret );

	static bool loadInstances( void );
	static bool loadDeviceTypes( void );
	static bool loadComplexTypes( void );

	static QMap< QString, instanceInfo> instances;
	static QMap< QString, deviceTypeInfo> deviceTypes;
	static QMap< QString, complexType > complexTypes;
	static void dump( void );
	static int countInstances( void );
	static int countDeviceTypes( void );
	static int countComplexTypes( void );
	static bool loadDeviceParams( const QString & name, deviceTypeInfo & i);
	static bool loadComplexType( const QString & name, complexType & i);

	static bool getInstanceInfo( const QString & instance, instanceInfo ** i );
};

#endif // !defined(AFX_BNCS_INSTANCE_LOOKUP_H__08D36E82_0B42_4C7A_AD66_EAB89EF84F44__INCLUDED_)
