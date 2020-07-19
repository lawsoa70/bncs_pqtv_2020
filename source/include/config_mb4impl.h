#ifndef CONFIG_MB4_H
#define CONFIG_MB4_H

#include <qdialog.h>
#include <qpixmap.h>

class QPushButton;
class QLabel;
class QButtonGroup;


class config_mb4 : public QDialog
{
    Q_OBJECT

public:
	/*! enum of possible predeffined Icons */
	enum Icon {
		NoIcon = 0,/*!< Show no icon */
		Information = 1,/*!< Show Information icon */
		Warning = 2,/*!< Show Warning icon */
		Critical = 3/*!< Show Critical icon */
	};




    config_mb4( uint iNoOfButtons=4, QWidget* parent = 0, const char* name = 0);
    config_mb4( const QString & text, uint iNoOfButtons=4, QWidget* parent = 0, const char* name = 0);
    config_mb4( const QString & text, const QString & caption, Icon icon, uint iNoOfButtons=4, QWidget* parent = 0, const char* name = 0);
    ~config_mb4();
	void setText(QString text);
	void setButtonText(int button, const QString & text);
	void setIconPixmap ( const QPixmap &);
	void setIcon( Icon);

public slots:
	void buttonClicked(int id);

private:
	void CreatePanel(uint iButtons);
	QButtonGroup* bGroup;
	uint iMaxButtons;
	int iReturn;
	void closeEvent(QCloseEvent* e);
	QPushButton* pb[4];

	QLabel* textLabel;
	QLabel* iconLabel;
};

#endif // CONFIG_MB4_H
