#ifndef TABLEVIEW_H
#define TABLEVIEW_H
#include <QTableWidget>
#include <QtSql/QSqlDatabase>
#include "database.h"

class TableView : public QTableWidget
{
	Q_OBJECT
public:
	TableView(Database *db, QWidget *parent = 0);
public slots:
	void showCustomMenu(const QPoint &pos);
	void popupShowHistoryDialog();
	void loadDatabase();
	void popupAddConsumeDialog();
private:
	void createContextMenu();
	QMenu *cmenu;
	Database *_db;
	QAction *add_or_consume_action;
	QAction *show_history_action;
};

#endif // TABLEVIEW_H
