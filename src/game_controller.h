#pragma once

#include "solver_environment.h"


class GameController : public EngineTypeSet {

private:

	/* data */

	world_type _world;

	move_one_piece_calculator_type _move_engine;

	state_path_type_interactive _path;

	//std::vector<uint8_t> _color_permutation; // not needed because of augmented state type

	cell_id_type _target_cell;

	std::optional<SolverEnvironment> _solver;

	std::size_t _solver_begin_index;

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

	/*
	GameController(
		const world_type& world,
		const positions_of_pieces_type_interactive& initial_state,
		const cell_id_type& target_cell,
		const std::vector<uint8_t>& color_permutation
	) :
		GameController(world,initial_state.add_permutation(color_permutation), target_cell)
	{}
	*/



	/* non-modifying */

	const positions_of_pieces_type_interactive& current_state() const { return _path.vector().back(); }

	inline bool is_final() const { return current_state().is_final(_target_cell); }

	inline bool is_initial() const { return _path.vector().size() == 1; }

	//inline const std::vector<uint8_t>& color_permutation() const { return _color_permutation; }

	inline const world_type& world() const { return _world; }

	inline const cell_id_type& target_cell() const { return _target_cell; }

	inline std::size_t depth() const { return _path.vector().size() - 1; }

	/* modifying */

	inline uint8_t move_feedback(piece_id_type& piece_id, const tobor::v1_0::direction& direction) {
		if (_solver) return 4;

		if (is_final()) return 2;

		auto next_state = _move_engine.successor_state_feedback(current_state(), piece_id, direction);

		if (next_state == current_state()) return 1;

		_path += next_state;

		return 0;
	}

	inline uint8_t move(const piece_id_type& piece_id, const tobor::v1_0::direction& direction) {
		if (_solver) return 4;

		if (is_final()) return 2;

		auto next_state = _move_engine.successor_state(current_state(), piece_id, direction);

		if (next_state == current_state()) return 1;

		_path += next_state;

		return 0;
	}

	inline void undo() {
		if (_solver) return; // or stop solver if undoing out of solver (?)

		if (_path.vector().size() > 1) {
			_path.vector().pop_back();
		}
	}

	void start_solver(std::function<void(const std::string&)> status_callback = nullptr);

	inline void reset_solver_steps() {
		// go back to solver_begin_index
		_path.vector().erase(_path.vector().begin() + _solver_begin_index, _path.vector().end());
	}

	void stop_solver() {
		_solver_begin_index = 0;
		_solver.reset();
	}

	inline void select_solution(const std::size_t& index) {
		if (_solver) {
			_solution_index = index;
			reset_solver_steps();
		}
	}

	void move_by_solver(bool forward);

	inline state_path_type_interactive path() const noexcept {
		return _path;
	}

	inline auto optimal_solutions() const {
		if (_solver) {
			return _solver.value().optimal_solutions();
		}
		return SolverEnvironment::optimal_solutions_vector();
	}

	~GameController() {}

	// todo: functions below:

	// must implement copy / move ctor and operator=
};

