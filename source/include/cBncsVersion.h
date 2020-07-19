// cBncsVersion.h: interface for the cBncsVersion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CBNCSVERSION_H__ED778E2D_D8CF_486A_9007_D0D8277156B5__INCLUDED_)
#define AFX_CBNCSVERSION_H__ED778E2D_D8CF_486A_9007_D0D8277156B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class cBncsVersion  
{
public:
	cBncsVersion(const char* szFile);
	virtual ~cBncsVersion();

             
	char* szComments;			/*!< Comment value */
	char* szInternalName;		/*!< Internal name */
	char* szProductName;		/*!< Product Name */
	char* szCompanyName;		/*!< Company Name */
	char* szLegalCopyright;		/*!< Copyright information*/
	char* szProductVersion;		/*!< Product version */
	char* szLegalTradeMarks;	/*!< Legal trademarks */
	char* szPrivateBuild;		/*!< Private build */
	char* szOriginalFileName;	/*!< Original file name */
	char* szSpecialBuild;		/*!< Special build */
	char* szFileVersion;		/*!< File version */
	char* szFileDescription;	/*!< File Description */


private:
	char* szFileName;
};

#endif // !defined(AFX_CBNCSVERSION_H__ED778E2D_D8CF_486A_9007_D0D8277156B5__INCLUDED_)
