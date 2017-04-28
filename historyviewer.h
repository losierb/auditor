#ifndef HISTORYVIEWER_H
#define HISTORYVIEWER_H

#include <QDialog>
#include <QTableView>
#include <QSqlRelationalTableModel>
#include <QLineEdit>
#include <QComboBox>

class HistoryViewer: public QDialog
{
	Q_OBJECT
public:
	HistoryViewer(QWidget *parent = 0, QString item_name = "");
private:
	QTableView *view;
	QComboBox *cbox;
	QLineEdit *name_input;
	QSqlRelationalTableModel *model;
	int add_consume_status;
	QString specific_item_name;
	bool show_this_month_only;
	void initialzeModel();
	void initialzeLayout();
	void updateFilter();
private slots:
	void showThisMonthOnly(int state);
	void showSummary();
	void commitTypeToShow(int index);
	void commitName();
};

#endif // HISTORYVIEWER_H
