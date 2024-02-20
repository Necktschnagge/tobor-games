#pragma once

class GameController; // to be removed! ps_map is private, this is needed for friend class to work. Find another solution.

#include "solver.h" // ..., tobor::v1_0::tobor_world

#include "world_generator.h"
#include "color_generator.h"

// #include "tobor_svg.h" produces error


#include <QMainWindow>
#include <QXmlStreamReader>
#include <QSvgRenderer>
#include <QGraphicsSvgItem>
#include <QKeyEvent>
#include <QMessageBox>
#include <QDebug>
#include <QStyle>


#include <memory>
#include <random>
#include <thread>


class MainWindow;

/**
* @brief Keeps track of the path of states visited so far
*
*/
class GameController {
public:

	/* Types */

	using world_type = tobor::v1_0::tobor_world<uint16_t>;

	using cell_id_type = tobor::v1_0::universal_cell_id<world_type>;

	using positions_of_pieces_type = tobor::v1_0::positions_of_pieces<tobor::v1_0::default_pieces_quantity, cell_id_type, false, false>;

	using piece_move_type = tobor::v1_0::default_piece_move;

	using move_one_piece_calculator_type = tobor::v1_0::move_one_piece_calculator<positions_of_pieces_type, tobor::v1_0::quick_move_cache<world_type>, piece_move_type>;

	using state_graph_node_type = tobor::v1_0::state_graph_node<positions_of_pieces_type, piece_move_type>;

	using partial_state_graph_type = tobor::v1_0::partial_state_graph<move_one_piece_calculator_type, state_graph_node_type>;

	using move_path_type = tobor::v1_0::move_path<piece_move_type>;


private:
	friend class GuiInteractiveController; // remove this!!!

	/* data */

	world_type tobor_world;

	move_one_piece_calculator_type move_one_piece_calculator;



	std::vector<positions_of_pieces_type> path; // state - path

	cell_id_type target_cell;



	std::vector<positions_of_pieces_type>::size_type solver_begin_index;

	std::optional<partial_state_graph_type> optional_solver_state_graph;

	std::optional<
		std::map<
		positions_of_pieces_type,
		std::vector<
		std::vector<move_path_type>
		>
		>
	> optional_classified_move_paths;

	std::size_t selected_solution_index;

	std::vector<uint8_t> colorPermutation;

	static constexpr bool DEFAULT_DO_NOT_WAIT_ON_LAZY_FREE{
#ifdef _DEBUG 
		false
#else
		true
#endif
	};

	bool do_not_wait_on_lazy_free{ DEFAULT_DO_NOT_WAIT_ON_LAZY_FREE };

	std::shared_ptr<
		std::tuple<
		std::thread,
		decltype(optional_solver_state_graph),
		decltype(optional_classified_move_paths)
		>
	> lazy_free_worker;

	void lazyFreeSolverData() {

		if (lazy_free_worker && std::get<0>(*lazy_free_worker).joinable()) {
			do_not_wait_on_lazy_free = true;
			std::get<0>(*lazy_free_worker).join();
			do_not_wait_on_lazy_free = DEFAULT_DO_NOT_WAIT_ON_LAZY_FREE;
		}

		lazy_free_worker = std::make_shared<std::tuple<
			std::thread,
			decltype(optional_solver_state_graph),
			decltype(optional_classified_move_paths)
			>>();

		std::swap(std::get<1>(*lazy_free_worker), optional_solver_state_graph);

		std::swap(std::get<2>(*lazy_free_worker), optional_classified_move_paths);

		auto routine = [&](
			std::shared_ptr<
			std::tuple<
			std::thread,
			decltype(optional_solver_state_graph),
			decltype(optional_classified_move_paths)
			>
			> sptr) {

				auto& sink_optional_solver_state_graph{ std::get<1>(*sptr) };
				auto& sink_optional_classified_move_paths{ std::get<2>(*sptr) };

				if (sink_optional_solver_state_graph.has_value()) {
					while (!sink_optional_solver_state_graph.value().ps_map.empty()) {
						sink_optional_solver_state_graph.value().ps_map.erase(sink_optional_solver_state_graph.value().ps_map.begin());
						if (!do_not_wait_on_lazy_free) {
							std::this_thread::sleep_for(std::chrono::milliseconds(10));
						}
					}
				}

				if (sink_optional_classified_move_paths.has_value()) {
					while (!sink_optional_classified_move_paths.value().empty()) {
						sink_optional_classified_move_paths.value().erase(sink_optional_classified_move_paths.value().begin());
						if (!do_not_wait_on_lazy_free) {
							std::this_thread::sleep_for(std::chrono::milliseconds(10));
						}
					}
				}
			};

		std::get<0>(*lazy_free_worker) = std::thread(routine, lazy_free_worker);
	}

public:

	GameController(
		const world_type& tobor_world,
		const positions_of_pieces_type& initial_state,
		const cell_id_type& target_cell,
		const std::vector<uint8_t>& colorPermutation
	) :
		tobor_world(tobor_world),
		move_one_piece_calculator(this->tobor_world),
		path{ initial_state },
		target_cell(target_cell),
		solver_begin_index(0),
		optional_solver_state_graph(),
		optional_classified_move_paths(),
		selected_solution_index(0),
		colorPermutation(colorPermutation)
	{

	}



