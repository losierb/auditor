#include "dialogs.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QPushButton>
#include <QButtonGroup>
#include <QDebug>
#include <QList>
#include <QCloseEvent>

NewItemDialog::NewItemDialog(QWidget *parent): QDialog(parent)
{
	setWindowTitle(tr("New Item"));
	addWidgets();

}

void NewItemDialog::addWidgets()
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	QLabel *name_label = new QLabel(tr("Name"), this);
	QLabel *number_label = new QLabel(tr("Number"), this);
	line_edit = new QLineEdit(this);
	spin_box = new QSpinBox(this);
	button_box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	connect(button_box, &QDialogButtonBox::accepted, this, &NewItemDialog::accept);
	connect(button_box, &QDialogButtonBox::rejected, this, &NewItemDialog::reject);
	layout->addWidget(name_label);
	layout->addWidget(line_edit);
	layout->addWidget(number_label);
	layout->addWidget(spin_box);
	layout->addWidget(button_box);
	setLayout(layout);
}

void NewItemDialog::accept()
{
	emit commitNewItems(line_edit->text(), spin_box->value());
	QDialog::accept();
}

ChangeItemNumberDialog::ChangeItemNumberDialog(const QString &name, QWidget *parent): QDialog(parent)
{
	_name = name;
	setWindowTitle(name);
	addWidgets();
}

void ChangeItemNumberDialog::accept()
{
	int value = spin_box->value();
	if(!if_add)
		value = -value;
	emit commitChangeItemNumber(_name, value);
	QDialog::accept();
}

void ChangeItemNumberDialog::addWidgets()
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	QRadioButton *addbtn = new QRadioButton(tr("Add"), this);
	QRadioButton *conbtn = new QRadioButton(tr("Consume"), this);
	QButtonGroup *grp = new QButtonGroup(this);
	grp->setExclusive(true);
	grp->addButton(addbtn, 1);
	grp->addButton(conbtn, 2);
	connect(addbtn, &QRadioButton::toggled, this, &ChangeItemNumberDialog::setIfAdd);
	addbtn->setChecked(true);
	spin_box = new QSpinBox(this);
	spin_box->setMinimum(1); // at lease one item for a valid change.
	QDialogButtonBox *button_box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	connect(button_box, &QDialogButtonBox::accepted, this, &ChangeItemNumberDialog::accept);
	connect(button_box, &QDialogButtonBox::rejected, this, &ChangeItemNumberDialog::reject);
	layout->addWidget(addbtn);
	layout->addWidget(conbtn);
	layout->addWidget(spin_box);
	layout->addWidget(button_box);
	setLayout(layout);
}

void ChangeItemNumberDialog::setIfAdd(bool status)
{
	qDebug() << "Status changed to" << status;
	if_add = status;
}

FindItemsDialog::FindItemsDialog(TableView *table, QWidget *parent): QDialog(parent)
{
	setWindowTitle(tr("Finding items"));
	tbl = table;
	QHBoxLayout *layout = new QHBoxLayout;
	QPushButton *btn = new QPushButton(tr("Find next"), this);
	QLineEdit *edit = new QLineEdit;
	QLabel *label = new QLabel(tr("Key:"), this);
	connect(edit, &QLineEdit::textEdited, this, &FindItemsDialog::findMatchedItemsFromTable);
	connect(btn, &QPushButton::pressed, this, &FindItemsDialog::findNextItemFromTable);
	layout->addWidget(label);
	layout->addWidget(edit);
	layout->addWidget(btn);
	setLayout(layout);
}

void FindItemsDialog::setMatchedItemsBackground(QColor color)
{
	QList<QTableWidgetItem *>::iterator it;
	for(it = items.begin(); it != items.end(); ++it) {
		(*it)->setBackgroundColor(color);
	}
}

void FindItemsDialog::findMatchedItemsFromTable(const QString &key)
{
	qDebug() << "Serching text:" << key;
	if(key == QString("")) {
		setMatchedItemsBackground(QColor(Qt::white));
		items = QList<QTableWidgetItem *>();
		i = items.begin();
		searchedKeys = key;
	} else {
		searchedKeys = key;
		setMatchedItemsBackground(QColor(Qt::white));
		items = tbl->findItems(key, Qt::MatchContains);
		i = items.begin();
		setMatchedItemsBackground(QColor(Qt::yellow));
		if(!items.empty())
			emit scrollToNextMatchedItem(*i);
	}
}

void FindItemsDialog::closeEvent(QCloseEvent *event)
{
	setMatchedItemsBackground(QColor(Qt::white));
	event->accept();
}

void FindItemsDialog::findNextItemFromTable()
{
	if(!items.empty()) {
		i++;
		if(i == items.end())
			i = items.begin();
		emit scrollToNextMatchedItem(*i);
	}
}
