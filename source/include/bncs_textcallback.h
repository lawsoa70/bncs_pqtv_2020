/********************************************/
/* Written by David Yates                   */
/* Copyright Atos 2007 */
/********************************************/
#if !defined(AFX_BNCS_CALLBACK_H__A6E12E75_7EF2_4723_B82E_DD39DB06ED5D__INCLUDED_)
#define AFX_BNCS_CALLBACK_H__A6E12E75_7EF2_4723_B82E_DD39DB06ED5D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/** 
\class bncs_textCallback
\brief A simple text callback class

*/

class bncs_textCallback  
{
public:
/**
\param buf text buffer containing the message
\param len length of the data in buf although buf is always null terminated
*/
	virtual void textCallback( const char* buf, int len ){};
};

#endif // !defined(AFX_BNCS_CALLBACK_H__A6E12E75_7EF2_4723_B82E_DD39DB06ED5D__INCLUDED_)
