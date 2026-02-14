#include "license_dialog.h"

#include "../license/license.h"


#include <QtGlobal> // version check
#include "text_input_dialog.h"


TextInputDialog::TextInputDialog(QWidget* parent, const QString& initialText) :
	QDialog(parent)
{
	setModal(true);
	setWindowTitle("Text Input");

	if (parent != nullptr) {
		setGeometry(parent->x() + 50, parent->y() + 50, parent->width() * 4 / 5, parent->height() * 4 / 5);
	}

	layout = new QVBoxLayout(this);

	text = new QTextEdit();
	text->setText(initialText);
	text->setReadOnly(false);

	ok_button = new QPushButton("OK");

	layout->addWidget(text);
	layout->addWidget(ok_button);

	QObject::connect(ok_button, &QPushButton::clicked, this, &QDialog::accept, Qt::AutoConnection);
}

QString TextInputDialog::getText() const
{
	return text->toPlainText();
}
