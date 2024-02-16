#include "key_event_filter.h"

#include<QKeyEvent>

bool ControlKeyEventAgent::eventFilter(QObject* object, QEvent* e)
{
	(void)object;

	QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(e);

	if (keyEvent == nullptr)
		return false; //pass-through

	// found a keyEvent

	//switch (keyEvent->key()) {
	//	
	//}

	return true; //absorbing event
}
