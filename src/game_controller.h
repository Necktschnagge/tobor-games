#pragma once

#include "solver_environment.h"

#include "abstract_game_controller.h"

#include "engine_typeset.h"

#include "color_generator.h"

#include "svg_1_1.h"

#include <QString>

template<class Pieces_Quantity_T>
class DRWGameController : public AbstractGameController {

public:

	using engine_typeset = ClassicEngineTypeSet<Pieces_Quantity_T>;

	using world_type = engine_typeset::world_type;
	using move_engine_type = engine_typeset::move_engine_type;
	using state_path_type_interactive = engine_typeset::state_path_type_interactive;
	using move_path_type = engine_typeset::move_path_type;
	using cell_id_type = engine_typeset::cell_id_type;
	using positions_of_pieces_type_interactive = engine_typeset::positions_of_pieces_type_interactive;
	using positions_of_pieces_type_solver = engine_typeset::positions_of_pieces_type_solver;
	using piece_id_type = engine_typeset::piece_id_type;
	using piece_move_type = engine_typeset::piece_move_type;
	using pieces_quantity_type = engine_typeset::pieces_quantity_type;

	using pieces_quantity_int_type = typename pieces_quantity_type::int_type;


	using graphics_type = tobor::v1_1::tobor_graphics<world_type, positions_of_pieces_type_solver>;
	using graphics_coloring_type = typename graphics_type::coloring;

	using solver_environment_type = SolverEnvironment<Pieces_Quantity_T>;

private:

	/* data */

	world_type _world;

	move_engine_type _move_engine;

	state_path_type_interactive _path;

	cell_id_type _target_cell;

	std::optional<solver_environment_type> _solver;

	std::size_t _solver_begin_index; // index of the first state where the solver moves to. == _path.vector().size() in case of solver-initial state

	std::size_t _solution_index;

	piece_id_type _selected_piece_id;

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



	/* non-modifying */

	const positions_of_pieces_type_interactive& current_state() const { return _path.vector().back(); }

	const positions_of_pieces_type_interactive& solver_begin_state() const {
		if (!_solver)
			return _path.vector().back();
		return _path.vector()[_solver_begin_index - 1];
	}

	virtual bool is_final() const { return current_state().is_final(_target_cell); }

	virtual bool is_initial() const { return _path.vector().size() == 1; }

	const world_type& world() const { return _world; }

	const cell_id_type& target_cell() const { return _target_cell; }

	virtual std::size_t depth() const { return _path.vector().size() - 1; }

	virtual std::size_t count_pieces() const { return pieces_quantity_type::COUNT_ALL_PIECES; }

	/* modifying */

	uint8_t move_feedback(piece_id_type& piece_id, const tobor::v1_0::direction& direction) {
		if (_solver) return 4;

		if (is_final()) return 2;

		auto next_state = _move_engine.successor_state_feedback(current_state(), piece_id, direction);

		if (next_state == current_state()) return 1;

		_path += next_state;

		return 0;
	}

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


	virtual void undo() {
		if (_solver) return; // or stop solver if undoing out of solver (?)

		if (_path.vector().size() > 1) {
			_path.vector().pop_back();
		}
	}

	virtual void start_solver(std::function<void(const std::string&)> status_callback = nullptr) override {
		_solver.emplace(current_state(), _target_cell, _move_engine, status_callback);

		_solver_begin_index = _path.vector().size();

		if (_solver.value().solutions_size() == 0) {
			// error dialog message!!!####
			return stop_solver();
		}

		if (status_callback) status_callback("Successfully executed solver.");
	}

	virtual void reset_solver_steps() {
		// go back to solver_begin_index
		_path.vector().erase(_path.vector().begin() + _solver_begin_index, _path.vector().end());
	}

	virtual void stop_solver() {
		_solver_begin_index = 0;
		_solver.reset();
	}

	virtual void select_solution(const std::size_t& index) {
		if (_solver) {
			_solution_index = index;
			reset_solver_steps();
		}
	}

