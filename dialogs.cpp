#include "dialogs.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>
#include <QDebug>

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
