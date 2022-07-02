#ifndef CURSES_TOOLS_H
#define CURSES_TOOLS_H

#include <string>
#include <vector>
#include <ncurses.h>
#include <menu.h>
#include "game_opt.hpp"

namespace Curses {
	class MenuDialog {
	public:
		WINDOW* dialog;
		MENU*   menu;
		ITEM**  item;
		ITEM*   entry_cur;
		char**  entry;
		size_t  size;

		MenuDialog(std::vector<std::vector<std::string>>& entry,
		           std::vector<std::string>& entry_label,
		           const std::string& title,
		           chtype attr, chtype nattr);
		~MenuDialog();

		ITEM* getItem(size_t index) const;
	};

	void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string,
	                     chtype attr);

	void middle_print(WINDOW* win, int y, int startx, int width, char* string);
	void wclear(WINDOW* win, int y, int start, size_t length);
};

#endif
