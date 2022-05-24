#include "screens.h"

#include <soogh.h>

#include "gui.h"

#include "config.h"
#include "tools-log.h"
#include "globals.h"
#include "soogh-debug.h"
#include <treemenu.h>

// C-style callbacks
void menu_close_cb(MenuItem* item, void* data);

MenuScreen::MenuScreen(SooghGUI& g) : Screen(g)
{
	menu.addSeparator("Menu");

	menu.onClose(menu_close_cb);

	menu.open();
};

void menu_close_cb(MenuItem* item, void* data)
{
	gui.popScreen();
};

void MenuScreen::load()
{
	// Do not load my screen, menu is overlayed
};

bool MenuScreen::loop()
{    
    return false;
};

bool MenuScreen::handle(soogh_event_t e)
{	
	switch(e)
	{
		case KEY_A_SHORT:
		case KEY_A_LONG:
		case KEY_A_LONG_REPEAT:
			menu.sendKey(TreeMenu::KEY_LEFT);
			break;
		case KEY_B_SHORT:
			menu.sendKey(TreeMenu::KEY_ENTER);
			break;
		case KEY_C_SHORT:
		case KEY_C_LONG:
		case KEY_C_LONG_REPEAT:
			menu.sendKey(TreeMenu::KEY_RIGHT);
			break;
		case KEY_B_LONG:
			menu.sendKey(TreeMenu::KEY_ESC);
			break;
		case KEY_AC_LONG:
			gui.popScreen();
			return true;
		default: break;
	};
	return true;
};
