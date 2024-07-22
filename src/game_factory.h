#pragma once

#include "game_controller.h"
#include "world_generator_1_1.h"

#include <memory>

class AbstractGameFactory {

public:

	[[nodiscard]] virtual AbstractGameController* create() const = 0;

	virtual void increment() = 0;

	[[nodiscard]] virtual AbstractGameFactory* clone() const = 0;



	virtual std::size_t world_generator_group_size() const = 0;

	virtual void set_world_generator_counter(std::size_t c) = 0;

	virtual std::size_t state_generator_group_size() const = 0;

	virtual void set_state_generator_counter(std::size_t c) = 0;



	virtual ~AbstractGameFactory() {}

};


template<class Pieces_Quantity_Type>
class OriginalGameFactory : public AbstractGameFactory {
public:

	using world_type = tobor::v1_1::dynamic_rectangle_world<uint16_t, uint8_t>;
	using pieces_quantity_type = Pieces_Quantity_Type;
	using cell_id_type = tobor::v1_1::min_size_cell_id<world_type>;

	using positions_of_pieces_type_interactive = tobor::v1_1::augmented_positions_of_pieces<pieces_quantity_type, cell_id_type, true, true>;


	using board_generator_type = tobor::v1_1::world_generator::original_4_of_16;

	using state_generator_type = tobor::v1_1::world_generator::initial_state_generator<
		positions_of_pieces_type_interactive,
		256,
		pieces_quantity_type::COUNT_TARGET_PIECES,
		pieces_quantity_type::COUNT_NON_TARGET_PIECES,
		4>;

	using product_generator_type = tobor::v1_1::world_generator::product_group_generator<board_generator_type, state_generator_type>;

private:

	product_generator_type _product_generator;

public:

	OriginalGameFactory() : _product_generator() {}

	OriginalGameFactory(const OriginalGameFactory& another) = default;

	OriginalGameFactory(OriginalGameFactory&& another) = default;


	[[nodiscard]] virtual AbstractGameController* create() const override {

		auto world = _product_generator.main().get_tobor_world();

		std::vector<typename pieces_quantity_type::int_type> initial_color_permutation;

		for (typename pieces_quantity_type::int_type i = 0; i < pieces_quantity_type::COUNT_ALL_PIECES; ++i) { // build neutral permutation
			initial_color_permutation.push_back(i);
		}

		if constexpr (pieces_quantity_type::COUNT_ALL_PIECES == 4) {
			initial_color_permutation = _product_generator.main().obtain_standard_4_coloring_permutation(initial_color_permutation);
		}
		else {
			initial_color_permutation = _product_generator.main().template obtain_permutation<std::vector<typename pieces_quantity_type::int_type>, pieces_quantity_type::COUNT_ALL_PIECES>(initial_color_permutation);
		}

		return new GameController<pieces_quantity_type>(
			world,
			_product_generator.side().get_positions_of_pieces(world).apply_permutation(initial_color_permutation),
			_product_generator.main().get_target_cell()
		);
	}

	inline OriginalGameFactory& operator++() { ++_product_generator; return *this; }

	virtual void increment() override { ++(*this); }

	virtual AbstractGameFactory* clone() const override { return new OriginalGameFactory(*this); }

	inline product_generator_type& product_generator() { return _product_generator; }

	virtual std::size_t world_generator_group_size() const override {
		return board_generator_type::CYCLIC_GROUP_SIZE;
	}

	virtual void set_world_generator_counter(std::size_t c) override {
		_product_generator.main().set_counter(c);
	}

	virtual std::size_t state_generator_group_size() const override {
		return state_generator_type::CYCLIC_GROUP_SIZE;
	}

	virtual void set_state_generator_counter(std::size_t c) override {
		_product_generator.side().set_counter(c);
	}

};

class SpecialCaseGameFactory : public AbstractGameFactory {
public:
	using world_type = tobor::v1_1::dynamic_rectangle_world<uint16_t, uint8_t>;
	using cell_id_type = tobor::v1_1::min_size_cell_id<world_type>;

	using pieces_quantity_type = tobor::v1_1::pieces_quantity<uint8_t, 1, 3>;
	using positions_of_pieces_type_interactive = tobor::v1_1::augmented_positions_of_pieces<pieces_quantity_type, cell_id_type, true, true>;

