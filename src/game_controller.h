#pragma once

#include "solver_environment.h"
#include "abstract_game_controller.h"

#include <QString>


/**
*	@brief Class for playing a game on a dynamic_rectangle_world, interactively and with solver.
*
*	@details States: The game is either in interactive mode or in solver mode.
*	This controller is restricted to one specifc world with a specific initial state and target cell, all set on construction.
*/
template<class Pieces_Quantity_T>
class DRWGameController : public AbstractGameController {

public:

	using engine_typeset = ClassicEngineTypeSet<Pieces_Quantity_T>;

	using world_type = typename engine_typeset::world_type;
	using move_engine_type = typename engine_typeset::move_engine_type;
	using state_path_type_interactive = typename engine_typeset::state_path_type_interactive;
	using move_path_type = typename engine_typeset::move_path_type;
	using cell_id_type = typename engine_typeset::cell_id_type;
	using positions_of_pieces_type_interactive = typename engine_typeset::positions_of_pieces_type_interactive;
	using positions_of_pieces_type_solver = typename engine_typeset::positions_of_pieces_type_solver;
	using piece_id_type = typename engine_typeset::piece_id_type;
	using piece_move_type = typename engine_typeset::piece_move_type;
	using pieces_quantity_type = typename engine_typeset::pieces_quantity_type;

	using pieces_quantity_int_type = typename pieces_quantity_type::int_type;


	using graphics_type = tobor::latest::svggen::board_composer<world_type, positions_of_pieces_type_solver>;
	using graphics_coloring_type = typename graphics_type::coloring;

	using solver_environment_type = SolverEnvironment<Pieces_Quantity_T>;

	using solver_optimal_solutions_vector = typename solver_environment_type::optimal_solutions_vector;
private:

	/* data */

	world_type _world;

	move_engine_type _move_engine;

	/**
	*	the path to current state in the game.
	*/
	state_path_type_interactive _path;

	cell_id_type _target_cell;

	std::optional<solver_environment_type> _solver;

	/**
	*	@brief Index of the first state where the solver moves to.
	*	Or _solver_begin_index == _path.vector().size() in case of solver-initial state
	*	
	*	@details Must be zero whenever _solver.has_value() == false, for canonicity.
	*/
	std::size_t _solver_begin_index;

	/**
	*	@brief Index of the selected solution.
	*
	*	@details Must be zero whenever _solver.has_value() == false, for canonicity.
	*/
	std::size_t _solution_index;

	piece_id_type _selected_piece_id;

	/**
	*	@brief Extracts coloring by applying a given permutation as std::integer_sequence and extracting SVGColorString
	*/
	template<class T, pieces_quantity_int_type ... Index_Sequence>
	inline static graphics_coloring_type make_coloring(
		T& permutated_color_vector,
		std::integer_sequence<pieces_quantity_int_type, Index_Sequence...>
	) {
		auto coloring = graphics_coloring_type{
			(permutated_color_vector.colors[Index_Sequence].getSVGColorString()) ...
		};
		return coloring;
	}


public:

	DRWGameController(
		const world_type& world,
		const positions_of_pieces_type_interactive& initial_state,
		const cell_id_type& target_cell
	) :
		_world(world),
		_move_engine(this->_world),
		_path({ initial_state }),
		_target_cell(target_cell),
		_solver(),
		_solver_begin_index(0),
		_solution_index(0),
		_selected_piece_id(0)
	{}



	/* non-modifying ********************************************************************************************/

	/**
	*	@brief Returns the current state of the game.
	*/
	const positions_of_pieces_type_interactive& current_state() const { return _path.vector().back(); }

	/**
	*	@brief Returns the state where the game was before the solver made any steps forward.
	*	@brief Returns current_state() when not in solver mode.
	*/
	const positions_of_pieces_type_interactive& solver_begin_state() const {
		if (!_solver)
			return _path.vector().back();
		return _path.vector()[_solver_begin_index - 1];
	}

	virtual bool is_final() const override { return current_state().is_final(_target_cell); }

	virtual bool is_initial() const override { return _path.vector().size() == 1; }

	/**
	*	@brief Returns a const reference to underlying world.
	*/
	const world_type& world() const { return _world; }

	/**
	*	@brief Returns the target cell.
	*/
	const cell_id_type& target_cell() const { return _target_cell; }

	virtual std::size_t depth() const override { return _path.vector().size() - 1; }

	virtual std::size_t count_pieces() const override { return pieces_quantity_type::COUNT_ALL_PIECES; }

	/* modifying ***********************************************************************************************+*/

	/**
	*	@brief Moves given piece in given direction.
	*	Parameter piece_id may be changed so that it points to the same piece after the move, especially if state changes piece order.
	*/
	uint8_t move_feedback(piece_id_type& piece_id, const tobor::v1_0::direction& direction) {
		if (_solver) return 4;

		if (is_final()) return 2;

		auto next_state = _move_engine.successor_state_feedback(current_state(), piece_id, direction);

		if (next_state == current_state()) return 1;

		_path += next_state;

		return 0;
	}
	
	/**
	*	@brief Moves given piece in given direction.
	*	@details Note that after this move \p piece_id might point to another piece because of piece reordering. Consider using move_feedback(...) instead.
	*/
	uint8_t move(const piece_id_type& piece_id, const tobor::v1_0::direction& direction) {
		if (_solver) return 4;

		if (is_final()) return 2;

		auto next_state = _move_engine.successor_state(current_state(), piece_id, direction);

		if (next_state == current_state()) return 1;

		_path += next_state;

		return 0;
	}

