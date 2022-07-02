#include <cstring>
#include <sstream>
#include <iomanip>
#include "curses_tools.hpp"
#include "tools.hpp"

using namespace std;

void Curses::print_in_middle(WINDOW *win, int starty, int startx, int width, char *string,
                             chtype attr)
{	int length, x, y;
	float temp;

	if(win == NULL)
		win = stdscr;
	getyx(win, y, x);
	if(startx != 0)
		x = startx;
	if(starty != 0)
		y = starty;
	if(width == 0)
		width = 80;

	length = strlen(string);
	temp = (width - length)/ 2;
	x = startx + (int)temp;
	//wattron(win, color);
	mvwprintw(win, y, x, "%s", string);
	//wattroff(win, color);
	refresh();
}

void Curses::middle_print(WINDOW* win, int y, int startx, int width, char* string)
{
	int length, x;

	length = strlen(string);
	if (length > width)
		width = length;
	x = startx +(int)(width - length)/2;

	mvwprintw(win, y, x, "%s", string);
	wrefresh(win);
}

void Curses::wclear(WINDOW* win, int y, int start, size_t length)
{
	wmove(win, y, start);
	for (size_t i = start; i < length; ++i)
		waddch(win, ' ');
	wrefresh(win);
}

Curses::MenuDialog::MenuDialog(vector<vector<string>>& entry_val,
                               vector<string>& entry_label,
                               const string& title,
                               chtype attr, chtype nattr)
	: size(entry_val.size())
{
	int starty, startx;
	const int length  = 14;
	const int padding = 8;
	stringstream entry_stream;
	size_t columns = entry_label.size();
	size_t* column_width;
	size_t row_width = 0;
	size_t i, j;

	/* Determine columns' max widths */
	/* By looking at entry_val values */
	column_width = new size_t[columns]();

	for (i = 0; i < entry_val.size(); ++i) {
		for (j = 0; j < entry_val[i].size(); ++j) {
			if (entry_val[i][j].size() > column_width[j])
				column_width[j] = entry_val[i][j].size();
		}
	}

	/* By looking at entry_val labels */
	for (i = 0; i < entry_label.size(); ++i)
		if (entry_label[i].size() > column_width[i])
			column_width[i] = entry_label[i].size();

	for (i = 0; i < columns; ++i) {
		row_width += column_width[i];
		if (i != columns -1)
			row_width += 3;
	}

	/* Find middle of screen */
	starty = (LINES -length)/2;
	startx = (COLS -(row_width +padding))/2;

	dialog = newwin(length, row_width +padding, starty, startx);
	keypad(dialog, TRUE);
	wbkgd(dialog, attr);
	wattron(dialog, attr);
	box(dialog, 0, 0);
	Curses::print_in_middle(dialog, 0, 0, row_width +padding, (char*)title.c_str(), attr);
	wmove(dialog, 2, 4);
	for (i = 0; i < columns; ++i) {
		if (i == columns -1)
			wprintw(dialog, "%-*s", (int)column_width[i], entry_label[i].c_str());
		else
			wprintw(dialog, "%-*s | ", (int)column_width[i], entry_label[i].c_str());
	}
	mvwaddch(dialog, 3, 0, ACS_LTEE);
	mvwhline(dialog, 3, 1, ACS_HLINE, row_width +padding);
	mvwaddch(dialog, 3, row_width +padding -1, ACS_RTEE);
	wattroff(dialog, attr);

	/* Create menu entries */
	entry = new char*[entry_val.size()];

	entry_stream << setfill(' ') << left;
	for (i = 0; i < entry_val.size(); ++i) {
		for (j = 0; j < entry_val[i].size(); ++j) {
			if (is_number(entry_val[i][j]))
				entry_stream << right;
			else
				entry_stream << left;

			entry_stream << setw(column_width[j]) << entry_val[i][j];

			if (j != entry_val[i].size() -1)
				entry_stream << " | ";

		}
		entry[i] = new char[entry_stream.str().size() +1];
		strcpy(entry[i], entry_stream.str().c_str());
		entry_stream.str("");
	}

	/* Create menu items */
	item = new ITEM*[entry_val.size() +1];
	for (i = 0; i < entry_val.size(); ++i)
		item[i] = new_item(entry[i], "");;
	item[entry_val.size()] = (ITEM*)NULL;

	menu = new_menu(item);
	set_menu_win(menu, dialog);
	set_menu_sub(menu, derwin(dialog, length -4, row_width +2, 4, 2));
	set_menu_mark(menu, "> ");
	set_menu_format(menu, 8, 1);
	set_menu_fore(menu, attr|A_REVERSE);
	set_menu_back(menu, attr);
	set_menu_grey(menu, nattr);

	post_menu(menu);

	wrefresh(dialog);
	refresh();

	delete[] column_width;
}

Curses::MenuDialog::~MenuDialog()
{
	size_t i;

	for (i = 0; i < size; ++i)
		free_item(item[i]);

	unpost_menu(menu);
	free_menu(menu);

	for (i = 0; i < size; ++i)
		delete[] entry[i];
	delete[] entry;

	delwin(dialog);
}

ITEM* Curses::MenuDialog::getItem(size_t index) const
{
	if (index < size -1)
		return item[index];
	else
		return item[size -1];
}
