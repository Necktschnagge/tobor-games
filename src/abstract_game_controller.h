#pragma once

#include "color_generator.h"

#include <QStringList>

#include <string>
#include <functional>


class AbstractGameController {

	// use this class as interface 

public:

	AbstractGameController() {}

	/* non-modifying */

	//virtual const AbstractGameState& current_state() const = 0;   maybe added to this interface

	virtual bool is_final() const = 0;

	virtual bool is_initial() const = 0;

	//virtual const abstract_world_type& world() const { return _world; }   maybe added to this interface

	//virtual const cell_id_type& target_cell() const { return _target_cell; }   maybe added to this interface

	virtual std::size_t depth() const = 0;

	virtual std::size_t count_pieces() const = 0;


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


	virtual std::string svg(const tobor::v1_0::color_vector& current_color_vector) const = 0;

	virtual bool select_piece_by_piece_id(const std::size_t& piece_id) = 0;

	virtual bool select_piece_by_color_id(const std::size_t& color_id) = 0;


	virtual ~AbstractGameController() {}
};
