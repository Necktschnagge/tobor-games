#include "color_generator.h"


bool QActionExtend::eventFilter(QObject* object, QEvent* e)
{
	qDebug() << "Action event";

	QString event_name = typeid(*e).name();

	try {
		QString x = object->metaObject()->className();
		qDebug() << event_name << " " << x;
		//if (in) ui->statusbar->showMessage(x);
	}
	catch (...) {
		//QString x = QString(typeid(object).name());
		QString x = QString::number(typeid(*object).hash_code());
		qDebug() << event_name << " " << x;
		//if (in) ui->statusbar->showMessage(x);
	}


    return false;
}

bool QActionExtend::event(QEvent* e)
{
	(void)e;
	qDebug() << "yes, i am here";
	return false;
}

//void QActionExtend::triggered(bool checked)
//{
//	qDebug() << "bla bla";
//}
