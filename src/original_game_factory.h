#pragma once

#include "game_controller.h"
#include "world_generator_1_1.h"
#include "cyclic_group_game_factory.h"

#include <string>
#include <memory>


template<class Pieces_Quantity_Type>
class OriginalGameFactory : public CyclicGroupGameFactory {
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

		return new DRWGameController<pieces_quantity_type>(
			world,
			_product_generator.side().get_positions_of_pieces(world).apply_permutation(initial_color_permutation),
			_product_generator.main().get_target_cell()
		);
	}

	inline OriginalGameFactory& operator++() { ++_product_generator; return *this; }

	virtual void increment() override { ++(*this); }

	virtual OriginalGameFactory* clone() const override { return new OriginalGameFactory(*this); }

	inline product_generator_type& product_generator() { return _product_generator; }

	virtual std::size_t world_generator_group_size() const override {
		return board_generator_type::CYCLIC_GROUP_SIZE;
	}
	
	virtual std::size_t get_world_generator_counter() const override {
		return _product_generator.main().get_counter();
	}

	virtual void set_world_generator_counter(std::size_t c) override {
		_product_generator.main().set_counter(c);
	}

	virtual std::size_t state_generator_group_size() const override {
		return state_generator_type::CYCLIC_GROUP_SIZE;
	}

	virtual std::size_t get_state_generator_counter() const override {
		return _product_generator.side().get_counter();
	}

	virtual void set_state_generator_counter(std::size_t c) override {
		_product_generator.side().set_counter(c);
	}

	virtual std::pair<std::string, std::size_t> svg_highlighted_targets() const override {

		using graphics = tobor::v1_1::tobor_graphics<world_type, positions_of_pieces_type_interactive>;

		const auto world{ _product_generator.main().get_tobor_world() };

		auto raw_cell_id_vector = board_generator_type::get_target_cell_id_vector(world);

		std::vector<cell_id_type> comfort_cell_id_vector;

		std::transform(raw_cell_id_vector.cbegin(), raw_cell_id_vector.cend(), std::back_inserter(comfort_cell_id_vector),
			[&](const auto& raw_cell_id) {
				return cell_id_type::create_by_id(raw_cell_id, world);
			}
		);

		std::string svg_string = graphics::draw_tobor_world_with_cell_markers(
			world,
			comfort_cell_id_vector
		);

		return std::make_pair(svg_string, comfort_cell_id_vector.size());

	}

	virtual ~OriginalGameFactory() override {}

};
