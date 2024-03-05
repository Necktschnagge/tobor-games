#pragma once

#include <QObject>
#include <QEvent>

class MainWindow;

class ControlKeyEventAgent : public QObject
{
	Q_OBJECT

		MainWindow* mainWindow;
public:
	ControlKeyEventAgent(MainWindow* mainWindow) : mainWindow(mainWindow) {}

	bool eventFilter(QObject* object, QEvent* event) override;

	virtual ~ControlKeyEventAgent() {}
};
