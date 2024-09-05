#include "predefined.h"

#include "mainwindow.h"
#include "debug_utils.h"

#include "fsl/container/byte_tree_map.h"

#include <QApplication>

#include <clocale>

// TERMINOLOGY

/*
[GAME] BOARD            :       the entire world
CELL                    :       atomic unit of a BOARD
TARGET CELL             :       the CELL which is marked as goal
TARGET PIECE            :       PIECE which must be moved to the TARGET CELL
PIECE                   :       one robot / piece


*/

class Dummy {

	uint32_t data;

public:
	Dummy(const uint32_t& data) : data(data % (((uint32_t)1) << 24)){

	}

	static constexpr std::size_t byte_size() { return 3; }

	inline uint8_t get_byte(std::size_t index) const {
		return (data >> (index * 8)) & 0xFF;
	}
};

void experimental() {
	fsl::byte_tree_map<Dummy, uint16_t> btm(1023);

	Dummy d1(16), d2(974), d3(16 + (7 << 16)), d4(1024);

	btm[d1] = 1;
	btm[d2] = 2;
	btm[d3] = 3;
	btm[d4] = 4;

}

int main(int argc, char* argv[])
{
	init_logger();

	experimental();

	QApplication qt_app(argc, argv);
	std::setlocale(LC_NUMERIC, "C");
	MainWindow main_window;
	main_window.show();

#ifdef DEBUG_EVENT_LOGGER_ENABLE

	DebugEventLogger del(&main_window);
	qt_app.installEventFilter(&del);

#endif // DEBUG_EVENT_LOGGER_ENABLE

	return qt_app.exec();
}
