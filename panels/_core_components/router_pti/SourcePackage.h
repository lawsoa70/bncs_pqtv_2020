// SourcePackage.h: interface for the SourcePackage class.
//
//////////////////////////////////////////////////////////////////////
#include <bncs_script_helper.h>
#include "DestPackage.h"

#if !defined(AFX_SOURCEPACKAGE1_H__235DF789_66E0_41A3_BFD9_DE1E1330E426__INCLUDED_)
#define AFX_SOURCEPACKAGE1_H__235DF789_66E0_41A3_BFD9_DE1E1330E426__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SourcePackage  
{
public:
	bncs_stringlist slVirtual;
	bncs_stringlist slDirectly;
	void AssociatedDestPackage(bncs_string s);
	SourcePackage();
	virtual ~SourcePackage();
	bncs_string Name;
	bncs_string Title;
	bncs_string Usage;
	int Index;
	int SDIIndex;
	bool bAssociatedPackagePolled;


};

#endif // !defined(AFX_SOURCEPACKAGE1_H__235DF789_66E0_41A3_BFD9_DE1E1330E426__INCLUDED_)
