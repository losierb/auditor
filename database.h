#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql/QSqlDatabase>
#include <QObject>
#include <QVector>
#include <QDateTime>
#include <QUndoStack>

class Database: public QObject
{
	Q_OBJECT
public:
	Database(const QString &path);
	~Database();
	QSqlDatabase *getDatabase() { return &db; }
	void addItemP(const QString &name, const int initial_number);
	void delItemP(const QString &name);
	void addRecordP(const QDateTime &ts, const QString &name, const int &amount_change, const int &after);
	void delRecordP(const QDateTime &ts);
	int queryItemNumberP(const QString &name);
	void changeItemNumberP(const QString &name, const int &new_amount);
signals:
	void databaseChanged();
	void canRedoChanged(bool canRedo);
	void canUndoChanged(bool canUndo);

public slots:
	void addItem(const QString &name, const int initial_number);
	void changeItemNumber(const QString &name, const int &amount_change);
	void undo();
	void redo();
private:
	QSqlDatabase db;
	QUndoStack *sequence;

	int queryItemID(const QString &name);
};

#endif // DATABASE_H
