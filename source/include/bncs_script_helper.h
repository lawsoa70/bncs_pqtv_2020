/*************************************/
/* Original Version by David Yates   */
/* Copyright Siemens IT Systems 2007 */
/*************************************/
#ifndef BNCS_SCRIPT_HELPER_INCLUDED
	#define BNCS_SCRIPT_HELPER_INCLUDED

#include <bncs_client_callback.h>

#include <bncs_string.h>
#include <bncs_stringlist.h>


static char BNCS_SCRIPTHELPER_SIG[]="BNCS_SIG,name=bncs_script_helper,type=LIB,version=4.8.4.0,date=05/11/2015,author=David Yates";
/*	enum revertiveType
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
	int iInfo;				// router source number
	char szInfo[ 256 ];		// infodriver slot contents / router source name
	enum revertiveType type;
	const char *szDevice;	// NYI
	const char *szIndex;	// NYI
*/


/** 
\class revertiveNotify
\brief A class that passes on information about BNCS events

\par Example - Infodriver
\code
// for an infodriver revertive from slot 456 of device 123 containing text "Hello mum"
void mypanel::revertiveCallback( revertiveNotify *r )         
{    
        int device = r->device();		// device is 123
        int index = r->index();			// index is 456
        bncs_string info = r->sInfo();		// info is "hello mum"
        int type = r->type();			// type is 2 (see CB_REVERTIVE documentation)
}
\endcode
\par Example - Router
\code
// for a router revertive from destination 456 of device 123 for source 1 of name "0001"
void mypanel::revertiveCallback( revertiveNotify *r )         
{    
        int device = r->device();		// device is 123
        int index = r->index();			// index is 456
        int iInfo = r->info();			// info is 1
        bncs_string sInfo = r->sInfo();		// name is "0001"
        int type = r->type();			// type is 1 (see CB_REVERTIVE documentation)
}
\endcode
\par Example - Database
\code
// for a database 1 name change for index 456 of device 123
void mypanel::databaseCallback( revertiveNotify *r )         
{    
        int device = r->device();		// device is 123
        int index = r->index();			// index is 456
        int database = r->database();		// database is 1
        int type = r->type();			// type is 4 (see CB_REVERTIVE documentation)
}
\endcode
*/
class revertiveNotify
{
private:
	CB_REVERTIVE *r;
public:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
	/** Used internally */
	revertiveNotify( CB_REVERTIVE * in ) 
	{
		r = in;
	};
#endif // DOXYGEN_SHOULD_SKIP_THIS
	/** Device number 

	Return the device ID of the device that generated this notification
	*/
	int device( void )
	{
		return r->iDevice;
	};
	/** Database number

	Return the number of the database - this is only relevant for database change notification messages.
	Its value is undefined for other notification messages
	*/
	int database( void )
	{
		return r->iDatabase;
	};
	/** Index

	Return the index for which this notification relates. For an infodriver it is the slot number, for a router it is the destination number.
	*/
	int index( void )
	{
		return r->iIndex;
	};
	/** Integer information

	Return the integer information. For a router this is the source number whereas sInfo() returns the source name. For GPIs this is the state. For Infodrivers this value is zero.
	*/
	int info( void )
	{
		return r->iInfo;
	};
	/** String information

	Return the string information. For a router this is the source name whereas info() returns the source number. For an infodriver this value is the slot contents. For GPIs this value is null.
	*/
	bncs_string sInfo( void )
	{
		return r->szInfo;
	};
	/** The type of revertive that this is.

	This is defined in CB_REVETIVE as:
	
\code
	enum revertiveType
	{
		ROUTER=1,		// 1
		INFODRIVER,		// 2
		GPI,			// 3
		DATABASE,		// 4
	};
\endcode
	*/
	int type( void )
	{
		return r->type;
	};

