#include "historyviewer.h"
#include <QSqlRelationalDelegate>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QDebug>
#include <QSqlQuery>
#include <QMessageBox>

const QString and_word = " AND ";
const QString last_month_only = "date(TIMESTAMP) > date('now','-1 month')";
const QString strPos = "CHANGE > 0";
const QString strNeg = "CHANGE < 0";
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
	QPushButton *btn2 = new QPushButton(tr("Summary"), this);
	connect(btn2, &QPushButton::pressed, this, &HistoryViewer::showSummary);
	name_input = new QLineEdit(specific_item_name, this);
	QCheckBox *btn_month_only = new QCheckBox(tr("Within a month only"), this);
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
	l2->addWidget(btn2);
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
		appendFilter(filter, strPos);
	} else if(add_consume_status == CONSUME_ONLY) {
		appendFilter(filter, strNeg);
	}
	if(show_this_month_only)
		appendFilter(filter, last_month_only);
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

void HistoryViewer::showSummary()
{
	int add = 0, consume = 0;
	QString selector = "SELECT SUM(CHANGE) FROM RECORDS WHERE ";
	if(specific_item_name != "") {
		QDialog *dialog = new QDialog(this);
		QString name = QString("ID = (SELECT ID FROM ITEMS WHERE NAME = '%1')").arg(specific_item_name);
		QString cond = name;
		dialog->setWindowTitle(tr("Summary of %1").arg(specific_item_name));
		QSqlQuery query;
		appendFilter(cond, strPos);
		if(show_this_month_only)
			appendFilter(cond, last_month_only);
		QString tmp = selector+cond;
		qDebug() << tmp;
		query.prepare(tmp);
		query.exec();
		if(query.next()) {
			add = query.value(0).toInt();
		}
		query.finish();
		cond = name;
		appendFilter(cond, strNeg);
		if(show_this_month_only)
			appendFilter(cond, last_month_only);
		query.prepare(selector+cond);
		query.exec();
		if(query.next()) {
			consume = -query.value(0).toInt();
		}
		QLabel *timeRange = new QLabel(this);
		if(show_this_month_only) {
			timeRange->setText(tr("Summary Within a month"));
		} else {
			timeRange->setText(tr("Summary of all times"));
		}
		QLabel *sumAdd = new QLabel(tr("Summary of addition: %1").arg(add), this);
		QLabel *sumCon = new QLabel(tr("Summary of Consumption: %1").arg(consume), this);
		QVBoxLayout *layout = new QVBoxLayout(this);
		layout->addWidget(timeRange);
		layout->addWidget(sumAdd);
		layout->addWidget(sumCon);
		dialog->setLayout(layout);
		dialog->exec();
	} else {
		QMessageBox::information(NULL, tr("Invalid Request"), tr("You haven't selected an item yet!"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
	}
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
