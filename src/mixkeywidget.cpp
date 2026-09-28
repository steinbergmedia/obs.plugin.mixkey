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
	dock->setStyleSheet ("QWidget { background-color:rgba(33, 33, 37, 1);} ");
	QVBoxLayout* layout = new QVBoxLayout ();
	layout->setContentsMargins (0, 0, 0, 0);
	layout->setSpacing (10);
	dock->setLayout (layout);

	QLabel* box = new QLabel ();
	box->setFixedSize (125, 19);
	box->setText ("SOURCE REQUIRED");
	box->setAlignment (Qt::AlignCenter);
	box->setStyleSheet (
	    "QLabel { border-radius:9; background-color:rgba(139, 92, 246, 0.3); font-size:8px; color:#D7C6FF; }");

	QPushButton* addButton = new QPushButton ();
	QPushButton::connect (addButton, &QPushButton::pressed, [] () {});

	addButton->setFixedSize (195, 29);
	addButton->setText ("Add MixKey Source");
	addButton->setStyleSheet (
	    "QPushButton { border-color:rgba(255, 255, 255, 0.04); border-width:1; border-radius:6; background-color:rgba(255, 255, 255, 0.16); }");
	QIcon icon;
	icon.addFile (QString ("C:\\Users\\ABruehl\\Downloads\\Vector.svg"), QSize (10, 10));
	addButton->setIcon (icon);
	addButton->setIconSize (QSize (10, 10));

	QPushButton* startButton = new QPushButton ();
	QPushButton::connect (startButton, &QPushButton::pressed, [] () {});
	startButton->setFixedSize (195, 29);
	startButton->setText ("Start MixKey");
	startButton->setStyleSheet (
	    "QPushButton { border-color:rgba(255, 255, 255, 0.04); border-width:1; border-radius:6; background-color:rgba(255, 255, 255, 0.16); }");
	QIcon startIcon;
	startIcon.addFile (QString ("C:\\Users\\ABruehl\\Downloads\\start.svg"), QSize (10, 10));
	startButton->setIcon (startIcon);
	startButton->setIconSize (QSize (10, 10));

	QPushButton* repairButton = new QPushButton ();
	QPushButton::connect (repairButton, &QPushButton::pressed, [] () {});
	repairButton->setFixedSize (195, 29);
	repairButton->setText ("Repair MixKey Source");
	repairButton->setStyleSheet (
	    "QPushButton { border-color:rgba(255, 255, 255, 0.04); border-width:1; border-radius:6; background-color:rgba(255, 255, 255, 0.16); }");
	QIcon repairIcon;
	repairIcon.addFile (QString ("C:\\Users\\ABruehl\\Downloads\\repair.svg"), QSize (10, 10));
	repairButton->setIcon (repairIcon);
	repairButton->setIconSize (QSize (10, 10));

	QLabel* label =
	    new QLabel ("Click \"Add MixKey Source\" to add the MixKey your in Sources list.");
	label->setAlignment (Qt::AlignCenter);
	label->setWordWrap (true);
	label->setFixedSize (195, 90);
	label->setStyleSheet ("QLabel { font-size:12px; color:#A5A8B5; }");

	layout->addWidget (box, 0, Qt::AlignCenter);
	layout->addWidget (label, 0, Qt::AlignCenter);
	layout->addWidget (addButton, 0, Qt::AlignCenter);
	layout->addWidget (startButton, 0, Qt::AlignCenter);
	layout->addWidget (repairButton, 0, Qt::AlignCenter);

	layout->setAlignment (Qt::AlignCenter | Qt::AlignTop);

	// QMainWindow *main = (QMainWindow *)obs_frontend_get_main_window();

	// main->addDockWidget(Qt::LeftDockWidgetArea,dock);

	return (void*)dock;
}
} // namespace MixKey
