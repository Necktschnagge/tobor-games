#pragma once

#include "abstract_game_factory.h"

/**
*	@brief Base class for game factories with cyclic group semantics.
*
*	@details Capabilities: Constructing derived classes of AbstractGameController,
*	Incrementing the factory, Setting / getting group counters and getting group sizes.
*/
class CyclicGroupGameFactory : public AbstractGameFactory {
public:

	[[nodiscard]] virtual CyclicGroupGameFactory* clone() const override = 0;


	virtual void increment() = 0;

	virtual std::size_t world_generator_group_size() const = 0;

	virtual std::size_t get_world_generator_counter() const = 0;

	virtual void set_world_generator_counter(std::size_t c) = 0;



	virtual std::size_t state_generator_group_size() const = 0;

	virtual std::size_t get_state_generator_counter() const = 0;

	virtual void set_state_generator_counter(std::size_t c) = 0;
};
