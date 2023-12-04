
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSvgRenderer>
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
			q_graphics_scene.release();
			q_svg_renderer.release();
		}
	};

	MainWindow(QWidget* parent = nullptr);
	~MainWindow();

private slots:
	void on_actionshowSVG_triggered();

	void on_actionAbout_triggered();

private:
	Ui::MainWindow* ui;

	SvgViewToolchain svgViewToolchain;

	void viewSvgInMainView(const QString& svg_string);
};

#endif // MAINWINDOW_H