	/** Send the raw notification string to the debugger with optional header
	\param[in] header Text that will prepend the debug string for identification purposes
	*/
	void dump( const char* header = 0 )
	{
		bncs_string out;

		switch( r->type )
		{
			case 1:
				out = bncs_string( "RR %1 %2 %3 '%4'" ).arg( r->iDevice ).arg( r->iIndex ).arg( r->iInfo ).arg( r->szInfo );
				break;
			case 2:
				out = bncs_string( "IR %1 %2 '%3'" ).arg( r->iDevice ).arg( r->iIndex ).arg( r->szInfo );
				break;
			case 3:
				out = bncs_string( "GR %1 %2 %3" ).arg( r->iDevice ).arg( r->iIndex ).arg( r->iInfo );
				break;
			case 4:
				out = bncs_string( "RD %1 %2 %3 '%4'" ).arg( r->iDevice ).arg( r->iIndex ).arg( r->iDatabase ).arg( r->szInfo );
				break;
		}

		if( header )
			OutputDebugString( bncs_string( "%1: %2" ).arg( header ).arg( out ));
		else
			OutputDebugString( out );
	}

};

/** 
\class buttonNotify
\brief A class that passes on information about button notifications

This class gives easy access to the id of the button that generated a notification
and the panel on which it sits.\n
Parameter passing is using\n
	&lt;parameter&gt;=&lt;value&gt;\n
method and this class also provides direct access to the &lt;parameter&gt; and &lt;value&gt;.

\par Example
\code
// for button called "mybutton" on panel "main" notifying us of "button.colour.top=red"
void mypanel::buttonCallback( buttonNotify *b )         
{    
        bncs_string s1 = b->id();		// s1 contains "mybutton"
        bncs_string s2 = b->panel();	// s2 contains "main"
        bncs_string s3= b->command();   // s3 contains "button"
        int i = b->subs();		// i contains 2
        bncs_string s4 = b->sub( 0 );   // s4 contains "colour"
        bncs_string s5 = b->sub( 1 );   // s5 contains "top"
        bncs_string s6 = b->value();	// s6 contains "red"
}
\endcode
In reality panel and button ids are likely to be numeric so you're likely to write something like this\n
Note that the bncs_string takes care of the conversion to type int to keep the switch-case happy.
\code
// for button called "mybutton" on panel "main" notifying us of "button.colour.top=red"
void mypanel::buttonCallback( buttonNotify *b )         
{    
	switch( b->panel())
	{
		case 1:
			switch( b->id())
			{
				case 2:
					textPut( "text", "you pressed button 2 on panel 1", 1, 1 );
			}
			break;
	}
}
\endcode
*/
class buttonNotify
{
private:
	CB_BUTTON * b;
public:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
	/** Used internally */
	buttonNotify( CB_BUTTON * in ) 
	{
		b = in;
	};
#endif // DOXYGEN_SHOULD_SKIP_THIS
	/** Button ID

	Return the ID of the button that generated this notification
	*/
	bncs_string id()
	{ 
		return b->szId; 
	};
	/** Panel ID

	Return the name of the panel which hosted the button
	*/
	bncs_string panel()
	{ 
		return b->szPanel; 
	};
	/** Number of sub command parameters

	Return the number of sub parameters this notification had\n
	\code
	button.colour=red
	\endcode
	has a subs() count of 1\n
	*/
	int subs()
	{ 
		return b->dec->subs; 
	};
	/** Return the sub parameters at given index
	\param in zero based index of parameter to return
	\par Example:
	\code
// for a notification of  "button.colour.top=red"
void mypanel::buttonCallback( buttonNotify *b )         
{    
	bncs_string s1 = b->sub( 0 );	// s1 contains "colour"
	bncs_string s2 = b->sub( 1 );	// s2 contains "top"
}
	\endcode
	*/
	bncs_string sub( int in )
	{ 
		return b->dec->sub[ in ]; 
	};
	/** Return the notification command
	\par Example:
	\code
// for a notification of  "button.colour=red"
void mypanel::buttonCallback( buttonNotify *b )         
{    
	bncs_string s = b->command();	// s contains "button"
}
	\endcode
	*/
	bncs_string command()
	{ 
		return b->dec->command; 
	};
	/** Return the notification value
	\par Example:
	\code
// for a notification of  "button.colour=red"
void mypanel::buttonCallback( buttonNotify *b )         
{    
	bncs_string s = b->value();	// s contains "red"
}
	\endcode
	*/
	bncs_string value()
	{ 
		return b->dec->value; 
	};