	virtual void move_by_solver(bool forward) {
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

	state_path_type_interactive path() const noexcept {
		return _path;
	}

	inline typename solver_environment_type::optimal_solutions_vector optimal_solutions() const {
		if (_solver) {
			return _solver.value().optimal_solutions();
		}
		return solver_environment_type::optimal_solutions_vector();
	}

	// remove the QStringList here! ###
	virtual QStringList optimal_solutions_list(const tobor::v1_0::color_vector& current_color_vector) const // this has to be improved!!!
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
			qStringList << s;
		}

		return qStringList;
	}


	template<class T, pieces_quantity_type::int_type ... Index_Sequence>
	inline static graphics_coloring_type make_coloring(
		T& permutated_color_vector,
		std::integer_sequence<typename pieces_quantity_type::int_type, Index_Sequence...>
	) {
		auto coloring = graphics_coloring_type{
			(permutated_color_vector.colors[Index_Sequence].getSVGColorString()) ...
		};
		return coloring;
	}


	// should be moved outside the game controller. This is my interim solution
	virtual std::string svg(
		const tobor::v1_0::color_vector& current_color_vector,
		const tobor::v1_1::general_piece_shape_selection& shape = tobor::v1_1::general_piece_shape_selection::BALL
	) const override {

		auto permutated_color_vector = current_color_vector;

		for (std::size_t i{ 0 }; i < current_color_vector.colors.size(); ++i) {
			permutated_color_vector.colors[i] = current_color_vector.colors[current_state().permutation()[i]];
		}

		typename graphics_type::coloring coloring =
			make_coloring(
				permutated_color_vector,
				std::make_integer_sequence<DRWGameController::pieces_quantity_type::int_type, DRWGameController::pieces_quantity_type::COUNT_ALL_PIECES>{}
			);

		std::string example_svg_string =
			graphics_type::draw_tobor_world(
				this->world(),
				this->current_state().naked(),
				this->target_cell(),
				coloring,
				shape
			);

		return example_svg_string;
	}

	virtual std::string svg_highlighted_target_cells() const override {

		// _world

		// move this function to game factory!!!

		auto raw_cell_id_vector = dynamic_cast<OriginalGameFactory<DRWGameController::pieces_quantity_type>*>(factory_history.back().get())->product_generator().main().get_target_cell_id_vector(world);

		std::vector<DRWGameController::cell_id_type> comfort_cell_id_vector;

		std::transform(raw_cell_id_vector.cbegin(), raw_cell_id_vector.cend(), std::back_inserter(comfort_cell_id_vector),
			[&](const auto& raw_cell_id) {
				return DRWGameController::cell_id_type::create_by_id(raw_cell_id, world);
			}
		);

		std::string svg_string = graphics_type::draw_tobor_world_with_cell_markers(
			world,
			comfort_cell_id_vector
		);

		mainWindow->viewSvgInMainView(svg_string);

		QString m{ "Number of generator target cells:   " };
		m += QString::number(comfort_cell_id_vector.size());
		mainWindow->ui->statusbar->showMessage(m);
	}


	virtual bool select_piece_by_piece_id(const std::size_t& piece_id) override {
		if (_solver) return false;
		if (!(piece_id < pieces_quantity_type::COUNT_ALL_PIECES)) return false;
		_selected_piece_id.value = static_cast<decltype(_selected_piece_id.value)>(piece_id);
		return true;
	}


	virtual bool select_piece_by_color_id(const std::size_t& color_id) override {
		auto iter = std::find(
			current_state().permutation().cbegin(),
			current_state().permutation().cend(),
			color_id
		);

		if (iter == current_state().permutation().cend())
			return false;

		return select_piece_by_piece_id(iter - current_state().permutation().cbegin());

		//setPieceId(static_cast<DRWGameController::pieces_quantity_type::int_type>(iter - current_game->current_state().permutation().cbegin()));
	}

	virtual std::size_t selected_piece_id() const override {
		return _selected_piece_id.value;
	}

	virtual std::size_t selected_piece_color_id() const override {
		return current_state().permutation()[_selected_piece_id.value];
	}

	virtual ~DRWGameController() override {}

	// todo: functions below:

	// must implement copy / move ctor and operator=
};

