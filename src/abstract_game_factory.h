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

	[[nodiscard]] virtual AbstractGameController* create() const = 0;

	[[nodiscard]] virtual AbstractGameFactory* clone() const = 0;


	virtual std::pair<std::string, std::size_t> svg_highlighted_targets() const = 0;

	virtual ~AbstractGameFactory() {}
};
