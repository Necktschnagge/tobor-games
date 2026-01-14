#pragma once

#include "../fsl/interface/stringable.h"
#include "../models/min_size_cell_id.h"
#include "style_sheet.h"

namespace tobor {
	namespace latest {
		namespace svggen {

			template <class Board>
			class piece_drawer {
			public:
				using board   = Board;
				using cell_id = tobor::v1_1::min_size_cell_id<board>;

				virtual std::unique_ptr<fsl::i::stringable> operator()(const board& b, const style& s, const cell_id& cid, const std::string& color) = 0;
			
				virtual ~piece_drawer() {}
			};

		} // namespace svggen
	}    // namespace latest
} // namespace tobor