	using game_controller_type = GameController<pieces_quantity_type>;

private:
	inline static world_type get22Game() {
		auto world = world_type(16, 16);
		world.block_center_cells(2, 2);

		world.west_wall_by_id(6) = true;
		world.west_wall_by_id(11) = true;
		world.west_wall_by_id(16 + 2) = true;
		world.west_wall_by_id(16 + 14) = true;
		world.west_wall_by_id(3 * 16 + 7) = true;
		world.west_wall_by_id(3 * 16 + 10) = true;
		world.west_wall_by_id(4 * 16 + 1) = true;
		world.west_wall_by_id(5 * 16 + 11) = true;
		world.west_wall_by_id(6 * 16 + 4) = true;
		world.west_wall_by_id(6 * 16 + 14) = true;
		world.west_wall_by_id(8 * 16 + 11) = true;
		world.west_wall_by_id(9 * 16 + 14) = true;
		world.west_wall_by_id(10 * 16 + 3) = true;
		world.west_wall_by_id(11 * 16 + 5) = true;
		world.west_wall_by_id(12 * 16 + 2) = true;
		world.west_wall_by_id(13 * 16 + 11) = true;
		world.west_wall_by_id(14 * 16 + 6) = true;
		world.west_wall_by_id(14 * 16 + 14) = true;
		world.west_wall_by_id(15 * 16 + 4) = true;
		world.west_wall_by_id(15 * 16 + 10) = true;

		world.south_wall_by_transposed_id(2) = true;
		world.south_wall_by_transposed_id(9) = true;
		world.south_wall_by_transposed_id(16 + 4) = true;
		world.south_wall_by_transposed_id(16 + 13) = true;
		world.south_wall_by_transposed_id(2 * 16 + 2) = true;
		world.south_wall_by_transposed_id(2 * 16 + 10) = true;
		world.south_wall_by_transposed_id(3 * 16 + 7) = true;
		world.south_wall_by_transposed_id(5 * 16 + 12) = true;
		world.south_wall_by_transposed_id(6 * 16 + 3) = true;
		world.south_wall_by_transposed_id(6 * 16 + 14) = true;
		world.south_wall_by_transposed_id(9 * 16 + 4) = true;
		world.south_wall_by_transposed_id(10 * 16 + 9) = true;
		world.south_wall_by_transposed_id(11 * 16 + 5) = true;
		world.south_wall_by_transposed_id(11 * 16 + 13) = true;
		world.south_wall_by_transposed_id(13 * 16 + 1) = true;
		world.south_wall_by_transposed_id(13 * 16 + 9) = true;
		world.south_wall_by_transposed_id(14 * 16 + 7) = true;
		world.south_wall_by_transposed_id(14 * 16 + 15) = true;
		world.south_wall_by_transposed_id(15 * 16 + 5) = true;
		world.south_wall_by_transposed_id(15 * 16 + 12) = true;

		return world;
	}


public:

	SpecialCaseGameFactory() {}

	SpecialCaseGameFactory(const SpecialCaseGameFactory&) = default;


	[[nodiscard]] virtual AbstractGameController* create() const override {

		world_type world{ get22Game() };

		cell_id_type target_cell{ cell_id_type::create_by_coordinates(9, 3, world) };

		positions_of_pieces_type_interactive initial_state = positions_of_pieces_type_interactive(
			{
				cell_id_type::create_by_coordinates(15, 15, world)
			},
			{
				cell_id_type::create_by_coordinates(1,0, world),
				cell_id_type::create_by_coordinates(12,14, world),
				cell_id_type::create_by_coordinates(12,15, world)
			}
		);

		return new game_controller_type(world, initial_state, target_cell);
	}

	virtual void increment() override {}

	[[nodiscard]] virtual AbstractGameFactory* clone() const override { return new SpecialCaseGameFactory(*this); }


	virtual std::size_t world_generator_group_size() const override { return 0; }

	virtual void set_world_generator_counter(std::size_t) override {}

	virtual std::size_t state_generator_group_size() const override { return 0; }

	virtual void set_state_generator_counter(std::size_t) override {}
};

