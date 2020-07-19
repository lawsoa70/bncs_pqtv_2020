// SourcePackage1.cpp: implementation of the SourcePackage class.
//
//////////////////////////////////////////////////////////////////////

#include "SourcePackage.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SourcePackage::SourcePackage()
{
	Index = 0;
	Name = "";
	SDIIndex = 0;
	slDirectly.clear();
	slVirtual.clear();
	bAssociatedPackagePolled = false;
	Usage = "0/0";
	Title = "";
}

SourcePackage::~SourcePackage()
{

}


void SourcePackage::AssociatedDestPackage(bncs_string s)
{
	bAssociatedPackagePolled = true;
	if(s.length() > 0)
	{
		bncs_string sDirectly, sVirtual;
		s.split('|', sDirectly, sVirtual);

		slDirectly = bncs_stringlist(sDirectly,',');
		slVirtual = bncs_stringlist(sVirtual,',');
	}
	else
	{
		slDirectly.clear();
		slVirtual.clear();
	}
	Usage = bncs_string("%1/%2").arg(slDirectly.count()).arg(slVirtual.count());
}
