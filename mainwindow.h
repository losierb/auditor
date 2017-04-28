#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>
#include "tableview.h"
#include "database.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();
public slots:
	void popupAddItemDialog();
	void popupHistoryDialog();
private:
	QAction *add_item_action;
	QAction *quit_action;
	QAction *undo_action;
	QAction *redo_action;
	QAction *find_action;
	QAction *refresh_action;
	QAction *show_history_action;
	TableView *table;
	Database *db;
	void loadDatabase();
	void createActions();
	void createMenuBar();
	void createToolBar();
};

#endif // MAINWINDOW_H
