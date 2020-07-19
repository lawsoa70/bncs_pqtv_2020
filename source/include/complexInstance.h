// complexInstance.h: interface for the complexInstance class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPLEXINSTANCE_H__6E523284_9DE9_4697_A220_0C3D3828C2A5__INCLUDED_)
#define AFX_COMPLEXINSTANCE_H__6E523284_9DE9_4697_A220_0C3D3828C2A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "complexParamInfo.h"
#include <qmap.h>

#ifdef STYLESHEET_EXPORTS
	#define DOEXPORT __declspec(dllexport) 
#else 
	#define DOEXPORT __declspec(dllimport) 
#endif

#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
template class __declspec(dllexport) QMap< QString, complexParam>;
// MOC_SKIP_END
#endif

class DOEXPORT complexInstance  
{
public:
	complexInstance();
	virtual ~complexInstance();

	QMap< QString, complexParam > complexParams;
};

#endif // !defined(AFX_COMPLEXINSTANCE_H__6E523284_9DE9_4697_A220_0C3D3828C2A5__INCLUDED_)
