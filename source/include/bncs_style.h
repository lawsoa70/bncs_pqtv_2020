#include <qpalette.h>
#include <qfont.h>
#include "qwindowsstyle.h"
#include "qpixmap.h"

#define	CE_BNCS_Control		(QStyle::ControlElement) (QStyle::CE_CustomBase+1)
#define	CE_BNCS_Control_LED (QStyle::ControlElement) (QStyle::CE_CustomBase+2)
#define	CE_BNCS_Label		(QStyle::ControlElement) (QStyle::CE_CustomBase+3)
#define	CE_BNCS_Runtime		(QStyle::ControlElement) (QStyle::CE_CustomBase+4)

#define	SR_BNCS_Control		(QStyle::SubRect) (QStyle::SR_CustomBase+1)

#ifdef BNCS_STYLE_EXPORTS
	#ifndef NO_BNCS_STYLE_EXPORTS
		#define DO_STYLE_EXPORT __declspec(dllexport) 
	#else
		#define DO_STYLE_EXPORT 
	#endif
#else 
	#ifndef NO_BNCS_STYLE_EXPORTS
		#define DO_STYLE_EXPORT __declspec(dllimport) 
	#else
		#define DO_STYLE_EXPORT 
	#endif
#endif

class DO_STYLE_EXPORT  bncs_style : public QWindowsStyle
{
	Q_OBJECT
public:
	bncs_style( int style = 1 );
	virtual ~bncs_style();

	void polish( QApplication * app );
	void unPolish( QApplication * app );
	void drawControlMask( ControlElement element,
					  QPainter *p,
					  const QWidget *widget,
					  const QRect &r,
					  const QStyleOption& opt ) const;
	void drawControl ( ControlElement pe, 
									QPainter * p, 
									const QWidget* w,
									const QRect & r, 
									const QColorGroup & cg, 
									SFlags flags, 
									const QStyleOption & opt ) const;
	int pixelMetric ( PixelMetric metric, const QWidget * widget ) const;
	void polish( QWidget * );
	void unPolish( QWidget *w );

//	QPixmap colourImage( const QPixmap &in, const QColor &col ) const;
	QRect subRect ( SubRect subrect, const QWidget * widget ) const;
private:
	void bncs_control_mask( QPainter * p, QRect & r );
	void createFontAndPalette();
	QPalette *usePalette;
	QFont *useFont;
	QPalette oldapppalette;
	QFont oldappfont;
	QPalette oldwpalette;
	QFont oldwfont;
	int style;
	QPixmap bm_back;
	QColor light, mid, dark;
	QPixmap topLeft, topRight, bottomLeft, bottomRight, topPix, bottomPix, leftPix, rightPix;
};