	virtual uint8_t move_selected(const tobor::v1_0::direction& direction) override {
		return move_feedback(_selected_piece_id, direction);
	}


	virtual void undo() override {
		if (_solver) return; // or stop solver if undoing out of solver (?)

		if (_path.vector().size() > 1) {
			_path.vector().pop_back();
		}
	}

	virtual uint8_t start_solver(std::function<void(const std::string&)> status_callback = nullptr) override {
		if (_solver) return 2;

		_solver.emplace(current_state(), _target_cell, _move_engine, status_callback);
		_solver_begin_index = _path.vector().size();

		if (_solver.value().solutions_size() == 0) {
			if (status_callback) status_callback("Solver terminated but did not find any solution.");
			stop_solver();
			return 1;
		}
		_solution_index = 0;
		if (status_callback) status_callback("Successfully executed solver.");
		return 0;
	}

	virtual void reset_solver_steps() override {
		// go back to solver_begin_index

		// could change this behavior into reset_steps(), resetting to initial state if not in solver mode. ###
		if (!_solver) return;
		_path.vector().erase(_path.vector().begin() + _solver_begin_index, _path.vector().end());
	}

	virtual void stop_solver() override {
		_solver_begin_index = 0;
		_solution_index = 0;
		_solver.reset();
	}

	virtual void select_solution(const std::size_t& index) override {
		if (_solver) {
			_solution_index = index;
			reset_solver_steps();
		}
	}

	virtual void move_by_solver(bool forward) override {
		if (!_solver) return;

		const auto index_next_move = _path.vector().size() - _solver_begin_index;

		const auto index_next_state = index_next_move + 1;

		if (forward) {
			if (is_final())
				return;
			_path += _solver.value().get_solution_state_path(_solution_index).vector()[index_next_state];
		}
		else { // back
			if (index_next_move == 0) // already at solver start
				return;
			_path.vector().pop_back();
		}
	}

	[[nodiscard]] virtual bool solver() const override {
		return _solver.has_value();
	}

	/**
	*	@brief Returns the current path.
	*/
	state_path_type_interactive path() const noexcept {
		return _path;
	}

	/**
	*	@brief Returns a vector of all optimal solutions.
	*/
	inline solver_optimal_solutions_vector optimal_solutions() const {
		if (_solver) {
			return _solver.value().optimal_solutions();
		}
		return solver_environment_type::optimal_solutions_vector();
	}

	// remove the QStringList here! ###
	virtual QStringList optimal_solutions_list(const tobor::v1_1::color_vector& current_color_vector) const override // this has to be improved!!!
	{
		if (!_solver) {
			return QStringList();
		}

		QStringList qStringList;

		const std::vector<std::pair<state_path_type_interactive, move_path_type>> partitions{ _solver.value().optimal_solutions() };

		for (std::size_t i{ 0 }; i < partitions.size(); ++i) {
			QString s;
			s = s + "[" + QString::number(i) + "]     ";
			for (const piece_move_type& m : partitions[i].second.vector()) {
				//is not checked for emptiness!!

				const char letter{ current_color_vector.colors[m.pid.value].UPPERCASE_shortcut_letter() };

				std::string color = std::string(1, letter);

				s = s + "  " + QString::fromStdString(color) + QString::fromStdString(static_cast<std::string>(m.dir));

			}
			s = s + "     ( NO COUNT " + /*QString::number(partitions[i].size()) + */ ")";
			// add counting or remove NO COUNT #####
			qStringList << s;
		}

		return qStringList;
	}

	// should be moved outside the game controller. This is my interim solution
	virtual std::string svg(
		const tobor::v1_1::color_vector& current_color_vector,
		const tobor::latest::svggen::general_piece_shape_selection& shape
	) const override {

		auto permutated_color_vector = current_color_vector;

		for (std::size_t i{ 0 }; i < current_color_vector.colors.size(); ++i) {
			permutated_color_vector.colors[i] = current_color_vector.colors[current_state().permutation()[i]];
		}

		typename graphics_type::coloring coloring =
			make_coloring(
				permutated_color_vector,
				std::make_integer_sequence<pieces_quantity_int_type, pieces_quantity_type::COUNT_ALL_PIECES>{}
			);

		std::string example_svg_string =
			graphics_type::draw_source_board(
				this->world(),
				this->current_state().naked(),
				this->target_cell(),
				coloring,
				shape
			);

		return example_svg_string;
	}

	virtual uint8_t select_piece_by_piece_id(const std::size_t& piece_id) override {
		if (_solver) return 2;
		if (!(piece_id < pieces_quantity_type::COUNT_ALL_PIECES)) return 1;
		_selected_piece_id.value = static_cast<decltype(_selected_piece_id.value)>(piece_id);
		return 0;
	}


	virtual uint8_t select_piece_by_color_id(const std::size_t& color_id) override {
		auto iter = std::find(
			current_state().permutation().cbegin(),
			current_state().permutation().cend(),
			color_id
		);

		if (iter == current_state().permutation().cend())
			return 4;

		return select_piece_by_piece_id(iter - current_state().permutation().cbegin());
	}

	virtual std::size_t selected_piece_id() const override {
		return _selected_piece_id.value;
	}

	virtual std::size_t selected_piece_color_id() const override {
		return current_state().permutation()[_selected_piece_id.value];
	}

	virtual ~DRWGameController() override {}

	// todo: functions below:

	// must implement copy / move ctor and operator= ####
};

