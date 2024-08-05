#include "license_dialog.h"

#include "../license/license.h"

#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

#include <QtGlobal> // version check


LicenseDialog::LicenseDialog(QWidget* parent) :
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

	const auto LICENSE{ QString::fromStdString(tobor::License::get_markdown_license()) };

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
	text->setPlainText(LICENSE);
#else
	text->setMarkdown(LICENSE);
#endif

	text->setTextInteractionFlags(Qt::LinksAccessibleByMouse);

	QPushButton* ok_button = new QPushButton("OK");

	layout->addWidget(text);
	layout->addWidget(ok_button);

	QObject::connect(ok_button, &QPushButton::clicked, this, &QDialog::accept, Qt::AutoConnection);
}
