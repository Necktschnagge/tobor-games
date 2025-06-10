
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "key_event_filter.h"

#include "cyclic_group_game_factory.h"

#include <QMainWindow>
#include <QSvgRenderer>
#include <QGraphicsSvgItem>
#include <QKeyEvent>
#include <QGraphicsScene>
#include <QLabel>
#include <QSignalMapper>
#include <QActionGroup> 

#include <memory>
#include <random>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


namespace fsl {
	template<class _Underlying, class _Label>
	class labeled {
	public:
		using type = _Underlying;
		using label_type = _Label;

	private:
		type _value;
		label_type _label;

	public:

		constexpr labeled(const type& value, const label_type& label = label_type())
			: _value(value)
			, _label(label)
		{
		}

		constexpr labeled(const type& value, label_type&& label)
			: _value(value)
			, _label(std::move(label))
		{
		}
		constexpr labeled(type&& value, const label_type& label = label_type())
			: _value(std::move(value))
			, _label(label)
		{
		}

		constexpr labeled(type&& value, label_type&& label)
			: _value(std::move(value))
			, _label(std::move(label))
		{
		}

		constexpr labeled(const labeled&) = default;
		constexpr labeled(labeled&&) = default;
		constexpr labeled& operator=(const labeled&) = default;
		constexpr labeled& operator=(labeled&&) = default;

	public:
		type& value() noexcept {
			return _value;
		}

		const type& value() const noexcept {
			return value();
		}

		label_type& label() noexcept {
			return _label;
		}

		const label_type& label() const noexcept {
			return label();
		}

		~labeled() {}

	};

	template<class _Underlying, class _Label>
	inline labeled<std::decay_t<_Underlying>, std::decay_t<_Label>> make_labeled(_Underlying&& value, _Label&& label) {
		return labeled<std::decay_t<_Underlying>, std::decay_t<_Label>>(std::forward<_Underlying>(value), std::forward<_Label>(label));
	}
}


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

private:
	// concrete actions executed via slots or via direct calls

	// game
	void startGame(AbstractGameFactory* factory);
	void startGame();
private Q_SLOTS:
	void startGameFromHistory(int index);
	void selectFactoryByIndex(int index);
private:
	void startReferenceGame22();
	void stopGame();

	// solver
	void startSolver();
	void selectSolution(std::size_t index);
	void stopSolver();

	// game moves
	void selectPieceByColorId(const std::size_t& color_id);
	void movePiece(const tobor::v1_0::direction& direction);
	void undo();




private Q_SLOTS:

	void on_actionHighlightGeneratedTargetCells_triggered();
	void on_actionEnableAllMenuBarItems_triggered();
	void on_action22ReferenceGame_triggered();
	void on_actionAbout_triggered();
	void on_actionNewGame_triggered();
	void on_actionStopGame_triggered();
	void on_actionMoveBack_triggered();
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

	// Refresh UI Elements / Button ENABLE
	void setMenuButtonEnableForNoGame();
	void setMenuButtonEnableForInteractiveGame();
	void setMenuButtonEnableForSolverGame();

	void createColorActions();


	// Refresh UI Elements / Views
	void refreshSVG();
	void refreshNumberOfSteps();
	void refreshMenuButtonEnable();
	void refreshStatusbar();
	void refreshSolutionPaths();
	void refreshHistory();
	void refreshMenuFactorySelect();

	void highlightGeneratedTargetCells();

	void selectFactory(std::size_t index);

private:

	Ui::MainWindow* ui;

	friend class ControlKeyEventAgent;

	SvgViewToolchain svgViewToolchain;

	StatusbarItems statusbarItems;

	ShapeSelectionItems shapeSelectionItems;

	std::vector<QMetaObject::Connection> inputConnections;

	//std::vector<QMetaObject::Connection> historyConnections;

	QSignalMapper* signalMapper{ nullptr };

	QSignalMapper* historySignalMapper{ nullptr };

	QSignalMapper* factorySelectSignalMapper{ nullptr };

	ControlKeyEventAgent controlKeyEventAgent;

	void viewSvgInMainView(const QString& svg_string);

	inline void viewSvgInMainView(const std::string& svg_string) {
		return viewSvgInMainView(QString::fromStdString(svg_string));
	}

	void disconnectInputConnections() {
		for (QMetaObject::Connection& c : inputConnections) {
			QObject::disconnect(c);
		}
		inputConnections.clear();
	}

	std::shared_ptr<AbstractGameController> current_game; /// check all positions where used!!!! #######

	std::vector<std::shared_ptr<CyclicGroupGameFactory>> factory_history;

	std::vector<fsl::labeled<std::unique_ptr<CyclicGroupGameFactory>, std::string>> board_state_factories;

	std::size_t factory_select;

	std::mt19937 generator;

	tobor::v1_0::color_vector current_color_vector;


private Q_SLOTS:

	void selectPieceByColor(int index);

	void refreshAll();

};

#endif // MAINWINDOW_H
