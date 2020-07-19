#ifndef cfg_Table_H
#define cfg_Table_H

#ifdef QT_PLUGIN
#define QT_CONFIG_TABLE_PLUGIN_EXPORT __declspec(dllexport)
#else
#define QT_CONFIG_TABLE_PLUGIN_EXPORT 
#endif


#include <qtable.h>
#include <qwidgetplugin.h>

class QT_CONFIG_TABLE_PLUGIN_EXPORT cfg_Table : public QTable
{
	Q_OBJECT

public:
	/*! enum of return values for validData */
	enum validData {
				DATA_VALID,		/*!< This row is valid */
				DATA_INVALID,	/*!< This row is invalid, return to previous contents */
				DATA_ALL_VALID,	/*!< All rows are valid, do not ask again */
				CANCEL			/*!< Cancel the paste, and retunr all rows to previous states */
				};

    cfg_Table( QWidget *parent = 0, const char *name = 0 );

public slots:
	void deleteItems();
	void copyToClipboard();
	void undoPaste();
	void pasteClipboard();


protected:
	virtual int validateRow(int iRow);

protected slots:
	void clipBoardDataChanged();

private:
	QStringList* undoBuffer;
	void fillUndoBuffer();

signals:
	void dataPasted();	/*!< Signal issued when data has been pasted, allowing the application to check the table, at this point undoPaste() can be called if the changes need o be undone.*/
	void clipboardDataValid(bool); /*!< Signal issued when the data on the clipboard has changed, indicating if it is suitable to paste, or not.*/

};


#endif
