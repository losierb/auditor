#include <QApplication>
#include <QIcon>
#include "mainwindow.h"
#include "dialogs.h"
#include "historyviewer.h"

static QString path = "tmp.db";

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
	db = new Database(path);
	table = new TableView(db, this);
	setCentralWidget(table);
	setMinimumSize(800, 600);
	createActions();
	createMenuBar();
	createToolBar();
	loadDatabase();
	connect(db, &Database::databaseChanged, table, &TableView::loadDatabase);
}

MainWindow::~MainWindow()
{
	delete table;
}

void MainWindow::popupAddItemDialog()
{
	NewItemDialog dialog(this);
	connect(&dialog, &NewItemDialog::commitNewItems, db, &Database::addItem);
	dialog.exec();
}

void MainWindow::popupHistoryDialog()
{
	HistoryViewer *viewer = new HistoryViewer(this);
	viewer->exec();
}

void MainWindow::popupFindDialog()
{
	FindItemsDialog *dialog = new FindItemsDialog(table, this);
	connect(dialog, &FindItemsDialog::scrollToNextMatchedItem, table, &TableView::scrollToItem);
	dialog->exec();
}

void MainWindow::loadDatabase()
{
	table->loadDatabase();
}

void MainWindow::createActions()
{
	quit_action = new QAction(QIcon::fromTheme("application-exit", QIcon(":/images/application-exit")), tr("&Quit"), this);
	add_item_action = new QAction(QIcon::fromTheme("list-add", QIcon(":/images/list-add")), tr("&Add Item"), this);
	undo_action = new QAction(QIcon::fromTheme("edit-undo", QIcon(":/images/edit-undo")), tr("&Undo"), this);
	redo_action = new QAction(QIcon::fromTheme("edit-redo", QIcon(":/images/edit-redo")), tr("&Redo"), this);
	find_action = new QAction(QIcon::fromTheme("edit-find", QIcon(":/images/edit-find")), tr("&Find"), this);
	refresh_action = new QAction(QIcon::fromTheme("view-refresh", QIcon(":/images/view-refresh")), tr("R&efresh"), this);
	about_qt_action = new QAction(tr("A&bout Qt"), this);
	show_history_action = new QAction(QIcon::fromTheme("view-history", QIcon(":/images/view-history")), tr("&History"), this);
	connect(quit_action, &QAction::triggered, qApp, QApplication::quit);
	connect(add_item_action, &QAction::triggered, this, &MainWindow::popupAddItemDialog);
	connect(show_history_action, &QAction::triggered, this, &MainWindow::popupHistoryDialog);
	connect(refresh_action, &QAction::triggered, table, &TableView::loadDatabase);
	connect(about_qt_action, &QAction::triggered, qApp, &QApplication::aboutQt);
	connect(undo_action, &QAction::triggered, db, &Database::undo);
	connect(redo_action, &QAction::triggered, db, &Database::redo);
	connect(find_action, &QAction::triggered, this, &MainWindow::popupFindDialog);
	undo_action->setDisabled(true);
	redo_action->setDisabled(true);
	connect(db, &Database::canUndoChanged, undo_action, &QAction::setEnabled);
	connect(db, &Database::canRedoChanged, redo_action, &QAction::setEnabled);
}

void MainWindow::createMenuBar()
{
	QMenu *file = menuBar()->addMenu(tr("&File"));
	file->addAction(quit_action);
	QMenu *edit = menuBar()->addMenu(tr("&Edit"));
	edit->addAction(undo_action);
	edit->addAction(redo_action);
	edit->addAction(find_action);
	edit->addAction(refresh_action);
	edit->addAction(show_history_action);
	QMenu *help = menuBar()->addMenu(tr("&Help"));
	help->addAction(about_qt_action);
}

void MainWindow::createToolBar()
{
	QToolBar *toolbar = addToolBar(tr("Main toolbar"));
	toolbar->setFloatable(false);
	toolbar->setMovable(false);
	toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	toolbar->addAction(add_item_action);
	toolbar->addAction(undo_action);
	toolbar->addAction(redo_action);
	toolbar->addAction(find_action);
	toolbar->addAction(refresh_action);
	toolbar->addAction(show_history_action);
	toolbar->addAction(quit_action);
}
