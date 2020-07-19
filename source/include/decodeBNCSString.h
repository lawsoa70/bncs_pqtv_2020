#if !defined(AFX_DECODEBNCSSTRING_H__E35F7688_22E3_4717_BE27_D3C54B16D051__INCLUDED_)
#define AFX_DECODEBNCSSTRING_H__E35F7688_22E3_4717_BE27_D3C54B16D051__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define BNCS_STRING_SUBS 32
  
class decodeBNCSString  
{
public:
	decodeBNCSString( const char * in, char delim = '\'' );
	decodeBNCSString(const decodeBNCSString & in);
		virtual ~decodeBNCSString();
/** List of sub elements from the input string */
	const char *sub[ BNCS_STRING_SUBS ];
/** The number of sub elements from the input string */
	int subs;
private:
	char *copy;
	char m_delim;
	void split(const char * in);
};

#endif // !defined(AFX_DECODEBNCSSTRING_H__E35F7688_22E3_4717_BE27_D3C54B16D051__INCLUDED_)
