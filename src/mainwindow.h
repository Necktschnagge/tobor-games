
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QXmlStreamReader>
#include <QSvgRenderer>
#include <QGraphicsSvgItem>
#include <QKeyEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow;

class GuiInteractiveController final {

	MainWindow* mainWindow;

	enum class InteractiveMode {
		NO_GAME,
		GAME_INTERACTIVE
	};

	InteractiveMode interactive_mode;

public:

	GuiInteractiveController(MainWindow* mainWindow) : mainWindow(mainWindow), interactive_mode(InteractiveMode::NO_GAME) {

	}

	void startGame();


	void stopGame();


};

class MainWindow : public QMainWindow

{
	Q_OBJECT

private:
	GuiInteractiveController guiInteractiveController;
	friend class GuiInteractiveController;

public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();

private slots:
	void on_actionshowSVG_triggered();

	void on_actionAbout_triggered();

	void on_actionNewGame_triggered();

	void on_actionStopGame_triggered();

private:
	Ui::MainWindow* ui;

protected:
	void keyPressEvent(QKeyEvent* e);
};

#endif // MAINWINDOW_H
