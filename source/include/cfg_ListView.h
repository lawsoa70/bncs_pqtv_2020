// cfg_ListView.h: interface for the cfg_ListView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CFG_LISTVIEW_H__ECD3786C_F6B1_4BF3_8D88_399509C1B53B__INCLUDED_)
#define AFX_CFG_LISTVIEW_H__ECD3786C_F6B1_4BF3_8D88_399509C1B53B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef QT_PLUGIN
#define QT_CONFIG_LISTVIEW_PLUGIN_EXPORT __declspec(dllexport)
#else
#define QT_CONFIG_LISTVIEW_PLUGIN_EXPORT 
#endif


#include <qlistview.h> 
#include <qwidgetplugin.h>

class QT_CONFIG_LISTVIEW_PLUGIN_EXPORT cfg_ListView : public QListView  
{
	Q_OBJECT


public:
	/*! enum of return values for validData */
	enum validData {
				DATA_VALID,		/*!< This row is valid */
				DATA_INVALID,	/*!< This row is invalid, return to previous contents */
				DATA_ALL_VALID,	/*!< All rows are valid, do not ask again */
				CANCEL			/*!< Cancel the paste, and return all rows to previous states */
				};
	cfg_ListView( QWidget *parent = 0, const char *name = 0 );
	virtual ~cfg_ListView();
	bool bClearSpareColumns; /*!< If true when replaceData() is called all data in un-pasted columns will be cleared, the default is false.*/

public slots:
	void insertData();
	void deleteItems();
	void copyToClipboard();
	void undoPaste();
	void replaceData();

protected:
	virtual QListViewItem* newItem(QListView* parent, QListViewItem * after);
	virtual int validateRow(QListViewItem* lvi);

protected slots:
	void clipBoardDataChanged();

private:
	void clearUndo();
	QListViewItem* updateEntry(QListViewItem* lvi, QString sData, bool bClear);

	QPtrList<QListViewItem>* replacedItems;
	QPtrList<QListViewItem>* newItems;

signals:
	void dataPasted(); /*!< Signal issued when data has been pasted, allowing the application to check the table, at this point undoPaste() can be called if the changes need to be undone.*/
	void clipboardDataValid(bool); /*!< Signal issued when the data on the clipboard has changed, indicating if it is suitable to paste, or not.*/

};

#endif // !defined(AFX_CFG_LISTVIEW_H__ECD3786C_F6B1_4BF3_8D88_399509C1B53B__INCLUDED_)
