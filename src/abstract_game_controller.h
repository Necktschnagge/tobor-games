#pragma once

#include "color_generator.h"
#include "svg_1_1.h"

#include <QStringList>

#include <functional>
#include <string>

/**
 *	@brief Interface for playing a game interactively and with solver.
 *
 *	@details States: The game is either in interactive mode or in solver mode.
 */
class AbstractGameController {

	// use this class as interface

public:
	AbstractGameController() {}

	/************************************************************************************************************* non-modifying */

	// virtual const AbstractGameState& current_state() const = 0;   maybe added to this interface

	/**
	 *	@brief Returns true if and only if the game is in final state.
	 */
	virtual bool is_final() const = 0;

	/**
	 *	@brief Returns true if and only if the game is in initial state.
	 */
	virtual bool is_initial() const = 0;

	// virtual const abstract_world_type& world() const { return _world; }   maybe added to this interface

	// virtual const cell_id_type& target_cell() const { return _target_cell; }   maybe added to this interface

	/**
	 *	@brief Returns the number of steps made since initial state. is_initial() == true if and only if depth() == 0
	 */
	virtual std::size_t depth() const = 0;

	/**
	 *	@brief Returns the total number of pieces used in this game.
	 */
	virtual std::size_t count_pieces() const = 0;

	/**
	 *	@brief Returns true if and only if game is in solver mode
	 */
	[[nodiscard]] virtual bool solver() const = 0;

	/**
	 *	@brief Returns the list of optimal solutions as string representation of move paths.
	 *	@brief Returns empty list if not in solver mode.
	 */
	virtual QStringList optimal_solutions_list(const tobor::v1_1::color_vector& current_color_vector) const = 0;
	// ### exchange the QStringList here. Should not use Qt classes, note color_vector also uses Qt. Decide about some additional abstraction.

	/*************************************************************************************************************** modifying */

	// virtual uint8_t move_feedback(piece_id_type& piece_id, const tobor::v1_0::direction& direction) = 0;   maybe added to this interface

	// virtual uint8_t move(const piece_id_type& piece_id, const tobor::v1_0::direction& direction) = 0;   maybe added to this interface

	/**
	 *	@brief Moves the previously selected piece in given direction \p direction, only in interactive mode.
	 *
	 *	@details In solver mode, returns 4 and does nothing.
	 *	In interactive mode and final state, returns 2 and does nothing.
	 *	In non-final state, interactive mode returns 1 in case it cannot move in given direction since the next obstacle is next to the selected piece.
	 *	Otherwise returns 0 for OK
	 */
	virtual uint8_t move_selected(const tobor::v1_1::direction& direction) = 0;

	/**
	 *	@brief Makes a step back, unless is_initial() or in solver mode. In the latter case it is a no-op.
	 */
	virtual void undo() = 0;

	/**
	 *	@brief Starts the solver and enters solver mode. In case of any error it remains in interactive mode.
	 *
	 *	@return Returns 2 in case already in solver mode, does nothing in this case.
	 *	Returns 1 in case the solver terminated but did not find any solution.
	 *	Returns 0 if successful, switched from interactive mode to solver mode.
	 */
	virtual uint8_t start_solver(std::function<void(const std::string&)> status_callback) = 0;

	/**
	 *	@brief Resets game state to the state where the solver has been started. Does nothing if not in solver mode.
	 */
	virtual void reset_solver_steps() = 0;

	/**
	 *	@brief Stops the solver mode, frees the solver's memory, switches to interactive mode but keeps the game's current state.
	 *	Does nothing if not in solver mode.
	 */
	virtual void stop_solver() = 0;

	/**
	 *	@brief Selects a solution by given index \p index and resets the game's state to the state where the solver was started.
	 *	Does nothing if not in solver mode.
	 */
	virtual void select_solution(const std::size_t& index) = 0;

	/**
	 *	@brief Makes a step according to the selected solution, either forward if \p forward is true or backwards otherwise.
	 *	@details Does nothing if not in solver mode.
	 *	Does nothing if trying to move forward in final state or backward in initial state.
	 */
	virtual void move_by_solver(bool forward) = 0;

	// virtual state_path_type_interactive path() const noexcept = 0;   maybe added to this interface

	// virtual auto optimal_solutions() const = 0;   maybe added to this interface

	/**
	 *	@brief Returns an svg of the game's current state.
	 */
	virtual std::string svg(const tobor::v1_1::color_vector& current_color_vector, const tobor::v1_1::general_piece_shape_selection& shape = tobor::v1_1::general_piece_shape_selection::BALL) const = 0;

	/**
	 *	@brief Selects a piece by its id.
	 *	@return Returns 2 in solver mode and does nothing
	 *	Returns 1 if in interactive mode but \p piece_id out of range.
	 *	Returns 0 on success.
	 */
	virtual uint8_t select_piece_by_piece_id(const std::size_t& piece_id) = 0;

	/**
	 *	@brief Selects a piece by its color id.
	 *	@return Returns 4 if color not found. Otherwise it returns according to select_piece_by_piece_id(const std::size_t&)
	 */
	virtual uint8_t select_piece_by_color_id(const std::size_t& color_id) = 0;

	/**
	 *	@brief Returns the piece id of the piece selected.
	 */
	virtual std::size_t selected_piece_id() const = 0;

	/**
	 *	@brief Returns the color id of the piece selected.
	 */
	virtual std::size_t selected_piece_color_id() const = 0;

	virtual ~AbstractGameController() {}
};
