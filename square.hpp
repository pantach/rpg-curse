#ifndef SQUARE_H
#define SQUARE_H

#include <ncurses.h>
#include "game_opt.hpp"
#include "item.hpp"
#include "living.hpp"
#include "curses_tools.hpp"

class Square {
public:
	static const int common_pct_range = 85;
	static const int market_pct_range = common_pct_range +1;
	static const int nonacc_pct_range = market_pct_range +14;

	Square() {};
	virtual ~Square() {};

	virtual char getSquare() const = 0;
	virtual bool isAccesible() const = 0;
	virtual bool isMarket() const = 0;

	virtual NCURSES_ATTR_T getCursesAttr() const = 0;
};

class Common: public Square {
	static const NCURSES_ATTR_T nattr = A_NORMAL;

public:
	char getSquare() const;
	bool isAccesible() const;
	bool isMarket() const;

	NCURSES_ATTR_T getCursesAttr() const;
};

class NonAccesible: public Square {
	static const NCURSES_ATTR_T nattr = COLOR_PAIR(GameOpt::DEFAULT_GREEN);

public:
	char getSquare() const;
	bool isAccesible() const;
	bool isMarket() const;

	NCURSES_ATTR_T getCursesAttr() const;
};

class Market: public Square {
	friend class Game;

	/* Market data */
	std::vector<Weapon*> weapon;
	std::vector<Armor*>  armor;
	std::vector<Potion*> potion;
	std::vector<Spell*>  firespell;
	std::vector<Spell*>  icespell;
	std::vector<Spell*>  lightningspell;

	enum MenuCode {
		BUY = 0,
		SELL
	};

	void createMenus(MenuCode);
	Curses::MenuDialog* createWeaponMenu(MenuCode mcode);
	Curses::MenuDialog* createArmorMenu(MenuCode mcode);
	Curses::MenuDialog* createPotionMenu(MenuCode mcode);
	Curses::MenuDialog* createSpellMenu(std::vector<Spell*>& spell, std::string spell_name,
	                                    MenuCode mcode);

	void update(Hero& hero);
	void updateBuyMenus(Hero& hero);
	void updateSellMenus(Hero& hero);

	// Curses
	static const size_t menus = 6;
	class Curses::MenuDialog* buy_menu[menus];
	class Curses::MenuDialog* sell_menu[menus];

	static const NCURSES_ATTR_T nattr   = COLOR_PAIR(GameOpt::RED_DEFAULT);
	static const NCURSES_ATTR_T nmattr  = COLOR_PAIR(GameOpt::WHITE_BLUE)|WA_BOLD;
	static const NCURSES_ATTR_T nnmattr = COLOR_PAIR(GameOpt::BLACK_BLUE)|WA_BOLD;

public:
	Market(std::vector<Weapon*>& weap,
	       std::vector<Armor*>& arm,
	       std::vector<Potion*>& pot,
	       std::vector<Spell*>& fsp,
	       std::vector<Spell*>& isp,
	       std::vector<Spell*>& lsp);
	~Market();

	void buy(Hero& hero, int menu_index, ITEM* item);
	bool sell(Hero& hero, int menu_index, ITEM* item);

	char getSquare() const;
	bool isAccesible() const;
	bool isMarket() const;

	// Curses
	NCURSES_ATTR_T getCursesAttr() const;
	NCURSES_ATTR_T getCursesMenuAttr() const;
	NCURSES_ATTR_T getCursesNegMenuAttr() const;
};

#endif
