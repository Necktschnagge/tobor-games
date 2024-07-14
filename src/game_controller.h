#pragma once

#include "solver_environment.h"

#include "abstract_game_controller.h"

#include "engine_typeset.h"

#include "color_generator.h"

#include <QString>

template<class Pieces_Quantity_T>
class GameController : public AbstractGameController {

public:

	using engine_typeset = ClassicEngineTypeSet<Pieces_Quantity_T>;

	using world_type = engine_typeset::world_type;
	using move_engine_type = engine_typeset::move_engine_type;
	using state_path_type_interactive = engine_typeset::state_path_type_interactive;
	using move_path_type = engine_typeset::move_path_type;
	using cell_id_type = engine_typeset::cell_id_type;
	using positions_of_pieces_type_interactive = engine_typeset::positions_of_pieces_type_interactive;
	using piece_id_type = engine_typeset::piece_id_type;


private:

	/* data */

	world_type _world;

	move_engine_type _move_engine;

	state_path_type_interactive _path;

	cell_id_type _target_cell;

	std::optional<SolverEnvironment<Pieces_Quantity_T>> _solver;

	std::size_t _solver_begin_index; // index of the first state where the solver moves to. == _path.vector().size() in case of solver-initial state

	std::size_t _solution_index;

public:

	GameController(
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
		_solution_index(0)
	{}



	/* non-modifying */

	virtual const positions_of_pieces_type_interactive& current_state() const { return _path.vector().back(); }

	virtual const positions_of_pieces_type_interactive& solver_begin_state() const {
		if (!_solver)
			return _path.vector().back();
		return _path.vector()[_solver_begin_index - 1];
	}

	virtual bool is_final() const { return current_state().is_final(_target_cell); }

	virtual bool is_initial() const { return _path.vector().size() == 1; }

	virtual const world_type& world() const { return _world; }

	virtual const cell_id_type& target_cell() const { return _target_cell; }

	virtual std::size_t depth() const { return _path.vector().size() - 1; }



	/* modifying */

	virtual uint8_t move_feedback(piece_id_type& piece_id, const tobor::v1_0::direction& direction) {
		if (_solver) return 4;

		if (is_final()) return 2;

		auto next_state = _move_engine.successor_state_feedback(current_state(), piece_id, direction);

		if (next_state == current_state()) return 1;

		_path += next_state;

		return 0;
	}

	virtual uint8_t move(const piece_id_type& piece_id, const tobor::v1_0::direction& direction) {
		if (_solver) return 4;

		if (is_final()) return 2;

		auto next_state = _move_engine.successor_state(current_state(), piece_id, direction);

		if (next_state == current_state()) return 1;

		_path += next_state;

		return 0;
	}

	virtual void undo() {
		if (_solver) return; // or stop solver if undoing out of solver (?)

		if (_path.vector().size() > 1) {
			_path.vector().pop_back();
		}
	}

	void start_solver(std::function<void(const std::string&)> status_callback = nullptr) {
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

	virtual state_path_type_interactive path() const noexcept {
		return _path;
	}

	inline auto optimal_solutions() const {
		if (_solver) {
			return _solver.value().optimal_solutions();
		}
		return SolverEnvironment::optimal_solutions_vector();
	}

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
			for (const GameController::piece_move_type& m : partitions[i].second.vector()) {
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

	~GameController() {}

	// todo: functions below:

	// must implement copy / move ctor and operator=
};

