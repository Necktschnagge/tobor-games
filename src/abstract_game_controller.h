#pragma once

#include "color_generator.h"

#include <QStringList>

#include <string>
#include <functional>


class AbstractGameController {

	// use this class as interface 

	AbstractGameController() {}

public:

	/* non-modifying */

	//virtual const positions_of_pieces_type_interactive& current_state() const = 0;

	//virtual const positions_of_pieces_type_interactive& solver_begin_state() const = 0;

	virtual bool is_final() const = 0;

	virtual bool is_initial() const = 0;

	//virtual const world_type& world() const { return _world; }

	//virtual const cell_id_type& target_cell() const { return _target_cell; }

	virtual std::size_t depth() const = 0;


	/* modifying */

	//virtual uint8_t move_feedback(piece_id_type& piece_id, const tobor::v1_0::direction& direction) = 0;
	//
	//virtual uint8_t move(const piece_id_type& piece_id, const tobor::v1_0::direction& direction) = 0;

	virtual void undo() = 0;

	virtual void start_solver(std::function<void(const std::string&)> status_callback) = 0;

	virtual void reset_solver_steps() = 0;

	virtual void stop_solver() = 0;

	virtual void select_solution(const std::size_t& index) = 0;

	virtual void move_by_solver(bool forward) = 0;

	//virtual state_path_type_interactive path() const noexcept = 0;

	//virtual auto optimal_solutions() const = 0;

	virtual QStringList optimal_solutions_list(const tobor::v1_0::color_vector& current_color_vector) const = 0;

	virtual ~AbstractGameController() {}
};
