#ifndef DIALOGS_H
#define DIALOGS_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QTableWidgetItem>
#include "tableview.h"

class NewItemDialog: public QDialog
{
	Q_OBJECT
public:
	NewItemDialog(QWidget *parent = 0);
signals:
	void commitNewItems(const QString name, int initial_number);
public slots:
	void accept();
private:
	void addWidgets();
	QDialogButtonBox *button_box;
	QLineEdit *line_edit;
	QSpinBox *spin_box;
};

class ChangeItemNumberDialog: public QDialog
{
	Q_OBJECT
public:
	ChangeItemNumberDialog(const QString &name, QWidget *parent = 0);
signals:
	void commitChangeItemNumber(const QString name, int number_change);
public slots:
	void accept();
private:
	void addWidgets();
	bool if_add;
	QDialogButtonBox *button_box;
	QSpinBox *spin_box;
	QString _name;
private slots:
	void setIfAdd(bool status);
};

class FindItemsDialog: public QDialog
{
	Q_OBJECT
public:
	FindItemsDialog(TableView *table, QWidget *parent = 0);
signals:
	void scrollToNextMatchedItem(const QTableWidgetItem *item, QAbstractItemView::ScrollHint hint = QAbstractItemView::EnsureVisible);
private:
	QList<QTableWidgetItem *> items;
	QString searchedKeys;
	QList<QTableWidgetItem *>::iterator i;
	TableView *tbl;
	void setMatchedItemsBackground(QColor color);
	void closeEvent(QCloseEvent *event);
private slots:
	void findNextItemFromTable();
	void findMatchedItemsFromTable(const QString &key);
};

#endif // DIALOGS_H
