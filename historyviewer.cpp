#include "historyviewer.h"
#include <QSqlRelationalDelegate>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QDebug>

const QString and_word = " AND ";

HistoryViewer::HistoryViewer(QWidget *parent, QString item_name): QDialog(parent)
{
	specific_item_name = item_name;
	show_this_month_only = false;
	setWindowTitle(tr("History"));
	initialzeLayout();
	initialzeModel();
}

void HistoryViewer::initialzeModel()
{
	model = new QSqlRelationalTableModel(this);
	model->setTable("RECORDS");
	model->setEditStrategy(QSqlRelationalTableModel::OnManualSubmit);
	model->setRelation(1, QSqlRelation("ITEMS", "ID", "NAME"));
	model->setHeaderData(0, Qt::Horizontal, tr("Time"));
	model->setHeaderData(1, Qt::Horizontal, tr("Item"));
	model->setHeaderData(2, Qt::Horizontal, tr("Changed number"));
	model->setHeaderData(3, Qt::Horizontal, tr("Remaining number"));
	updateFilter();
	model->select();
	view->setModel(model);
}

void HistoryViewer::initialzeLayout()
{
	QPushButton *btn = new QPushButton(tr("Commit"), this);
	connect(btn, &QPushButton::pressed, this, &HistoryViewer::commitName);
	name_input = new QLineEdit(specific_item_name, this);
	QCheckBox *btn_month_only = new QCheckBox(tr("Show this month only"), this);
	view = new QTableView(this);
	view->setItemDelegate(new QSqlRelationalDelegate(this));
	QVBoxLayout *layout = new QVBoxLayout(this);
	QHBoxLayout *l1 = new QHBoxLayout(this);
	QLabel *label1 = new QLabel(tr("Name:"), this);
	l1->addWidget(label1);
	l1->addWidget(name_input);
	l1->addWidget(btn);
	layout->addLayout(l1);
	layout->addWidget(btn_month_only);
	layout->addWidget(view);
	setLayout(layout);
}

QString &appendFilter(QString &in, const QString &filter)
{
	if(in == "")
		in += filter;
	else
		in = in+and_word+filter;
	return in;
}

void HistoryViewer::updateFilter()
{
	QString filter = "";
	if(specific_item_name != "") {
		appendFilter(filter, QString("NAME = '%1'").arg(specific_item_name));
	}
	if(import_export_status == AddOnly) {
		appendFilter(filter, QString("CHANGE > 0"));
	} else if(import_export_status == ConsumeOnly) {
		appendFilter(filter, "CHANGE < 0");
	}
	if(show_this_month_only)
		appendFilter(filter, "date(TIMESTAMP) > date('start of month')");
	qDebug() << filter;
	model->setFilter(filter);
}

void HistoryViewer::showThisMonthOnly(bool show)
{

}

void HistoryViewer::commitName()
{
	specific_item_name = name_input->text();
	updateFilter();
}
