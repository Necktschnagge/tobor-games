#include "info.h"

#include "../src/mainwindow.h"

#include <iostream>

void print_info()
{
	std::cout << MainWindow::qtVersion() << std::endl;

}
