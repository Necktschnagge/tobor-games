
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "gui_interactive_controller.h"


#include <QMainWindow>
#include <QXmlStreamReader>
#include <QSvgRenderer>
#include <QGraphicsSvgItem>
#include <QKeyEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();

private slots:
	void on_actionshowSVG_triggered();

	void on_actionAbout_triggered();

	void on_actionNewGame_triggered();

	void on_actionStopGame_triggered();

    void on_actionTest_ListView_triggered();

    void on_actionRED_triggered();

    void on_actionYELLOW_triggered();

    void on_actionGREEN_triggered();

    void on_actionBLUE_triggered();

    void on_actionNORTH_triggered();

    void on_actionEAST_triggered();

    void on_actionSOUTH_triggered();

    void on_actionWEST_triggered();

private:
	Ui::MainWindow* ui;
	GuiInteractiveController guiInteractiveController;
	friend class GuiInteractiveController;

protected:
	void keyPressEvent(QKeyEvent* e);
};

#endif // MAINWINDOW_H
