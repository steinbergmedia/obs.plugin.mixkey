//------------------------------------------------------------------------
// Flags       : clang-format SMTGSequencer
// Project     : mixkey
// Filename    : mixkeywidget.cpp
// Created by  : abruehl, 09/2026
// Description : <Description>
//------------------------------------------------------------------------
#include "mixkeywidget.h"

#include <QBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QProcess>
#include <QPushButton>
#include <QWidget>

namespace MixKey {

//------------------------------------------------------------------------
void* createDocWidget ()
{
	QWidget* dock = new QWidget ();
	QVBoxLayout* layout = new QVBoxLayout (dock);
	layout->setContentsMargins (6, 6, 6, 6);
	layout->setSpacing (0);
	layout->setAlignment (Qt::AlignTop);
	dock->setLayout (layout);

	QLabel* box = new QLabel ();
	box->setText ("SOURCE REQUIRED");
	box->setAlignment (Qt::AlignCenter);

	QPushButton* addButton = new QPushButton ();
	QPushButton::connect (addButton, &QPushButton::pressed, [] () {});

	addButton->setText ("Add MixKey Source");
	QIcon icon;
	icon.addFile (QString ("C:\\Users\\ABruehl\\Downloads\\Vector.svg"), QSize (10, 10));
	addButton->setIcon (icon);
	addButton->setIconSize (QSize (10, 10));

	QPushButton* startButton = new QPushButton ();
	QPushButton::connect (startButton, &QPushButton::pressed, [] () {});
	startButton->setText ("Start MixKey");
	QIcon startIcon;
	startIcon.addFile (QString ("C:\\Users\\ABruehl\\Downloads\\start.svg"), QSize (10, 10));
	startButton->setIcon (startIcon);
	startButton->setIconSize (QSize (10, 10));

	QPushButton* repairButton = new QPushButton ();
	QPushButton::connect (repairButton, &QPushButton::pressed, [] () {});
	repairButton->setText ("Repair MixKey Source");
	QIcon repairIcon;
	repairIcon.addFile (QString ("C:\\Users\\ABruehl\\Downloads\\repair.svg"), QSize (10, 10));
	repairButton->setIcon (repairIcon);
	repairButton->setIconSize (QSize (10, 10));

	QLabel* label =
	    new QLabel ("Click \"Add MixKey Source\" to add the MixKey your in Sources list.");
	label->setAlignment (Qt::AlignCenter);
	label->setWordWrap (true);

	layout->addWidget (box);
	layout->addWidget (label);
	layout->addWidget (addButton);
	layout->addWidget (startButton);
	layout->addWidget (repairButton);

	//layout->setAlignment (Qt::AlignCenter | Qt::AlignTop);

	// QMainWindow *main = (QMainWindow *)obs_frontend_get_main_window();

	// main->addDockWidget(Qt::LeftDockWidgetArea,dock);

	return (void*)dock;
}
} // namespace MixKey
