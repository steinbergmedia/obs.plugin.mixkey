//------------------------------------------------------------------------
// Flags       : clang-format SMTGSequencer
// Project     : mixkey
// Filename    : mixkeywidget.cpp
// Created by  : abruehl, 09/2026
// Description : <Description>
//------------------------------------------------------------------------
#include "mixkeywidget.h"

#include "mixkeyipc.h"
#include "sourcehandling.h"

#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

namespace MixKey {

//------------------------------------------------------------------------
struct Context
{
	Client* socket;
};

static Context clientContext;

//------------------------------------------------------------------------
static void addSource ()
{
	if (streamChannelExists ())
		return;

	createStreamChannelSource ();
	return;
}

//------------------------------------------------------------------------
QPushButton* createButton (const QString& text, const QString& iconPath = "",
                           const QString& objectName = "")
{
	QPushButton* newButton = new QPushButton ();
	newButton->setText (text);
	QIcon buttonIcon;
	buttonIcon.addFile (iconPath, QSize (10, 10));
	newButton->setIcon (buttonIcon);
	newButton->setIconSize (QSize (10, 10));
	if (objectName.isEmpty () == false)
		newButton->setObjectName (objectName);

	return newButton;
}

//------------------------------------------------------------------------
void* createDocWidget ()
{
	QWidget* dock = new QWidget ();
	QVBoxLayout* layout = new QVBoxLayout (dock);
	layout->setContentsMargins (6, 6, 6, 6);
	layout->setSpacing (0);
	layout->setAlignment (Qt::AlignTop);
	dock->setLayout (layout);

	QLabel* statusIndicator = new QLabel ();
	statusIndicator->setText ("SOURCE REQUIRED");
	statusIndicator->setAlignment (Qt::AlignCenter);

	QPushButton* addButton = createButton (
	    "Add MixKey Source", "C:\\Users\\ABruehl\\Downloads\\Vector.svg", "addbutton");
	QPushButton::connect (addButton, &QPushButton::pressed, [] () {
		//
		addSource ();
	});

	QPushButton* startButton =
	    createButton ("Start MixKey", "C:\\Users\\ABruehl\\Downloads\\start.svg", "startbutton");

	QPushButton* repairButton = createButton (
	    "Repair MixKey Source", "C:\\Users\\ABruehl\\Downloads\\repair.svg", "repairbutton");

	QLabel* label =
	    new QLabel ("Click \"Add MixKey Source\" to add the MixKey your in Sources list.");
	label->setAlignment (Qt::AlignCenter);
	label->setWordWrap (true);

	layout->addWidget (statusIndicator);
	layout->addWidget (label);
	layout->addWidget (addButton);
	layout->addWidget (startButton);
	layout->addWidget (repairButton);

	clientContext.socket = new Client (dock);

	return (void*)dock;
}

//------------------------------------------------------------------------
} // namespace MixKey
