#ifndef HISTORYVIEWER_H
#define HISTORYVIEWER_H

#include <QDialog>
#include <QTableView>
#include <QSqlRelationalTableModel>
#include <QLineEdit>

typedef enum {
	AddOnly = 1,
	ConsumeOnly = 2,
	All = 3,
} TypeToShow;

class HistoryViewer: public QDialog
{
	Q_OBJECT
public:
	HistoryViewer(QWidget *parent = 0, QString item_name = "");
private:
	QTableView *view;
	QLineEdit *name_input;
	QSqlRelationalTableModel *model;
	TypeToShow import_export_status;
	QString specific_item_name;
	bool show_this_month_only;
	void initialzeModel();
	void initialzeLayout();
	void updateFilter();
private slots:
	void showThisMonthOnly(bool show);
	void commitName();
};

#endif // HISTORYVIEWER_H
