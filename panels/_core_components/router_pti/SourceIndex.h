// SourceIndex.h: interface for the SourceIndex class.
//
//////////////////////////////////////////////////////////////////////
#include "DestIndex.h"
#include <bncs_script_helper.h>
#include <vector>
#include "SourcePackage.h"

#if !defined(AFX_SOURCEINDEX_H__A98BF1E0_4F56_4C25_B8F3_FC3833FBB1DB__INCLUDED_)
#define AFX_SOURCEINDEX_H__A98BF1E0_4F56_4C25_B8F3_FC3833FBB1DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SourceIndex  
{
public:
	vector<SourcePackage*> GetSourcePackages();
	void RemoveSourcePackage(SourcePackage* s);
	void AddSourcePackage(SourcePackage* s);
	int GetUsageCount();
	vector<DestIndex*> GetDestIndexes();
	void RemoveDest(DestIndex* d);
	void AddDest(DestIndex* d);
	SourceIndex();
	virtual ~SourceIndex();
	int Index;
	bncs_string Name;
private:
	vector<DestIndex*> DestsRoutedTo;
	vector<SourcePackage*> SourcePackagesUsedIn; //List of the source Packages the SDI appears in

};

#endif // !defined(AFX_SOURCEINDEX_H__A98BF1E0_4F56_4C25_B8F3_FC3833FBB1DB__INCLUDED_)
