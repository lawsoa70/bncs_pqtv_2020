#if !defined(AFX_BNCS_CONTROL_FUNCS_H__63762D33_B320_48E6_A067_C9BA90D5AF7F__INCLUDED_)
#define AFX_BNCS_CONTROL_FUNCS_H__63762D33_B320_48E6_A067_C9BA90D5AF7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qwidgetplugin.h>
#include <qwidget.h>
#include <qcolor.h>
#include <qpixmap.h>
#include "notifyCallbackClass.h"
#include "timernotify.h"

class bncs_popup;
class bncs_control;

// this ensures that the control is exported for the dll but imported when used
//  called directly (i.e. not using QWidgetFactory)
/*
#ifndef BNCS_CONTROL_FUNCS_DLL_MAIN
	#undef QT_WIDGET_PLUGIN_EXPORT
	#define QT_WIDGET_PLUGIN_EXPORT
#endif
*/

class QT_WIDGET_PLUGIN_EXPORT bncs_control_funcs : public timerCallbackClass
{
	// this ensures that the property editor for this class is able to access what is 
	//  otherwise private member variables but protects these variables from everyone else
	friend class bncs_control_edit;

	// lets start with a few enums that define what this button is going to look like
	//   what the fundamental type of control this is
public:
	enum
	{
		button=1,
		label
	};

	// for alignment
	enum alignment
	{
		top=1,
		right,
		bottom,
		left,
		centre
	};

	// label border types
	enum
	{
		none=1,
		raised,
		lowered,
		blackbox,
		whitebox,
		plain
	};

	enum 
	{
		led_small = 1,
		led_medium,
		led_large
	};

	enum
	{
		as_designed = 1,
		set_minimum,
		shrink_to_fit
	};

	#define DEFAULT_STYLE			button
	#define DEFAULT_LABELSTYLE		lowered
	#define DEFAULT_NOTIFY_PRESSED	false
	#define DEFAULT_NOTIFY_RELEASED	true
	#define DEFAULT_NOTIFY_BROWSE	false
	#define DEFAULT_FCOLOUR			""
	#define DEFAULT_BCOLOUR			""
	#define DEFAULT_LEDCOLOUR		""
	#define DEFAULT_PIXMAPSTRETCH	false
	#define DEFAULT_FONT			""
	#define DEFAULT_FONTSTYLE		""
	#define DEFAULT_FONTSIZE		0
	#define DEFAULT_HTEXTALIGN		centre
	#define DEFAULT_VTEXTALIGN		centre
	#define DEFAULT_HPIXMAPALIGN	centre
	#define DEFAULT_VPIXMAPALIGN	top
	#define DEFAULT_HLEDALIGN		right
	#define DEFAULT_VLEDALIGN		top
	#define DEFAULT_LED				false
	#define DEFAULT_DOUBLEPUSH		false
	#define DEFAULT_BACKGROUND		false
	#define DEFAULT_SETMINIMUMSIZE	true
	#define DEFAULT_LEDSIZE			led_small
	#define DEFAULT_SIZEPOLICY		as_designed
	#define DEFAULT_LOST_FOCUS		false

public:
	bncs_control_funcs( QWidget * );
	virtual ~bncs_control_funcs();
	void doSetCaption( const QString & );

	// these are for the property editors that things like 
	//  the database browser components need (reference Mark Baldry)
	void doPaintEvent(	QPaintEvent *pe, 
						QWidget * widget, 
						const QRect & rect, 
						const QStyle & style,
						const QColorGroup & colorGroup,
						const QFont & font );
	void doResizeEvent(  QResizeEvent * );
	void doMousePressEvent ( QMouseEvent * e );
	void doMouseReleaseEvent ( QMouseEvent * e );
	void doDefaultValues( void );
	void doSetCaptoin( const QString & );
	QString alignToString( int h, int v );
	void stringToAlign( QString &in, int *h, int *v );
	void calcMinimumSize( void );
	void calcWidthAndHeight( int * width, int * height );
	QRect calcDrawRect(int, int, const QRect&, const QRect &);
	void godoublepush( bool start );

	void setEnabled( bool );
	void setNotifyCallback( notifyCallbackClass *n );
	notifyCallbackClass *notify;

	QFont * privateFont;

	int control_type;				// label or a button
	int labelstyle;			// raised, lowered etc.
	bool pixmapStretch;		// leave bitmap it's original size or stretch it
	bool led;
	int ledsize;
	bool isenabled;
	bool down;

	// define some colours to paint the control
	QString fcolour;
	QString bcolour;
	QString ledcolour;


	QPixmap pixmap;
	QString pixmapName;
	QString fontface;
	int fontsize;
	int usefontsize;	// this may be different from fontsize if squished by smart sizing mode
	QString fontStyle;

	QString stylesheet;
	QString statesheet;

	bool notify_pressed;
	bool notify_released;
	bool notify_browse;

	int hTextAlign;
	int vTextAlign;
	int hPixmapAlign;
	int vPixmapAlign;
	int hLEDAlign;
	int vLEDAlign;

	int sizepolicy;

	bool doublepush;		// whether this button requires two presses before actioning events
	bool background;		// whether this control is a background button
	QString text;
	bool push1;
	bncs_control * double_push_indicator;
	QString path;
	bool lostFocus;

private:
	QWidget * parent;

	#define TIMER_BACKGROUND	1
	#define TIMER_DOUBLEPUSH	2
	void timerCallback( int );
	timer *myTimer;
};

#endif // !defined(AFX_BNCS_CONTROL_FUNCS_H__63762D33_B320_48E6_A067_C9BA90D5AF7F__INCLUDED_)
