#include "tableview.h"
#include <QMenu>
#include <QAction>
#include <QStringList>
#include <QSqlQuery>
#include <QVariant>
#include <QColor>
#include "dialogs.h"
#include <QDebug>
#include "database.h"
#include "historyviewer.h"

TableView::TableView(Database *db, QWidget *parent): QTableWidget(parent)
{
	_db = db;
	createContextMenu();
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setContextMenuPolicy(Qt::CustomContextMenu);
	setSelectionMode(QAbstractItemView::SingleSelection);
	connect(this, &TableView::customContextMenuRequested, this, &TableView::showCustomMenu);
}

void TableView::createContextMenu()
{
	cmenu = new QMenu(tr("Custom Menu"));
	add_or_consume_action = new QAction(tr("&Add/Consume"));
	show_history_action = new QAction(tr("&Show History"));
	cmenu->addAction(add_or_consume_action);
	cmenu->addAction(show_history_action);
	connect(add_or_consume_action, &QAction::triggered, this, &TableView::popupAddConsumeDialog);
	connect(show_history_action, &QAction::triggered, this, &TableView::popupShowHistoryDialog);
}

void TableView::showCustomMenu(const QPoint &pos)
{
	QTableWidgetItem *item = itemAt(pos);
	cmenu->popup(this->viewport()->mapToGlobal(pos));

}

void TableView::popupAddConsumeDialog()
{
	int row = currentRow();
	QString name = item(row, 0)->text();
	ChangeItemNumberDialog dialog(name, this);
	connect(&dialog, &ChangeItemNumberDialog::commitChangeItemNumber, _db, &Database::changeItemNumber);
	dialog.exec();
}

void TableView::popupShowHistoryDialog()
{
	int row = currentRow();
	QString name = item(row, 0)->text();
	HistoryViewer *viewer = new HistoryViewer(this, name);
	viewer->exec();
}

void TableView::loadDatabase()
{
	QSqlQuery query("SELECT NAME,NUMBER FROM ITEMS");
	query.last();
	setRowCount(query.at()+1);
	query.first();
	setColumnCount(2);
	QStringList headers;
	headers << tr("Name") << tr("Number");
	setHorizontalHeaderLabels(headers);
	int i = 0;
	do {
		QString name = query.value(0).toString();
		int number = query.value(1).toInt();
		QTableWidgetItem *i_name = new QTableWidgetItem(name);
		QTableWidgetItem *i_number = new QTableWidgetItem(QString("%1").arg(number));
		if(number < 3)
			i_number->setBackgroundColor(QColor(Qt::red));
		setItem(i, 0, i_name);
		setItem(i, 1, i_number);
		i++;
	} while(query.next());
}
