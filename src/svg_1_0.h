#pragma once

#include "fsl/interface/stringable.h"
#include "models/legacy_world.h"
#include "models/redundant_cell_id.h"

#include "logger.h"

#include <numeric>
#include <string>

#include <fstream>

#include "svggen/path.h"
#include "svggen/path_command.h"
#include "svggen/path_commands/M.h"
#include "svggen/path_commands/Z.h"
#include "svggen/path_commands/a.h"
#include "svggen/path_commands/c.h"
#include "svggen/path_commands/l.h"

#include "svggen/compound.h"
#include "svggen/xml_root_element.h"
#include "svggen/style_sheet.h"

// tobor colors:
// red      #f4191c
// yellow   #ffbd02
// green    #00a340
// blue     #4285f4

namespace tobor {
	namespace v1_0 {
		namespace svg {

			namespace svg_path_elements {

			} // namespace svg_path_elements
		}    // namespace svg

		template <class Positions_Of_Pieces_Type_T>
		class tobor_graphics {
		public:
			using positions_of_pieces_type = Positions_Of_Pieces_Type_T;

			using cell_id_type = typename positions_of_pieces_type::cell_id_type;

			using pieces_quantity_type = typename positions_of_pieces_type::pieces_quantity_type;

			using pq_size_type = typename pieces_quantity_type::int_type;

			static constexpr pq_size_type COUNT_ALL_PIECES{ pieces_quantity_type::COUNT_ALL_PIECES };

			struct coloring {
				using array_type = std::array<std::string, COUNT_ALL_PIECES>;

				array_type colors;

				template <class... T>
				coloring(T&&... args) : colors{ std::forward<T>(args)... } {}
			};

			enum class piece_shape_selection { BALL, DUCK };

			static_assert(pieces_quantity_type::COUNT_TARGET_PIECES == 1, "Not yet supported: multiple target pieces");
		};
	} // namespace v1_0

} // namespace tobor