	/* non-modifying */

	const positions_of_pieces_type& currentState() const {
		return path.back();
	}

	inline bool isFinal() const {
		return currentState().is_final(target_cell);
	}

	inline bool isEmptyPath() const {
		return path.size() == 1;
	}



	/* modifying */

	inline void movePiece(const tobor::v1_0::default_piece_id& piece_id, const tobor::v1_0::direction& direction) {
		if (isFinal()) {
			return;
		}

		auto [next_state, valid] = move_one_piece_calculator.successor_state(currentState(), piece_id, direction);

		if (valid) {
			path.push_back(next_state);
		}

	}

	inline void undo() {
		if (path.size() > 1) {
			path.pop_back();
		}
	}


	move_path_type& get_selected_solution_representant(std::size_t index);

	void startSolver(QMainWindow* mw);

	void stopSolver() {

		lazyFreeSolverData();

		solver_begin_index = 0;

		//std::optional<partial_state_graph_type> graph_sink;

		//std::swap(optional_solver_state_graph, graph_sink); // for debug mode: need async map destruction

		optional_solver_state_graph.reset();
		optional_classified_move_paths.reset();
		selected_solution_index = 0;
	}

	void moveBySolver(bool forward);

	inline void selectSolution(const std::size_t& index) {
		selected_solution_index = index;
		// selects a solution from the list of solutions
	}

	inline void resetSolverSteps() {
		// go back to solver_begin_index
		path.erase(path.begin() + solver_begin_index, path.end());
	}

	~GameController() {
		do_not_wait_on_lazy_free = true;
		if (lazy_free_worker && std::get<0>(*lazy_free_worker).joinable()) {
			std::get<0>(*lazy_free_worker).join();
		}
	}

	// must implement copy / move ctor and operator=
};

class GuiInteractiveController final {

public:
	enum class InteractiveMode {
		NO_GAME,
		GAME_INTERACTIVE,
		SOLVER_INTERACTIVE_STEPS
	};

	using board_generator_type = tobor::v1_0::world_generator::original_4_of_16;

	using state_generator_type = tobor::v1_0::world_generator::initial_state_generator<GameController::positions_of_pieces_type, 256, 1, 3, 4>;

	using product_generator_type = tobor::v1_0::world_generator::product_group_generator<board_generator_type, state_generator_type>;

private:
	MainWindow* mainWindow;

	InteractiveMode interactive_mode;

	std::list<GameController> gameHistory;

	tobor::v1_0::default_piece_id selected_piece_id{ 0 };

	product_generator_type productWorldGenerator;

	std::mt19937 generator;


	//tobor::v1_0::tobor_graphics<GameController::positions_of_pieces_type>::coloring coloring = tobor::v1_0::tobor_graphics<GameController::positions_of_pieces_type>::coloring("red", "yellow", "green", "blue");
	// needs tobor svg include which brings errors...

public:

	tobor::v1_0::color_vector current_color_vector;

	void startReferenceGame22();

	GuiInteractiveController(MainWindow* mainWindow) :
		mainWindow(mainWindow),
		interactive_mode(InteractiveMode::NO_GAME),
		productWorldGenerator()
	{
		std::random_device rd;

		generator.seed(rd());

		std::uniform_int_distribution<uint64_t> distribution_on_uint64_board(0, board_generator_type::CYCLIC_GROUP_SIZE);
		std::uniform_int_distribution<uint64_t> distribution_on_uint64_pieces(0, product_generator_type::side_group_generator_type::CYCLIC_GROUP_SIZE);

		productWorldGenerator.main().set_counter(distribution_on_uint64_board(generator));
		productWorldGenerator.side().set_counter(distribution_on_uint64_pieces(generator));

		//originalGenerator.set_counter(distribution_on_uint64(generator));

		//originalGenerator.set_counter(73021); // 72972 73021

		//originalGenerator.set_generator(1);
		//originalGenerator.set_counter(3223);

		//productWorldGenerator.main().set_counter(73021);
	}

	inline InteractiveMode interactiveMode() const {
		return interactive_mode;
	}

	void startGame();

	void createColorActions();

	void stopGame();

	void moveBySolver(bool forward);

	void setPieceId(const tobor::v1_0::default_piece_id& piece_id);

	void selectPieceByColorId(const std::size_t& color_id);

	void refreshNumberOfSteps();

	void refreshSVG();

	void refreshMenuButtonEnable();

	void refreshStatusbar();

	inline void refreshAll() {
		refreshSVG();
		refreshStatusbar();
		refreshMenuButtonEnable();
		viewSolutionPaths();
	}

	void movePiece(const tobor::v1_0::direction& direction);

	void undo();

	void startSolver();

	void stopSolver();

	void selectSolution(std::size_t index);

	void viewSolutionPaths();

	void highlightGeneratedTargetCells();

};


