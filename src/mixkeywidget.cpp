//------------------------------------------------------------------------
// Flags       : clang-format SMTGSequencer
// Project     : mixkey
// Filename    : mixkeywidget.cpp
// Created by  : abruehl, 09/2026
// Description : <Description>
//------------------------------------------------------------------------
#include "mixkeywidget.h"

#include <QLabel>
#include <QProcess>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

namespace MixKey {

//------------------------------------------------------------------------
void* createDocWidget ()
{
	QWidget* dock = new QWidget ();
	QVBoxLayout* layout = new QVBoxLayout ();
	layout->setContentsMargins (0, 0, 0, 0);
	layout->setSpacing (10);
	dock->setLayout (layout);

	QPushButton* pushButton = new QPushButton ();
	QPushButton::connect (pushButton, &QPushButton::pressed, [] () {});

	QLabel* label = new QLabel ("Welcome to MixKey...");
	label->setFixedSize (200, 30);
	layout->addWidget (label);
	layout->addWidget (pushButton);

	// QMainWindow *main = (QMainWindow *)obs_frontend_get_main_window();

	// main->addDockWidget(Qt::LeftDockWidgetArea,dock);

	return (void*)dock;
}
} // namespace MixKey
