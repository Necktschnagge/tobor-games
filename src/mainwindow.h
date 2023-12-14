
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "gui_interactive_controller.h"


#include <QMainWindow>
#include <QSvgRenderer>
#include <QGraphicsSvgItem>
#include <QKeyEvent>
#include <QGraphicsScene>

#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE



class MainWindow : public QMainWindow
{
	Q_OBJECT

public:

	struct SvgViewToolchain {
		std::unique_ptr<QSvgRenderer> q_svg_renderer;
		std::unique_ptr<QGraphicsScene> q_graphics_scene;
		/*
		*/
		inline SvgViewToolchain& operator =(SvgViewToolchain&& another) {
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

private:
	Ui::MainWindow* ui;
	GuiInteractiveController guiInteractiveController;
	friend class GuiInteractiveController;

	SvgViewToolchain svgViewToolchain;

	void viewSvgInMainView(const QString& svg_string);

	inline void viewSvgInMainView(const std::string& svg_string) {
		return viewSvgInMainView(QString::fromStdString(svg_string));
	}


protected:
	void keyPressEvent(QKeyEvent* e);

};

#endif // MAINWINDOW_H
