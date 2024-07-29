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
	/**
	*	@brief Clones this factory. The caller takes ownership of the returned object.
	*/
	[[nodiscard]] virtual CyclicGroupGameFactory* clone() const override = 0;

	/** @brief Increments the underlying cyclic group counter. */
	virtual void increment() = 0;

	/** @brief Returns the group size of the world generator. */
	[[nodiscard]] virtual std::size_t world_generator_group_size() const = 0;

	/** @brief Returns the current counter for world generator group. */
	[[nodiscard]] virtual std::size_t get_world_generator_counter() const = 0;

	/** @brief Sets the current counter for world generator group. */
	virtual void set_world_generator_counter(std::size_t c) = 0;



	/** @brief Returns the group size of the state generator. */
	[[nodiscard]] virtual std::size_t state_generator_group_size() const = 0;

	/** @brief Returns the current counter for state generator group. */
	[[nodiscard]] virtual std::size_t get_state_generator_counter() const = 0;

	/** @brief Sets the current counter for state generator group. */
	virtual void set_state_generator_counter(std::size_t c) = 0;
};
