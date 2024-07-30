#pragma once

#include "abstract_game_controller.h"

#include <utility> // std::pair


/**
*	@brief Base class for game factories.
*
*	@details Capabilities: Constructing derived classes of AbstractGameController, creating svg with target highlighting
*/
class AbstractGameFactory {
public:

	/**
	*	@brief Constructs some derivation of AbstractGameController. The caller takes ownership of the returned object.
	*/
	[[nodiscard]] virtual AbstractGameController* create() const = 0;

	/**
	*	@brief Clones this factory. The caller takes ownership of the returned object.
	*/
	[[nodiscard]] virtual AbstractGameFactory* clone() const = 0;

	/**
	*	@brief Returns an svg presenting a game board with target cells marked and the number of target cells.
	*/
	virtual std::pair<std::string, std::size_t> svg_highlighted_targets() const = 0;

	virtual ~AbstractGameFactory() {}
};
