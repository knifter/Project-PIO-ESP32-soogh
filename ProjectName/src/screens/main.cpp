#include "screens.h"

#include <memory>
#include <soogh.h>

#include "gui.h"

#include "config.h"
#include "tools-log.h"
#include "globals.h"

// C-style callbacks

/*********************************************************************************************************************************/
MainScreen::MainScreen(SooghGUI& g) : Screen(g)
{
};

void MainScreen::loop()
{
    return;
};

bool MainScreen::handle(soogh_event_t key)
{
	switch(key)
	{
		case KEY_A_SHORT:
			break;
		case KEY_B_SHORT:
			return true;
		case KEY_C_SHORT:
			break;
		case KEY_B_LONG:
		default: break;
	};
	return true;
};
