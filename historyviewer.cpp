#include "historyviewer.h"
#include <QSqlRelationalDelegate>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QDebug>

const QString and_word = " AND ";

const int ADD_ONLY = 0;
const int CONSUME_ONLY = 1;
const int ADD_AND_CONSUME = 2;

HistoryViewer::HistoryViewer(QWidget *parent, QString item_name): QDialog(parent)
{
	specific_item_name = item_name;
	add_consume_status = ADD_AND_CONSUME;
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
	cbox = new QComboBox(this);
	cbox->insertItem(ADD_ONLY, tr("Add only"));
	cbox->insertItem(CONSUME_ONLY, tr("Consume only"));
	cbox->insertItem(ADD_AND_CONSUME, tr("All"));
	cbox->setCurrentIndex(ADD_AND_CONSUME);
	connect(cbox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &HistoryViewer::commitTypeToShow);
	QPushButton *btn = new QPushButton(tr("Commit"), this);
	connect(btn, &QPushButton::pressed, this, &HistoryViewer::commitName);
	name_input = new QLineEdit(specific_item_name, this);
	QCheckBox *btn_month_only = new QCheckBox(tr("Show this month only"), this);
	connect(btn_month_only, &QCheckBox::stateChanged, this, &HistoryViewer::showThisMonthOnly);
	view = new QTableView(this);
	view->setItemDelegate(new QSqlRelationalDelegate(this));
	QVBoxLayout *layout = new QVBoxLayout(this);
	QHBoxLayout *l1 = new QHBoxLayout(this);
	QHBoxLayout *l2 = new QHBoxLayout(this);
	QLabel *label1 = new QLabel(tr("Name:"), this);
	l1->addWidget(label1);
	l1->addWidget(name_input);
	l1->addWidget(btn);
	layout->addLayout(l1);
	l2->addWidget(btn_month_only);
	l2->addWidget(cbox);
	layout->addLayout(l2);
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
	if(add_consume_status == ADD_ONLY) {
		appendFilter(filter, QString("CHANGE > 0"));
	} else if(add_consume_status == CONSUME_ONLY) {
		appendFilter(filter, "CHANGE < 0");
	}
	if(show_this_month_only)
		appendFilter(filter, "date(TIMESTAMP) > date('start of month')");
	qDebug() << filter;
	model->setFilter(filter);
}

void HistoryViewer::showThisMonthOnly(int state)
{
	if(state == Qt::Checked)
		show_this_month_only = true;
	else
		show_this_month_only = false;
	updateFilter();
}

void HistoryViewer::commitTypeToShow(int index)
{
	add_consume_status = index;
	qDebug() << index;
	updateFilter();
}

void HistoryViewer::commitName()
{
	specific_item_name = name_input->text();
	updateFilter();
}
