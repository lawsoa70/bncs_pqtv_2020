// complexType.h: interface for the complexType class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPLEXTYPE_H__7CC0ECC7_AD6D_4E82_A0FE_69D36A9FCECB__INCLUDED_)
#define AFX_COMPLEXTYPE_H__7CC0ECC7_AD6D_4E82_A0FE_69D36A9FCECB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <QString.h>
#include <qmap.h>
#include "complexInstance.h"

#ifdef STYLESHEET_EXPORTS
	#define DOEXPORT __declspec(dllexport) 
#else 
	#define DOEXPORT __declspec(dllimport) 
#endif

#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
template class __declspec(dllexport) QMap< QString, complexInstance>;
// MOC_SKIP_END
#endif

class DOEXPORT complexType  
{
public:
	complexType();
	virtual ~complexType();

	QMap< QString, complexInstance > complexInstances;
};

#endif // !defined(AFX_COMPLEXTYPE_H__7CC0ECC7_AD6D_4E82_A0FE_69D36A9FCECB__INCLUDED_)
