#pragma once

#include "cyclic_group_game_factory.h"
#include "game_controller.h"

/**
*	@brief Factory to create a special game with a 22 step - optimal solution.
*/
class SpecialCaseGameFactory : public CyclicGroupGameFactory {
public:
	using world_type = tobor::v1_1::dynamic_rectangle_world<uint16_t, uint8_t>;
	using cell_id_type = tobor::v1_1::min_size_cell_id<world_type>;

	using pieces_quantity_type = tobor::v1_1::pieces_quantity<uint8_t, 1, 3>;
	using positions_of_pieces_type_interactive = tobor::v1_1::augmented_positions_of_pieces<pieces_quantity_type, cell_id_type, true, true>;

	using game_controller_type = DRWGameController<pieces_quantity_type>;

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

	[[nodiscard]] virtual SpecialCaseGameFactory* clone() const override { return new SpecialCaseGameFactory(*this); }


	virtual std::size_t world_generator_group_size() const override { return 0; }

	virtual std::size_t get_world_generator_counter() const override { return 0; }

	virtual void set_world_generator_counter(std::size_t) override {}

	virtual std::size_t state_generator_group_size() const override { return 0; }

	virtual std::size_t get_state_generator_counter() const override { return 0; }

	virtual void set_state_generator_counter(std::size_t) override {}

	virtual std::pair<std::string, std::size_t> svg_highlighted_targets() const override {
		using graphics = tobor::v1_1::tobor_graphics<world_type, positions_of_pieces_type_interactive>;
		return std::make_pair(graphics::draw_tobor_world_with_cell_markers(get22Game(), {}), 0);
	}

	virtual ~SpecialCaseGameFactory() override {}
};