	/** Send the raw notification string to the debugger with optional header
	\param[in] header Text that will prepend the debug string for identification purposes
	*/
	void dump( const char* header = 0 )
	{
		if( b && b->dec )
		{
			bncs_string out = b->szId;
			
			out += ": ";
			out += b->dec->command;

			for( int x = 0 ; x < b->dec->subs ; x++ )
				out += bncs_string( ".%1" ).arg( b->dec->sub[ x ] );

			out += "=";
			out += b->dec->value;

			if( header )
				OutputDebugString( bncs_string( "%1: %2" ).arg( header ).arg( out ));
			else
				OutputDebugString( out );
		}
	}
};

/** 
\class parentNotify
\brief A class that passes on information about parent notifications
*/
class parentNotify
{
private:
	decodeCaptionString *dec;
public:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
	/** Used internally */
	parentNotify( decodeCaptionString *in)
	{
		dec = in;
	}
#endif // DOXYGEN_SHOULD_SKIP_THIS
	/** Number of sub command parameters

	Return the number of sub parameters this notification had\n
	\code
	button.colour=red
	\endcode
	has a subs() count of 1\n
	*/
	int subs()
	{ 
		return dec->subs; 
	};
	/** Return the sub parameters at given index
	\param in zero based index of parameter to return
	\par Example:
	\code
// for a notification of  "button.colour.top=red"
void mypanel::parentCallback( parentNotify *p )         
{    
	bncs_string s1 = p->sub( 0 );	// s1 contains "colour"
	bncs_string s2 = p->sub( 1 );	// s2 contains "top"
}
	\endcode
	*/
	bncs_string sub( int in )
	{ 
		return dec->sub[ in ]; 
	};
	/** Return the notification command
	\par Example:
	\code
// for a notification of  "button.colour=red"
void mypanel::parentCallback( parentNotify *p )         
{    
	bncs_string s = p->command();	// s contains "button"
}
	\endcode
	*/
	bncs_string command()
	{ 
		return dec->command; 
	};
	/** Return the notification value
	\par Example:
	\code
// for a notification of  "button.colour=red"
void mypanel::parentCallback( parentNotify *p )         
{    
	bncs_string s = p->value();	// s contains "red"
}
	\endcode
	*/
	bncs_string value()
	{ 
		return dec->value; 
	};

	/** Send the raw notification string to the debugger with optional header
	\param[in] header Text that will prepend the debug string for identification purposes
	*/
	void dump( const char* header = 0 )
	{
		if( dec )
		{
			bncs_string out = dec->command;

			for( int x = 0 ; x < dec->subs ; x++ )
				out += bncs_string( ".%1" ).arg( dec->sub[ x ] );

			out += "=";
			out += dec->value;

			if( header )
				OutputDebugString( bncs_string( "%1: %2" ).arg( header ).arg( out ));
			else
				OutputDebugString( out );
		}
	}
};




/** 
\class httpNotify
\brief A class that passes on information about HTTP (file transfer) notifications

This class tells you the end-state of an HTTP file transfer

\par Example
\code
// for a remote file from http://myhost.com/file.png to a local file /panels/images/myfile.png
void mypanel::buttonCallback( buttonNotify *b )         
{    
        bncs_string s1 = b->url();	// s1 contains "http://myhost.com/file.png"
        bncs_string s2 = b->data();	// s2 contains "/panels/images/myfile.png"
        bool b1 = b->string();		// b1 is false - this was a file download
        bool b2 = b->ok();			// b2 is true - the file transferred ok
}
\endcode
*/
class httpNotify
{
private:
	CB_HTTP * h;
public:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
	/** Used internally */
	httpNotify( CB_HTTP * in ) 
	{
		h = in;
	};
#endif // DOXYGEN_SHOULD_SKIP_THIS
	/** url

	Return the URL of the item transferred
	*/
	bncs_string url( void )
	{ 
		return h->url; 
	};
	/** file

	Return the local file name or actual data of the item transferred
	*/
	bncs_string data( void )
	{ 
		return h->data; 
	};

