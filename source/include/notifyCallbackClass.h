// notifyCallback.h: interface for the notifyCallback class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NOTIFYCALLBACK_H__87E623F2_C49C_4AC8_9E0A_D681BD7A6A33__INCLUDED_)
#define AFX_NOTIFYCALLBACK_H__87E623F2_C49C_4AC8_9E0A_D681BD7A6A33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class notifyCallbackClass
{
public:
	virtual void notifyCallback( const QString & sender, const QString & value )=0;

};

#endif // !defined(AFX_NOTIFYCALLBACK_H__87E623F2_C49C_4AC8_9E0A_D681BD7A6A33__INCLUDED_)
