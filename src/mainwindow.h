
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "gui_interactive_controller.h"
#include "key_event_filter.h"



#include <QMainWindow>
#include <QSvgRenderer>
#include <QGraphicsSvgItem>
#include <QKeyEvent>
#include <QGraphicsScene>
#include <QLabel>
#include <QSignalMapper>
#include <QActionGroup> 

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


private:
	class StatusbarItems {
	public:

		static constexpr int QUADRATIC_COLOR_LABEL_SIZE{ 15 };

		QLabel* stepsKey;
		QLabel* stepsValue;

		QLabel* boardIdKey;
		QLabel* boardIdValue; // two modes? "702563:378 // 3:0:2:3:5:2:7 " hint: redPlanetQuadrant, .., .., .., permutation, board rotation, target cell

		QLabel* solverKey; // "RUNNING / OFF / AUTO-PLAY / STEP-MODE"
		QLabel* solverValue;

		QLabel* pieceSelectedKey; // "[colored square]" current selected piece's color
		QLabel* pieceSelectedValue;

		void init(QStatusBar* statusbar);

		void setSelectedPiece(const QColor& c);
	};

	class ShapeSelectionItems {
	public:

		QAction* ball;
		QAction* duck;
		QAction* swan;

		QActionGroup* group;

		void createInsideQMenu(MainWindow* mainWindow, QMenu* qMenu);

		QAction* getSelectedShape() const;
	};

public:

	MainWindow(QWidget* parent = nullptr);
	~MainWindow();

private slots:
	void on_actionshowSVG_triggered();

	void on_actionHighlightGeneratedTargetCells_triggered();

	void on_actionEnableAllMenuBarItems_triggered();

	void on_actionAbout_triggered();

	void on_actionNewGame_triggered();

	void on_actionStopGame_triggered();

	void on_actionMoveBack_triggered();

	void on_actionTest_ListView_triggered();

	void on_actionNORTH_triggered();

	void on_actionEAST_triggered();

	void on_actionSOUTH_triggered();

	void on_actionWEST_triggered();

	void on_actionForward_triggered();

	void on_actionBack_triggered();

	void on_actionStart_Solver_triggered();

	void on_actionStop_Solver_triggered();

	void on_actionLicense_Information_triggered();

	void on_listView_doubleClicked(const QModelIndex& index);

private:
	Ui::MainWindow* ui;
	GuiInteractiveController guiInteractiveController;
	friend class GuiInteractiveController;
	friend class ControlKeyEventAgent;

	SvgViewToolchain svgViewToolchain;

	StatusbarItems statusbarItems;

	ShapeSelectionItems shapeSelectionItems;

	std::vector<QMetaObject::Connection> inputConnections;

	QSignalMapper* signalMapper;

	ControlKeyEventAgent controlKeyEventAgent;

	void viewSvgInMainView(const QString& svg_string);

	inline void viewSvgInMainView(const std::string& svg_string) {
		return viewSvgInMainView(QString::fromStdString(svg_string));
	}

	QMenu* getSelectPieceSubMenu();

	void disconnectInputConnections() {
		for (QMetaObject::Connection& c : inputConnections) {
			QObject::disconnect(c);
		}
		inputConnections.clear();
	}

private slots:

	void selectPieceByColor(int index);

	void refreshAllInGuiInteractiveController();

};

#endif // MAINWINDOW_H