	/** string

	Return the flag which says if this was a file transfer (false) or a string transfer (true)
	*/
	bool string( void )
	{
		return h->string;
	};

	/** ok

	Return whether this HTTP transfer was ok (true) or failed (false )
	*/
	bool ok( void )
	{
		return h->ok;
	};

	/** err

	Return the error message string (null if nothing interesting happened)
	*/
	bncs_string err( void )
	{ 
		return h->err; 
	};

};

class  bncs_script_helper : public bncs_client_callback
{
public:
	bool getDevSlot(const bncs_string & instance, const bncs_string & param, int* dev, int* slot, char* type);
	bool getDevSlot( const bncs_string & instance, const bncs_string & param, bncs_string & altId, int *dev, int *slot, char *type);
	bool getDev(bncs_string instance, int* dev, int* offset = 0);
	void audioStopAll();
	void audioDestroy(const bncs_string &name);
	void audioStop(const bncs_string &name);
	void audioPlay(const bncs_string & name, int repeat);
	void audioPlay(const bncs_string & name, const bncs_string & file, int repeat);
	bncs_script_helper( bncs_client_callback * parent, const char * path );
	virtual ~bncs_script_helper();
	void* callback( const void *, int );

	// panel commands
	bool panelShow( const bncs_string & pnl, const bncs_string & path, const bncs_string & target="" );
	bool panelInit( const bncs_string & pnl, const bncs_string & path, const bncs_string & target="" );
	bool panelTarget( const bncs_string & pnl, const bncs_string & target);
	bool panelPopup( const bncs_string & pnl, const bncs_string & path, int width = 0, int height = 0 );
	bool panelRemove( const bncs_string & pnl );
	bool panelDestroy( const bncs_string & pnl );

	// text commands
	void textPut( const bncs_string & what, const bncs_string & pnl, const bncs_string & id );
	void textPut( const bncs_string & param, const bncs_string & value, const bncs_string & pnl, const bncs_string & id );
	void textGet( const bncs_string & what, const bncs_string & pnl, const bncs_string & id, bncs_string & ret);

	// router commands
	void routerName( int dev, int db, int index, bncs_string & ret );
	int routerIndex( int dev, int db, const bncs_string & name );
	void routerCrosspoint( int dev, int source, int dest, const bncs_string & mask="", bool now = false  );
	void routerRegister( int dev, int start, int end, bool add=false );
	void routerUnregister( int dev );
	void routerPoll( int dev, int start, int end );
	void routerQuery( int dev, int start, int end );
	void routerModify( int dev, int database, int index, const bncs_string& name, bool reload );

	// info commands
	void infoUnregister( int dev );
	void infoPoll( int dev, int start, int end );
	void infoWrite( int dev, const bncs_string & what, int index, bool now = false );
	void infoWrite( const bncs_string & format, ... );
	void infoQuery( int dev, int start, int end );
	void infoRegister( int dev, int start, int end, bool add=false );

	// gpi commands
	void gpiRegister( int dev, int start, int end, bool add=false );
	void gpiSwitch( int dev, bool state, int index, bool now = false  );
	void gpiPoll( int dev, int start, int end );
	void gpiQuery( int dev, int start, int end );
	void gpiUnregister( int dev );

	// other commands
	void timerStart( int timer, int duration );
	void timerStop( int timer );
	void captionEnable( bool captionOn, const bncs_string & title, const bncs_string & args = "" );
	void setGeometry( int x, int y, int w, int h );
	void setSize( int w, int h );
	void setSize( const bncs_string & pnl );

	// control commands
	void controlEnable( const bncs_string & panel, const bncs_string & id );
	void controlDisable( const bncs_string & panel, const bncs_string & id );

	void controlShow( const bncs_string & panel, const bncs_string & id );
	void controlHide( const bncs_string & panel, const bncs_string & id );

	void controlMove( const bncs_string & panel, const bncs_string & id, int x, int y );
	void controlPosition( const bncs_string & panel, const bncs_string & id, bncs_string & x, bncs_string & y);

