#ifndef RPG_H
#define RPG_H

#include <vector>
#include <cstdio>
#include <ncurses.h>
#include <panel.h>
#include <menu.h>
#include "rpg.hpp"
#include "square.hpp"
#include "item.hpp"
#include "living.hpp"
#include "game_opt.hpp"
#include "panels.hpp"

class Grid;

class Game {
	enum InfoWinCode {
		GRID = 0,
		GRID_MARKET,
		MARKET_BUY,
		MARKET_SELL,
		MARKET_SELL_FAIL,
		INVENTORY,
		INVENTORY_FAIL,
		BATTLE,
		MONSTER_DEAD,
		HERO_DEAD,
	};

	void drawHeroStats();
	void updateHeroStats();
	void drawInfoWin(InfoWinCode iwcode);
	void updateInfoWin(InfoWinCode iwcode);
	void drawBattleWin();
	void battleWinShowMonster(Monster* mon);
	void battleWinUpdateMonsterStats(Monster* mon);
	void battleWinClear();
	void battleWinUpdateAttack(Hero* hero, Monster* mon, Item* item, bool successful);
	void battleWinUpdateAttack(Hero* hero, Spell* spell, bool successful);
	void battleWinUpdateAttack(Monster* mon, Hero* hero, bool successful);
	bool monsterAppears(Hero* hero);
	Monster* randomMonster(int lvl);

	/* Game data */
	Grid* grid;
	Hero* hero;

	/* Items */
	std::vector<Weapon*> weapon;
	std::vector<Armor*> armor;
	std::vector<Potion*> potion;

	std::vector<Weapon*> parseWeapons(std::string filename);
	std::vector<Armor*>  parseArmor(std::string filename);
	std::vector<Potion*> parsePotions(std::string filename);

	/* Spells */
	std::vector<Spell*> firespell;
	std::vector<Spell*> icespell;
	std::vector<Spell*> lightspell;

	std::vector<Spell*> parseSpells(std::string filename, Spell::Type);

	static const int monster_possibility = 5;

	/* CURSES */
	WINDOW* hero_stats;
	WINDOW* info_win;
	WINDOW* battle_win;
	PanelManager panel;

	static const NCURSES_ATTR_T game_attr = COLOR_PAIR(GameOpt::WHITE_DEFAULT)|WA_BOLD;
	static const NCURSES_ATTR_T battle_attr = COLOR_PAIR(GameOpt::RED_DEFAULT)|WA_BOLD;
	static const NCURSES_ATTR_T hero_attr = COLOR_PAIR(GameOpt::GREEN_DEFAULT)|WA_BOLD;

public:
	Game(Hero::Category cat, std::string name, int l, int w);
	~Game();

	void play();
};

class Grid {
	friend class Game;

	/* Curses */
	WINDOW* win;

	/* Grid data */
	Square*** sq;
	int length;
	int width;

	Common common;
	Market market;
	NonAccesible nonacc;

	NCURSES_ATTR_T getCursesAttr() const { return COLOR_PAIR(GameOpt::GREEN_DEFAULT); };

public:
	enum Direction {
		UP = 0,
		RIGHT,
		DOWN,
		LEFT
	};

	Grid(int l, int w, std::vector<Weapon*>& weap,
	     std::vector<Armor*>& arm,
	     std::vector<Potion*>& pot,
	     std::vector<Spell*>& fsp,
	     std::vector<Spell*>& isp,
	     std::vector<Spell*>& lsp);
	~Grid();

	void drawMap();
	void drawHero(Hero* hero);
	void move(Hero* hero, Direction d);
};

#endif
