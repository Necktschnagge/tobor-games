#pragma once

#include <vector>
#include <QColor>
#include <QMenuBar>
#include <QAction>
#include <QEvent>
#include <QDebug>
#include <QSignalMapper>


namespace tobor {

	namespace v1_0 {

		class color_vector {

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

			static color_vector get_standard_coloring() {
				color_vector v;

				v.colors.emplace_back(0xFF, 0x00, 0x00, "red", "&red", "r");
				v.colors.emplace_back(0x00, 0xFF, 0x00, "green", "&green", "g");
				v.colors.emplace_back(0x00, 0x00, 0xFF, "blue", "&blue", "b");
				v.colors.emplace_back(0x00, 0xFF, 0xFF, "yellow", "&yellow", "y");

				return v;
			}

		};

	}
}
