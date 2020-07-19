#if !defined(AFX_RESPONSE_H__45FD904F_B41E_4314_AEFC_9C0B0E7FCF49__INCLUDED_)
#define AFX_RESPONSE_H__45FD904F_B41E_4314_AEFC_9C0B0E7FCF49__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class response
{
public:
	enum status
	{
		OK = 0,
		FAIL,
		READONLY,
		DONE
	};

	response(	enum status instate = OK, 
				QString inoperation = "", 
				QString invalue = "", 
				QString inerror = "" )
	{
		state = instate;
		operation = inoperation;
		value=invalue;
		error=inerror;
	};

	enum status state;
	QString operation;
	QString value;
	QString error;

	QString textStatus( enum status in )
	{
		switch( in )
		{
			case OK:		return "OK";
			case FAIL:		return "FAIL";
			case READONLY:	return "READONLY";
			case DONE:		return "DONE";
			default:		return "UNKNOWN";
		}
	};
};

#endif // !defined(AFX_RESPONSE_H__45FD904F_B41E_4314_AEFC_9C0B0E7FCF49__INCLUDED_)
