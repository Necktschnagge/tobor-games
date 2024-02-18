#pragma once

#include <QColor>
#include <QMenuBar>
#include <QAction>
#include <QEvent>
#include <QDebug>
#include <QSignalMapper>

#include <vector>
#include <stdexcept>

namespace tobor { 

	namespace v1_0 {

		class color_vector { // TODO: this class is ok, but should be enhanced later.

		public:

			struct color_item {

				int r{ 0 };
				int g{ 0 };
				int b{ 0 };

				std::string display_string;
				std::string display_string_with_underscore;
				std::string shortcut_letter;


				color_item(int r, int g, int b, std::string display_string, std::string display_string_with_underscore, std::string shortcut_letter) :
					r(r),
					g(g),
					b(b),
					display_string(display_string),
					display_string_with_underscore(display_string_with_underscore),
					shortcut_letter(shortcut_letter)
				{}
			};

		private:

		public:
			color_vector() {}

			std::vector<color_item> colors; // make it private!

			static color_vector get_standard_coloring(uint8_t count_colors) {
				color_vector v;

				v.colors.emplace_back(0xFF, 0x00, 0x00, "red", "&red", "r");
				v.colors.emplace_back(0x00, 0xFF, 0x00, "green", "&green", "g");
				v.colors.emplace_back(0x00, 0x00, 0xFF, "blue", "&blue", "b");
				
				v.colors.emplace_back(0xFF, 0xFF, 0x00, "yellow", "&yellow", "y");
				
				v.colors.emplace_back(0xFF, 0x90, 0x00, "orange", "&orange", "o");
				v.colors.emplace_back(0x90, 0x00, 0xFF, "purple", "&purple", "p");
				
				v.colors.emplace_back(0x00, 0xFF, 0xFF, "cyan", "&cyan", "c");
				v.colors.emplace_back(0xFF, 0x00, 0xFF, "magenta", "&magenta", "m");
				
				v.colors.emplace_back(0xcf, 0xcf, 0xcf, "light grey", "&light grey", "l");
				v.colors.emplace_back(0x47, 0x47, 0x47, "dark grey", "&dark grey", "d");
				v.colors.emplace_back(0x10, 0x10, 0x10, "black", "blac&k", "k");

				if (count_colors > v.colors.size()) {
					throw std::logic_error("Too many colors requested.");
				}

				color_vector result;

				std::copy_n(v.colors.cbegin(), count_colors, std::back_inserter(result.colors));

				return result;
			}

		};

	}
}
