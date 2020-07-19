// SourceIndex.cpp: implementation of the SourceIndex class.
//
//////////////////////////////////////////////////////////////////////

#include "SourceIndex.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SourceIndex::SourceIndex()
{
	Index = 0;
	Name = "Empty";
	DestsRoutedTo.clear();
	SourcePackagesUsedIn.clear();
}

SourceIndex::~SourceIndex()
{

}

void SourceIndex::AddDest(DestIndex* d)
{
	DestsRoutedTo.push_back(d);
}

void SourceIndex::RemoveDest(DestIndex* d)
{
	vector<DestIndex*>::iterator it;
	for(it = DestsRoutedTo.begin(); it != DestsRoutedTo.end(); ++it)
	{
		if((*it) == d)
		{
			break;
		}
	}

	if(it != DestsRoutedTo.end())
	{
		DestsRoutedTo.erase(it);
	}
}

vector<DestIndex*> SourceIndex::GetDestIndexes()
{

	return DestsRoutedTo;
}

int SourceIndex::GetUsageCount()
{
	return DestsRoutedTo.size();
}

void SourceIndex::AddSourcePackage(SourcePackage *s)
{
	SourcePackagesUsedIn.push_back(s);
}

void SourceIndex::RemoveSourcePackage(SourcePackage *s)
{
	vector<SourcePackage*>::iterator it;
	for(it = SourcePackagesUsedIn.begin(); it != SourcePackagesUsedIn.end(); ++it)
	{
		if((*it) == s)
		{
			break;
		}
	}
	
	if(it != SourcePackagesUsedIn.end())
	{
		SourcePackagesUsedIn.erase(it);
	}
}

vector<SourcePackage*> SourceIndex::GetSourcePackages()
{
	return SourcePackagesUsedIn;
}
