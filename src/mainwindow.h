
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "gui_interactive_controller.h"


#include <QMainWindow>
#include <QSvgRenderer>
#include <QGraphicsSvgItem>
#include <QKeyEvent>
#include <QGraphicsScene>
#include <QLabel>

#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE



class MainWindow : public QMainWindow
{
	Q_OBJECT

private:
	struct StatusbarItems {
		QLabel* stepsKey;
		QLabel* stepsValue;

		// add more labels: "BoardId:" "702563:378 // 3:0:2:3:5:2:7 " hint: redPlanetQuadrant, .., .., .., permutation, board rotation, target cell
		// add more labels: "Solver:" "RUNNING / OFF / AUTO-PLAY / STEP-MODE"

		void init(QWidget* parent, QStatusBar* statusbar);
	};

public:

	struct SvgViewToolchain {
		std::unique_ptr<QSvgRenderer> q_svg_renderer;
		std::unique_ptr<QGraphicsScene> q_graphics_scene;
		/*
		*/
		inline SvgViewToolchain& operator =(SvgViewToolchain&& another) noexcept {
			q_graphics_scene = std::move(another.q_graphics_scene);
			q_svg_renderer = std::move(another.q_svg_renderer);

			return *this;
		}

		inline ~SvgViewToolchain() {
			q_graphics_scene.reset();
			q_svg_renderer.reset();
		}
	};

	MainWindow(QWidget* parent = nullptr);
	~MainWindow();

private slots:
	void on_actionshowSVG_triggered();

	void on_actionAbout_triggered();

	void on_actionNewGame_triggered();

	void on_actionStopGame_triggered();

	void on_actionMoveBack_triggered();

	void on_actionTest_ListView_triggered();

	void on_actionRED_triggered();

	void on_actionYELLOW_triggered();

	void on_actionGREEN_triggered();

	void on_actionBLUE_triggered();

	void on_actionNORTH_triggered();

	void on_actionEAST_triggered();

	void on_actionSOUTH_triggered();

	void on_actionWEST_triggered();

	void setNumberOfSteps(QString& c);

private:
	Ui::MainWindow* ui;
	GuiInteractiveController guiInteractiveController;
	friend class GuiInteractiveController;

	SvgViewToolchain svgViewToolchain;

	StatusbarItems statusbarItems;

	void viewSvgInMainView(const QString& svg_string);

	inline void viewSvgInMainView(const std::string& svg_string) {
		return viewSvgInMainView(QString::fromStdString(svg_string));
	}


protected:
	void keyPressEvent(QKeyEvent* e);

};

#endif // MAINWINDOW_H
