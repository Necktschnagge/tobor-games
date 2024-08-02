#pragma once

#include "../license/license.h"

#include <QDialog>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

class LicenseDialog : public QDialog {
public:
	LicenseDialog(QWidget* parent = nullptr) :
		QDialog(parent)
	{
		setModal(true);
		setWindowTitle("License Information");

		if (parent != nullptr) {
			setGeometry(parent->x() + 50, parent->y() + 50, parent->width() * 4 / 5, parent->height() * 4 / 5);
		}

		QVBoxLayout* layout = new QVBoxLayout(this);

		QTextEdit* text = new QTextEdit();
		text->setReadOnly(true);
		text->setMarkdown(QString::fromStdString(tobor::License::get_markdown_license()));
		text->setTextInteractionFlags(Qt::LinksAccessibleByMouse);

		QPushButton* ok_button = new QPushButton("OK");

		layout->addWidget(text);
		layout->addWidget(ok_button);

		QObject::connect(ok_button, &QPushButton::clicked, this, &QDialog::accept, Qt::AutoConnection);
	}

};
