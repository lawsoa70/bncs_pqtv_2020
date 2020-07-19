
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the ALM_SYS_SELECT_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// ALM_SYS_SELECT_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef ALM_SYS_SELECT_EXPORTS
#define ALM_SYS_SELECT_API __declspec(dllexport)
#else
#define ALM_SYS_SELECT_API __declspec(dllimport)
#endif

#include <windows.h>
#include <qstring.h>
#include <qvbox.h>

class response;
class sys_selectBase;
class QStringList;

// This class is exported from the alm_sys_select.dll
class ALM_SYS_SELECT_API CAlm_sys_select : public QVBox
{
	Q_OBJECT

public:
	void setSystem(const QString& sSystem);
	QString getSystemName();
	bool setSystemNames(QString sNames);
	CAlm_sys_select(QWidget* parent,const char* name=0);

protected slots:
	void selectSystem();

private:
	QStringList slSysNames;
	static QString sCurrentSystem;
	sys_selectBase* display;

signals:
	void sysChanged(const QString&); /*!< Signal issued when the user selects a different system, note that this is only issued to the current plugin.*/
};

#ifdef ALM_SYS_SELECT_CLASS
QString CAlm_sys_select::sCurrentSystem;
#endif