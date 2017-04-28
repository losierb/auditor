#include "database.h"
#include <QUndoCommand>
#include <QFile>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QSqlError>

const QString create_table_items = "CREATE TABLE ITEMS (ID INTEGER PRIMARY KEY AUTOINCREMENT,NAME TEXT NOT NULL,NUMBER INTEGER NOT NULL)";
const QString create_table_records = "CREATE TABLE RECORDS (INTEGER TIMESTAMP PRIMARY KEY NOT NULL,ID INTEGER NOT NULL,CHANGE INTEGER NOT NULL,AFTER INTEGER NOT NULL)";
const QString insert_into_items = "INSERT INTO ITEMS (NAME,NUMBER) VALUES (:NAME,:NUMBER)";
const QString query_items_id = "SELECT ID FROM ITEMS WHERE NAME = :NAME";
const QString query_items_number = "SELECT NUMBER FROM ITEMS WHERE NAME = :NAME";
const QString insert_into_records = "INSERT INTO RECORDS VALUES (:TIMESTAMP,:ID,:NUMBER,:AFTER)";
const QString delete_from_items = "DELETE FROM ITEMS WHERE NAME = :NAME";
const QString delete_from_records = "DELETE FROM RECORDS WHERE TIMESTAMP = :TIMESTAMP";
const QString query_records_between_dates = "SELECT * FROM RECORDS WHERE TIMESTAMP BETWEEN :T1 AND :T2";
const QString query_records_by_iems = "SELECT * FROM RECORDS WHERE ID = (SELECT ID FROM ITEMS WHERE NAME = :NAME)";
const QString update_items_number = "UPDATE ITEMS SET NUMBER = :NUMBER WHERE NAME = :NAME";
const QString update_items_name = "UPDATE ITEMS SET NAME = :NEWNAME WHERE NAME = :NAME";

class UndoCommand: public QUndoCommand
{
public:
	UndoCommand(Database *db, QDateTime &ts, const QString &name, int original_amount, int amount_change, bool is_new_item);
	void undo();
	void redo();
private:
	Database *database;
	QDateTime timestamp;
	QString item_name;
	int _original_amount;
	int _amount_change;
	bool new_item;
};

UndoCommand::UndoCommand(Database *db, QDateTime &ts, const QString &name, int original_amount, int amount_change, bool is_new_item)
{
	_original_amount = original_amount;
	_amount_change = amount_change;
	database = db;
	timestamp = ts;
	item_name = name;
	new_item = is_new_item;
}

void UndoCommand::undo()
{
	qDebug() << "undo()";
	database->delRecordP(timestamp);
	if(new_item)
		database->delItemP(item_name);
	else
		database->changeItemNumberP(item_name, _original_amount);
}

void UndoCommand::redo()
{
	qDebug() << "redo()";
	if(new_item)
		database->addItemP(item_name, _amount_change);
	else
		database->changeItemNumberP(item_name, _original_amount+_amount_change);
	database->addRecordP(timestamp, item_name, _amount_change, _original_amount+_amount_change);
}

// this function also indicates whether the item exists.
int Database::queryItemID(const QString &name)
{
	QSqlQuery query;
	query.prepare(query_items_id);
	query.bindValue(":NAME", name);
	query.exec();
	if(query.next()) {
		return query.value(0).toInt();
	} else {
		return -1;
	}
}

Database::Database(const QString &path)
{
	bool db_exist = QFile::exists(path);
	db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName(path);
	db.open();
	if(!db_exist) {
		QSqlQuery query;
		query.exec(create_table_records);
		query.exec(create_table_items);
	}
	sequence = new QUndoStack(this);
	connect(sequence, &QUndoStack::canUndoChanged, this, &Database::canUndoChanged);
	connect(sequence, &QUndoStack::canRedoChanged, this, &Database::canRedoChanged);
}

Database::~Database()
{
	db.close();
}

void Database::addItem(const QString &name, const int initial_number)
{
	QDateTime ts = QDateTime::currentDateTime();
//	addItemP(name, initial_number);
//	addRecordP(ts, name, initial_number, initial_number);
	UndoCommand *cmd = new UndoCommand(this, ts, name, 0, initial_number, true);
	sequence->push(cmd);
	emit databaseChanged();
}

void Database::changeItemNumber(const QString &name, const int &amount_change)
{
	int original_amount = queryItemNumberP(name);
	qDebug() << "original amount:" << original_amount;
	QDateTime ts = QDateTime::currentDateTime();
	int new_amount = original_amount + amount_change;
	qDebug() << "new amount:" << new_amount;

//	changeItemNumberP(name, new_amount);
//	addRecordP(ts, name, amount_change, new_amount);
	UndoCommand *cmd = new UndoCommand(this, ts, name, original_amount, amount_change, false);
	sequence->push(cmd);
	emit databaseChanged();
}

void Database::undo()
{
	sequence->undo();
	emit databaseChanged();
}

void Database::redo()
{
	sequence->redo();
	emit databaseChanged();
}

void Database::addItemP(const QString &name, const int initial_number)
{
	QSqlQuery query;
	query.prepare(insert_into_items);
	query.bindValue(":NAME", name);
	query.bindValue(":NUMBER", initial_number);
	query.exec();
}

void Database::delItemP(const QString &name)
{
	QSqlQuery query;
	query.prepare(delete_from_items);
	query.bindValue(":NAME", name);
	query.exec();
}

void Database::addRecordP(const QDateTime &ts, const QString &name, const int &amount_change, const int &after)
{
	int id = queryItemID(name);
	QSqlQuery query;
	query.prepare(insert_into_records);
	query.bindValue(":TIMESTAMP", ts.toString("YYYY-MM-DD HH:mm:ss"));
	query.bindValue(":ID", id);
	query.bindValue(":NUMBER", amount_change);
	query.bindValue(":AFTER", after);
	query.exec();
}

void Database::delRecordP(const QDateTime &ts)
{
	QSqlQuery query;
	query.prepare(delete_from_records);
	query.bindValue(":TIMESTAMP", ts.toString("YYYY-MM-DD HH:mm:ss"));
}

int Database::queryItemNumberP(const QString &name)
{
	QSqlQuery query;
	query.prepare(query_items_number);
	query.bindValue(":NAME", name);
	query.exec();
	if(query.next())
		return query.value(0).toInt();
	return -1;
}

void Database::changeItemNumberP(const QString &name, const int &new_amount)
{
	qDebug() << "changeItemNumberP(" << name << "," << new_amount << ");";
	QSqlQuery query;
	query.prepare(update_items_number);
	query.bindValue(":NUMBER", new_amount);
	query.bindValue(":NAME", name);
	query.exec();
}
