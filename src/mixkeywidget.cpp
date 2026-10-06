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

#include "obs.h"

namespace MixKey {

//------------------------------------------------------------------------
struct Context
{
	Client* client {nullptr};
};

static Context context;

//------------------------------------------------------------------------
static void addSource ()
{
	if (streamChannelExists ())
		return;

	createStreamChannelSource ();
	return;
}

//------------------------------------------------------------------------
static void addIcon (const QString& iconPath, QPushButton* button)
{
	if (iconPath.isEmpty () || button == nullptr)
		return;

	static const QSize kIconSize = QSize (10, 10);

	QIcon icon;
	icon.addFile (iconPath, kIconSize);
	button->setIcon (icon);
	button->setIconSize (kIconSize);
}

//------------------------------------------------------------------------
static QPushButton* createButton (const QString& text, const QString& iconPath,
                                  const QString& objectName)
{
	QPushButton* button = new QPushButton ();
	button->setText (text);
	button->setObjectName (objectName);

	addIcon (iconPath, button);

	return button;
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

	context.client = new Client (dock);

	QLabel* status = new QLabel ();
	status->setText ("Offline");
	status->setAlignment (Qt::AlignCenter);
	status->setStyleSheet ("color: red;");
	status->setFixedHeight (38);

	QPushButton* addButton = createButton (
	    "Add MixKey Source", "C:\\Users\\ABruehl\\Downloads\\Vector.svg", "addbutton");
	QPushButton::connect (addButton, &QPushButton::pressed, [] () { addSource (); });
	addButton->setDisabled (streamChannelExists ());

	QPushButton* startButton =
	    createButton ("Start MixKey", "C:\\Users\\ABruehl\\Downloads\\start.svg", "startbutton");

	QPushButton* repairButton = createButton (
	    "Repair MixKey Source", "C:\\Users\\ABruehl\\Downloads\\repair.svg", "repairbutton");

	layout->addWidget (status);
	layout->addWidget (addButton);
	layout->addWidget (startButton);
	layout->addWidget (repairButton);
	
	context.client->onConnectionStateChanged = [status] (const Client::State state) {
		switch (state)
		{
			case Client::State::kConnected:
			{
				status->setStyleSheet ("color: green;");
				status->setText ("Online");
				break;
			}
			case Client::State::kDisconnected:
			{
				status->setStyleSheet ("color: red;");
				status->setText ("Offline");
				break;
			}
		}
	};

	return (void*)dock;
}

//------------------------------------------------------------------------
} // namespace MixKey
