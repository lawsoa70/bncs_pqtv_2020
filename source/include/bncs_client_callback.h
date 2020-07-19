/*************************************/
/* Original Version by David Yates   */
/* Copyright Atos 2007 */
/*************************************/
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifndef bncs_client_callback_included
	#define bncs_client_callback_included

#include <decodecaptionstring.h>

// this is the internal callback function that passes all information
class bncs_client_callback
{
public:
	enum callbackTypes
	{
		ctTEXT=0,				
		ctREVERTIVE,
		ctBUTTON,
		ctPARENT, 
		ctTIMER,
		ctFREE,
		ctDATABASE,
		ctHTTP
	};
	virtual ~bncs_client_callback(){};
	virtual void * callback( const void * message, int type = ctTEXT )=0;
};

// Revertive callback type
class CB_REVERTIVE
{
public:
	enum revertiveType
	{
		ROUTER=1,
		INFODRIVER,
		GPI,
		DATABASE,
	};
	// the following may be optionally filled by different revertive types
	// e.g. database is not used by the infodriver revertive type
	int iDevice;
	int iDatabase;		
	int iIndex;				// infodriver slot number / router index
	int iInfo;				// router source number / gpi state
	const char *szInfo;		// infodriver slot contents / router source name
	enum revertiveType type;
	const char *szDevice;	// NYI
	const char *szIndex;	// NYI
};

class CB_BUTTON
{
public:
	int iPanel;
	int iId;
	const char * szPanel;
	const char * szId;
	decodeCaptionString * dec;	// refer to the decodeCaptionString class for full explaination
};

class CB_HTTP
{
public:
	const char * url;
	const char * data;
	bool string;
	bool ok;
	const char* err;
};

#endif

#endif // DOXYGEN_SHOULD_SKIP_THIS
