#pragma once

#include "game_controller.h"
#include "world_generator.h"

#include <memory>

class GameFactory {

public:

	[[nodiscard]] virtual std::shared_ptr<GameController> create() const = 0;

};


template<class Piece_Quantity_Type>
class OriginalGameFactory : public GameFactory {
public:

	using world_type = tobor::v1_1::dynamic_rectangle_world<uint16_t, uint8_t>;
	using piece_quantity_type = Piece_Quantity_Type;
	using cell_id_type = tobor::v1_1::min_size_cell_id<world_type>;

	using positions_of_pieces_type_interactive = tobor::v1_1::augmented_positions_of_pieces<piece_quantity_type, cell_id_type, true, true>;


	using board_generator_type = tobor::v1_1::world_generator::original_4_of_16;

	using state_generator_type = tobor::v1_1::world_generator::initial_state_generator<
		positions_of_pieces_type_interactive,
		256,
		piece_quantity_type::COUNT_TARGET_PIECES,
		piece_quantity_type::COUNT_NON_TARGET_PIECES,
		4>;

	using product_generator_type = tobor::v1_1::world_generator::product_group_generator<board_generator_type, state_generator_type>;

private:

	product_generator_type _product_generator;

public:

	OriginalGameFactory() : _product_generator() {}

	OriginalGameFactory(const OriginalGameFactory& another) = default;

	OriginalGameFactory(OriginalGameFactory&& another) = default;


	[[nodiscard]] virtual std::shared_ptr<GameController> create() const override {

		auto world = _product_generator.main().get_tobor_world();

		std::vector<typename piece_quantity_type::int_type> initial_color_permutation;

		for (typename piece_quantity_type::int_type i = 0; i < piece_quantity_type::COUNT_ALL_PIECES; ++i) { // build neutral permutation
			initial_color_permutation.push_back(i);
		}

		if constexpr (piece_quantity_type::COUNT_ALL_PIECES == 4) {
			initial_color_permutation = _product_generator.main().obtain_standard_4_coloring_permutation(initial_color_permutation);
		}
		else {
			initial_color_permutation = _product_generator.main().obtain_permutation<std::vector<piece_quantity_type::int_type>, piece_quantity_type::COUNT_ALL_PIECES>(initial_color_permutation);
		}

		return std::make_shared<GameController>(
			world,
			_product_generator.side().get_positions_of_pieces(world).apply_permutation(initial_color_permutation),
			_product_generator.main().get_target_cell()
		);
	}


	inline OriginalGameFactory& operator++() { ++_product_generator; return *this; }

	inline product_generator_type& product_generator() { return _product_generator; }

};

class SpecialCaseGameFactory : public GameFactory {
public:
	using world_type = tobor::v1_1::dynamic_rectangle_world<uint16_t, uint8_t>;
	using cell_id_type = tobor::v1_1::min_size_cell_id<world_type>;

	using piece_quantity_type = tobor::v1_1::pieces_quantity<uint8_t, 1, 3>;
	using positions_of_pieces_type_interactive = tobor::v1_1::augmented_positions_of_pieces<piece_quantity_type, cell_id_type, true, true>;


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


	[[nodiscard]] virtual std::shared_ptr<GameController> create() const override {

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

		return std::make_shared<GameController>(world, initial_state, target_cell);
	}

};