	bncs_string controlCreate( const bncs_string & panel, const bncs_string & id, const bncs_string & buttonClass, int x, int y, int w, int h, const bncs_string & settings );
	
	// host commands
	void hostNotify( const bncs_string & message );
	bncs_string id( void );

	// callbacks
	/** Button notification

	This gets called whenever a button wants to notify us of something\n
	See the buttonNotify documentation for the values it returns
	*/
#ifdef BNCS_CALLBACK_BY_REF
	virtual void buttonCallback( buttonNotify &b )=0;
#else
	virtual void buttonCallback( buttonNotify *b )=0;
#endif
	/** Revertive notification

	This gets called whenever network event occurs
	See the CB_REVERTIVE documentation for the values it returns
	*/
#ifdef BNCS_CALLBACK_BY_REF
	virtual int revertiveCallback( revertiveNotify & r )=0;
#else
	virtual int revertiveCallback( revertiveNotify * r )=0;
#endif

	/** Parent notification

	This gets called whenever our parent wants to tell us something
	See the parentNotify documentation for the values it returns
	*/
#ifdef BNCS_CALLBACK_BY_REF
	virtual bncs_string parentCallback( parentNotify &p )=0;
#else
	virtual bncs_string parentCallback( parentNotify *p )=0;
#endif

	/** HTTP notification

	This gets called whenever HTTP wants to tell us something
	See the parentNotify documentation for the values it returns
	*/
#ifdef BNCS_CALLBACK_BY_REF
	virtual void httpCallback( httpNotify &p ){};		// this has an implementation so that existing scripts don't throw an error
#else
	virtual void httpCallback( httpNotify *p ){};
#endif

	/** Timer notification

	This gets called whenever a timer event occurs
	See the timerStart() / timerStop() documentation for usage information
	*/
	virtual void timerCallback( int )=0;

	/** Timer notification

	This gets called whenever a database change event occurs
	See the CB_REVERTIVE documentation for the values it returns
	*/
	virtual void databaseCallback( revertiveNotify * r ){};

	void debug( const bncs_string & str );
	void debug( const bncs_string & format, const bncs_string & in1, const bncs_string & in2="", const bncs_string & in3="", const bncs_string & in4="", const bncs_string & in5="", const bncs_string & in6="", const bncs_string & in7="", const bncs_string & in8="", const bncs_string & in9="" );

	int workstation( void );

	void navigateAdjust( const bncs_string & str );
	void navigateExecute( const bncs_string & str );
	bool navigateCanExecute( const bncs_string & str );
	bool navigateCanAdjust( const bncs_string & str );
	bncs_stringlist getWorkstationSettings( void );
	bncs_string getWorkstationSetting(const bncs_string & setting);
	bncs_string getObjectSetting(const bncs_string & object, const bncs_string & setting);
	int getRouterSize(int intDevice, int intSwitch);
	bncs_string getUser( void );
	bncs_string getUserLevel( void );
	int getUserLevelInt( void );
	bncs_stringlist getIdList( const bncs_string & pnl, const bncs_string & startsWithFilter="" );
	void enableActionAll();
	void actionAll(const bncs_string & sValue);

	void httpGet( const bncs_string & url, const bncs_string & localFile );
	void httpGetString( const bncs_string & url );

	void httpPut( const bncs_string & url, const bncs_string & localFile );
	void httpPutString( const bncs_string & url, const bncs_string & string );

private:
	bncs_client_callback * logic;
	bncs_string exePath;
	boolean m_bEnableAck;
};

#define EXPORT_BNCS_SCRIPT( a )		extern "C" \
{\
	EXPORT_SCRIPT bncs_client_callback * load_bncs_client_script( bncs_client_callback * parent, const char * path ) {return new a( parent, path );};\
}\

#define DESCRIBE_BNCS_SCRIPT( a )		extern "C" \
{\
	EXPORT_SCRIPT const char * describe_bncs_client_script( void ) {return a;};\
}\

#endif // BNCS_SCRIPT_HELPER_INCLUDED
