#include <sstream>
#include "fast-cpp-csv-parser/csv.h"
#include "curses_tools.hpp"
#include "tools.hpp"
#include "rpg.hpp"

using namespace std;


Game::Game(Hero::Category cat, string name, int l, int w)
{
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	start_color();
	use_default_colors();

	init_pair(GameOpt::DEFAULT_GREEN, -1, COLOR_GREEN);
	init_pair(GameOpt::DEFAULT_RED, -1, COLOR_RED);
	init_pair(GameOpt::RED_DEFAULT, COLOR_RED, -1);
	init_pair(GameOpt::WHITE_DEFAULT, COLOR_WHITE, -1);
	init_pair(GameOpt::GREEN_DEFAULT, COLOR_GREEN, -1);
	init_pair(GameOpt::WHITE_BLUE, COLOR_WHITE, COLOR_BLUE);
	init_pair(GameOpt::BLACK_BLUE, COLOR_BLACK, COLOR_BLUE);

	weapon     = parseWeapons("market/weapons.csv");
	armor      = parseArmor("market/armor.csv");
	potion     = parsePotions("market/potions.csv");
	firespell  = parseSpells("market/firespells.csv", Spell::FIRE);
	icespell   = parseSpells("market/icespells.csv", Spell::ICE);
	lightspell = parseSpells("market/lightningspells.csv", Spell::LIGHTNING);

	grid = new Grid(l, w, weapon, armor, potion, firespell, icespell, lightspell);
	switch (cat) {
	case Hero::WARRIOR:
		hero = new Warrior(name, weapon[0]);
		break;
	case Hero::SORCERER:
		hero = new Sorcerer(name, weapon[0]);
		break;
	case Hero::PALADIN:
		hero = new Paladin(name, weapon[0]);
		break;
	}

	drawHeroStats();
	drawBattleWin();
	drawInfoWin(InfoWinCode::GRID);
	grid->drawHero(hero);

	panel.add(PanelManager::GRID, grid->win);
	panel.add(PanelManager::MARKET_BUY, grid->market.buy_menu[0]->dialog);
	panel.add(PanelManager::MARKET_BUY, grid->market.buy_menu[1]->dialog);
	panel.add(PanelManager::MARKET_BUY, grid->market.buy_menu[2]->dialog);
	panel.add(PanelManager::MARKET_BUY, grid->market.buy_menu[3]->dialog);
	panel.add(PanelManager::MARKET_BUY, grid->market.buy_menu[4]->dialog);
	panel.add(PanelManager::MARKET_BUY, grid->market.buy_menu[5]->dialog);
	panel.add(PanelManager::MARKET_SELL, grid->market.sell_menu[0]->dialog);
	panel.add(PanelManager::MARKET_SELL, grid->market.sell_menu[1]->dialog);
	panel.add(PanelManager::MARKET_SELL, grid->market.sell_menu[2]->dialog);
	panel.add(PanelManager::MARKET_SELL, grid->market.sell_menu[3]->dialog);
	panel.add(PanelManager::MARKET_SELL, grid->market.sell_menu[4]->dialog);
	panel.add(PanelManager::MARKET_SELL, grid->market.sell_menu[5]->dialog);
	panel.add(PanelManager::INVENTORY, hero->inventory[0]->dialog);
	panel.add(PanelManager::INVENTORY, hero->inventory[1]->dialog);
	panel.add(PanelManager::INVENTORY, hero->inventory[2]->dialog);
	panel.add(PanelManager::INVENTORY, hero->inventory[3]->dialog);
	panel.add(PanelManager::BATTLE, battle_win);
}

Game::~Game()
{
	delwin(hero_stats);
	delwin(info_win);
	delwin(battle_win);
	//delete hero;
	delete grid;

	endwin();
}

void Game::play()
{
	ITEM* cur_item;
	Hero::Coord coord;
	Monster* mon;
	int ch;

	panel.switchTo(panel.GRID);
	while ((ch = panel.getchar())) {
		if (panel.is(panel.GRID)) {
			switch (ch) {
			case KEY_UP:
				grid->move(hero, grid->UP);
				break;
			case KEY_RIGHT:
				grid->move(hero, grid->RIGHT);
				break;
			case KEY_DOWN:
				grid->move(hero, grid->DOWN);
				break;
			case KEY_LEFT:
				grid->move(hero, grid->LEFT);
				break;
			case 'i':
				panel.switchTo(panel.INVENTORY);
				break;
			case 10:
				coord = hero->getCoord();
				if (grid->sq[coord.y][coord.x]->isMarket()) {
					grid->market.update(*hero);
					panel.switchTo(panel.MARKET_BUY);
				}
				break;
			case 'q':
				goto end;
			default:
				break;
			}

			switch (ch) {
			case KEY_UP:
			case KEY_RIGHT:
			case KEY_LEFT:
			case KEY_DOWN:
				if (monsterAppears(hero)) {
					mon = randomMonster(hero->getLevel());
					panel.switchTo(panel.BATTLE);
					battleWinShowMonster(mon);
				}
				break;
			}

		}
		else if (panel.is(panel.MARKET_BUY)) {
			switch (ch) {
			case KEY_UP:
				menu_driver(grid->market.buy_menu[panel.getIndex()]->menu, REQ_UP_ITEM);
				break;
			case KEY_DOWN:
				menu_driver(grid->market.buy_menu[panel.getIndex()]->menu, REQ_DOWN_ITEM);
				break;
			case KEY_RIGHT:
				panel.next();
				break;
			case KEY_LEFT:
				panel.prev();
				break;
			case 10:
			    cur_item = current_item(grid->market.buy_menu[panel.getIndex()]->menu);
			    if (cur_item) {
					grid->market.buy(*hero, panel.getIndex(), cur_item);
					updateHeroStats();
				}
				break;
			case 's':
				panel.switchTo(panel.MARKET_SELL);
				break;
			case 'q':
				panel.switchTo(panel.INVENTORY);
				panel.clear();
				hero->updateInventoryMenus();
				panel.add(panel.INVENTORY, hero->inventory[0]->dialog);
				panel.add(panel.INVENTORY, hero->inventory[1]->dialog);
				panel.add(panel.INVENTORY, hero->inventory[2]->dialog);
				panel.add(panel.INVENTORY, hero->inventory[3]->dialog);

				panel.switchTo(panel.GRID);
				break;
			default:
				break;
			}
		}
		else if (panel.is(panel.MARKET_SELL)) {
			switch (ch) {
			case KEY_UP:
				menu_driver(grid->market.sell_menu[panel.getIndex()]->menu, REQ_UP_ITEM);
				break;
			case KEY_DOWN:
				menu_driver(grid->market.sell_menu[panel.getIndex()]->menu, REQ_DOWN_ITEM);
				break;
			case KEY_RIGHT:
				panel.next();
				break;
			case KEY_LEFT:
				panel.prev();
				break;
			case 10:
			    cur_item = current_item(grid->market.sell_menu[panel.getIndex()]->menu);
			    if (cur_item) {
				    bool err;
					err = grid->market.sell(*hero, panel.getIndex(), cur_item);
					if (err) {
						updateInfoWin(InfoWinCode::MARKET_SELL_FAIL);
						panel.getchar();
					}
					else
						updateHeroStats();
				}
				break;
			case 'b':
				panel.switchTo(panel.MARKET_BUY);
				break;
			case 'q':
				panel.switchTo(panel.INVENTORY);
				panel.clear();
				hero->updateInventoryMenus();
				panel.add(panel.INVENTORY, hero->inventory[0]->dialog);
				panel.add(panel.INVENTORY, hero->inventory[1]->dialog);
				panel.add(panel.INVENTORY, hero->inventory[2]->dialog);
				panel.add(panel.INVENTORY, hero->inventory[3]->dialog);

				panel.switchTo(panel.GRID);
				break;
			default:
				break;
			}
		}
		else if (panel.is(panel.INVENTORY)) {
			int panel_index = panel.getIndex();
			int menu_index;
			bool update = false;

			switch (ch) {
			case KEY_UP:
				menu_driver(hero->inventory[panel.getIndex()]->menu, REQ_UP_ITEM);
				break;
			case KEY_DOWN:
				menu_driver(hero->inventory[panel.getIndex()]->menu, REQ_DOWN_ITEM);
				break;
			case KEY_RIGHT:
				panel.next();
				break;
			case KEY_LEFT:
				panel.prev();
				break;
			case 10:
				cur_item = current_item(hero->inventory[panel.getIndex()]->menu);
				if (cur_item) {
					bool err = false;
					menu_index = item_index(cur_item);
					if (panel.getIndex() == hero->INDEX_WEAPONS)
						err = hero->equip(hero->getWeapon(menu_index));
					else if (panel.getIndex() == hero->INDEX_ARMOR)
						err = hero->equip(hero->getArmor(menu_index));
					else if (panel.getIndex() == hero->INDEX_POTIONS) {
						hero->use(hero->getPotion(menu_index));
						updateHeroStats();
					}

					if (err) {
						updateInfoWin(InfoWinCode::INVENTORY_FAIL);
						panel.getchar();
					}
					else {
						update = true;
						if (panel.getIndex() == hero->INDEX_POTIONS)
							panel.switchTo(panel.BATTLE);
					}
				}
				break;
			case 'u':
				cur_item = current_item(hero->inventory[panel.getIndex()]->menu);
				menu_index = item_index(cur_item);

				hero->unequipArmor();
				update = true;
				break;
			case 'q':
				panel.switchTo(panel.GRID);
				break;
			default:
				break;
			}

			if (update) {
				panel.clear();
				hero->updateInventoryMenus();
				panel.add(panel.INVENTORY, hero->inventory[0]->dialog);
				panel.add(panel.INVENTORY, hero->inventory[1]->dialog);
				panel.add(panel.INVENTORY, hero->inventory[2]->dialog);
				panel.add(panel.INVENTORY, hero->inventory[3]->dialog);

				cur_item = hero->inventory[panel_index]->getItem(menu_index);
				set_current_item(hero->inventory[panel_index]->menu, cur_item);

				panel.switchTo(panel.INVENTORY, panel_index);
			}
		}
		else if (panel.is(panel.BATTLE)) {
			bool successful;

			switch (ch) {
			case 'a':
				successful = hero->attack(mon);
				battleWinUpdateAttack(hero, mon, hero->getWeaponEquipped(), successful);
				break;
			}

			if (mon->isDead()) {
				delete mon;
				updateInfoWin(InfoWinCode::MONSTER_DEAD);
				panel.getchar();
				hero->getPrize();
				panel.switchTo(panel.GRID);
				ch = 'q';
			}

			switch (ch) {
			case 'a':
				successful = mon->attack(hero);
				battleWinUpdateAttack(mon, hero, successful);
				if (hero->isDead()) {
					updateInfoWin(InfoWinCode::HERO_DEAD);
					panel.getchar();
					hero->revive();
					panel.switchTo(panel.GRID);
				}
				break;
			}
			updateHeroStats();
		}

		if (panel.is(panel.GRID)) {
			coord = hero->getCoord();
			if (grid->sq[coord.y][coord.x]->isMarket())
				updateInfoWin(InfoWinCode::GRID_MARKET);
			else
				updateInfoWin(InfoWinCode::GRID);

			grid->drawHero(hero);
		}
		else if (panel.is(panel.MARKET_BUY))
			updateInfoWin(InfoWinCode::MARKET_BUY);
		else if (panel.is(panel.MARKET_SELL))
			updateInfoWin(InfoWinCode::MARKET_SELL);
		else if (panel.is(panel.INVENTORY))
			updateInfoWin(InfoWinCode::INVENTORY);
		else if (panel.is(panel.BATTLE))
			updateInfoWin(InfoWinCode::BATTLE);
	}
end:
	return;
}

void Game::drawHeroStats()
{
	int starty, startx;
	int length = 2;
	int width = grid->width;

	starty = (LINES -grid->length)/2 -2;
	startx = (COLS -width)/2;

	hero_stats = newwin(length, width +2, starty, startx);
	updateHeroStats();
}

void Game::updateHeroStats()
{
	stringstream line1, line2;

	line1 << "NAME: " << hero->getName() << "  LVL: " << hero->getLevel() << "  HEALTH: "
		  << hero->getHealth() << "  POW: " << hero->getMagicPower() << "  STR: "
		  << hero->getStrength() << "  DEX: " << hero->getDexterity() << "  AGI: "
		  << hero->getAgility();
	line2 << "MONEY: " << hero->getMoney() << "  EXP: " << hero->getExperience();

	wattron(hero_stats, game_attr);
	werase(hero_stats);
	Curses::middle_print(hero_stats, 0, 0, grid->width, (char*)line1.str().c_str());
	Curses::middle_print(hero_stats, 1, 0, grid->width, (char*)line2.str().c_str());
	wattroff(hero_stats, game_attr);
	wrefresh(hero_stats);
}

void Game::drawInfoWin(InfoWinCode iwcode)
{
	int starty, startx;
	int length = 2;
	int width = grid->width;

	starty = (LINES +grid->length)/2 +2;
	startx = (COLS -width)/2;

	info_win = newwin(length, width +2, starty, startx);

	updateInfoWin(iwcode);
}

void Game::updateInfoWin(InfoWinCode iwcode)
{
	wattron(info_win, game_attr);
	switch (iwcode) {
	case GRID:
		werase(info_win);
		Curses::middle_print(info_win, 0, 0, grid->width,
		                     (char*)"Q: Quit Game  ARROWS: Move  I: Inventory");
		break;
	case GRID_MARKET:
		werase(info_win);
		Curses::middle_print(info_win, 0, 0, grid->width,
		                     (char*)"Q: Quit Game  ARROWS: Move  ENTER: Enter Market");
		break;
	case MARKET_BUY:
		werase(info_win);
		Curses::middle_print(info_win, 0, 0, grid->width,
		                     (char*)"Q: Quit Menu  UP/DOWN: Change Entry  LEFT/RIGHT: "
		                     "Change Menu  S: Sell Menu  ENTER: Buy");
		break;
	case MARKET_SELL:
		werase(info_win);
		Curses::middle_print(info_win, 0, 0, grid->width,
		                     (char*)"Q: Quit Menu  UP/DOWN: Change Entry  LEFT/RIGHT: "
		                     "Change Menu  B: Buy Menu  ENTER: Sell");
		break;
	case MARKET_SELL_FAIL:
		werase(info_win);
		wattron(info_win, battle_attr);
		Curses::middle_print(info_win, 0, 0, grid->width,
		                     (char*)"Cannot sell your only weapon! Press any key to continue");
		wattroff(info_win, battle_attr);
		break;
	case INVENTORY:
		werase(info_win);
		Curses::middle_print(info_win, 0, 0, grid->width,
		                     (char*)"Q: Quit Menu  UP/DOWN: Change Entry  LEFT/RIGHT: "
		                     "Change Menu  ENTER: Equip/Use  U: Unequip Armor");
		break;
	case INVENTORY_FAIL:
		werase(info_win);
		wattron(info_win, battle_attr);
		Curses::middle_print(info_win, 0, 0, grid->width,
		                     (char*)"Cannot Equip: Free hands! Press any key to continue");
		wattroff(info_win, battle_attr);
		break;
	case BATTLE:
		werase(info_win);
		Curses::middle_print(info_win, 0, 0, grid->width,
		                     (char*)"A: Attack");
		break;
	case MONSTER_DEAD:
		werase(info_win);
		Curses::middle_print(info_win, 0, 0, grid->width,
		                     (char*)"The monster is dead. Press any key to continue");
		break;
	case HERO_DEAD:
		werase(info_win);
		Curses::middle_print(info_win, 0, 0, grid->width,
		                     (char*)"Hero is dead and will revive. Press any key to"
		                     " continue");
		break;
	}
	wattroff(info_win, game_attr);
	wrefresh(info_win);
}

void Game::drawBattleWin()
{
	int starty, startx;
	const int length = 16;
	const int width  = 70;

	/* Find middle of screen */
	starty = (LINES -length)/2;
	startx = (COLS -width)/2;

	battle_win = newwin(length, width, starty, startx);
	keypad(battle_win, TRUE);
	wbkgd(battle_win, battle_attr);
	wattron(battle_win, battle_attr);
	box(battle_win, 0, 0);
	Curses::middle_print(battle_win, 0, 0, width, (char*)" Battle ");
	wattroff(battle_win, battle_attr);
}

void Game::battleWinShowMonster(Monster* mon)
{
	stringstream msg;

	msg << "A wild " << mon->getCategory() << " named " << mon->getName() << " appears!";

	battleWinClear();
	wattron(battle_win, battle_attr);
	Curses::wclear(battle_win, 2, 1, 69);
	Curses::middle_print(battle_win, 2, 1, 66, (char*)msg.str().c_str());
	wattroff(battle_win, battle_attr);

	battleWinUpdateMonsterStats(mon);
}

void Game::battleWinClear()
{
	Curses::wclear(battle_win, 8, 1, 69);
	Curses::wclear(battle_win, 9, 1, 69);
	Curses::wclear(battle_win, 11, 1, 69);
	Curses::wclear(battle_win, 12, 1, 69);
}

void Game::battleWinUpdateMonsterStats(Monster* mon)
{
	stringstream msg, msg2;

	msg  << "LVL: " << mon->getLevel() << " HEALTH: " << mon->getHealth();
	msg2 << "DAMAGE:" << mon->getDamage() << " DEFENSE: " << mon->getDefense()
		 << " EVADE (PCT): " << mon->getEvade();
	wattron(battle_win, game_attr);
	Curses::wclear(battle_win, 5, 1, 69);
	Curses::wclear(battle_win, 6, 1, 69);
	Curses::middle_print(battle_win, 4, 1, 66, (char*)"STATS");
	Curses::middle_print(battle_win, 5, 1, 66, (char*)msg.str().c_str());
	Curses::middle_print(battle_win, 6, 1, 66, (char*)msg2.str().c_str());
	wattroff(battle_win, game_attr);
	wrefresh(battle_win);
}

void Game::battleWinUpdateAttack(Hero* hero, Monster* mon, Item* item, bool successful)
{
	stringstream msg, msg2;

	battleWinClear();
	battleWinUpdateMonsterStats(mon);

	switch (item->getType()) {
	case Item::WEAPON:
		msg << hero->getName() << " strikes " << mon->getName() << " with a "
			<< item->getName();
			break;
	case Item::POTION:
		msg << hero->getName() << " uses " << item->getName();
		break;
	default:
		break;
	}

	if (successful) {
		if (mon->isDead())
			msg2 << mon->getName() << " received a fatal blow!";
		else
			msg2 << mon->getName() << " receives a blow";
	}
	else
		msg2 << mon->getName() << " evades!";

	wattron(battle_win, hero_attr);
	Curses::middle_print(battle_win, 8, 1, 66, (char*)msg.str().c_str());
	wattroff(battle_win, hero_attr);
	wattron(battle_win, battle_attr);
	Curses::middle_print(battle_win, 9, 1, 66, (char*)msg2.str().c_str());
	wattroff(battle_win, battle_attr);
	wrefresh(battle_win);
}

void Game::battleWinUpdateAttack(Monster* mon, Hero* hero, bool successful)
{
	stringstream msg, msg2;

	battleWinUpdateMonsterStats(mon);

	msg << mon->getName() << " strikes back " << hero->getName();
	if (successful) {
		Armor* armor;
		if (hero->isDead())
			msg2 << hero->getName() << " received a fatal blow ";
		else if ((armor = hero->getArmorEquipped()))
			msg2 << hero->getName() << " gets injured but is armored with a "
				 << armor->getName();
		else
			msg2 << hero->getName() << " receives a blow";
	}
	else
		msg2 << hero->getName() << " evades!";

	wattron(battle_win, battle_attr);
	Curses::middle_print(battle_win, 11, 1, 66, (char*)msg.str().c_str());
	wattroff(battle_win, battle_attr);
	wattron(battle_win, hero_attr);
	Curses::middle_print(battle_win, 12, 1, 66, (char*)msg2.str().c_str());
	wattroff(battle_win, hero_attr);
	wrefresh(battle_win);
}

bool Game::monsterAppears(Hero* hero)
{
	Hero::Coord coord = hero->getCoord();

	if (grid->sq[coord.y][coord.x]->isAccesible()) {
		int r = randgen(1, 100);

		if (r <= monster_possibility)
			return true;
		else
			return false;
	}
	else
		return false;
}

Monster* Game::randomMonster(int lvl)
{
	Monster* mon;
	int rand = randgen(0, Monster::CATEGORIES_TOTAL -1);

	switch (rand) {
	case (Monster::DRAGON):
		mon = new Dragon("Drugol", lvl);
		break;
	case (Monster::EXOSKELETON):
		mon = new Exoskeleton("Wurdulak", lvl);
		break;
	case (Monster::SPIRIT):
		mon = new Spirit("Blase", lvl);
		break;
	}

	return mon;
}

Grid::Grid(int l, int w, vector<Weapon*>& wap, vector<Armor*>& arm, vector<Potion*>& pot,
           vector<Spell*>& fsp, vector<Spell*>& isp, vector<Spell*>& lsp) :
	length(l), width(w), market(wap, arm, pot, fsp, isp, lsp)
{
	int i, j;

	sq = new Square**[length];
	for (i = 0; i < length; ++i) {
		sq[i] = new Square*[width];
		for (j = 0; j < width; ++j) {
			int randnum = randgen(1, 100);;

			if (i +j == 0 || randnum <= Square::common_pct_range)
				sq[i][j] = &common;
			else if (randnum <= Square::market_pct_range)
				sq[i][j] = &market;
			else
				sq[i][j] = &nonacc;
		}
	}

	drawMap();
}

Grid::~Grid()
{
	int i;

	//delwin(win);

	for (i = 0; i < length; ++i)
		delete[] sq[i];
	delete[] sq;
}

void Grid::drawMap()
{
	int starty, startx;
	int x, y;

	starty = (LINES -length)/2;
	startx = (COLS -width)/2;

	win = newwin(length +2, width +2, starty, startx);
	keypad(win, TRUE);
	wattron(win, this->getCursesAttr());
	box(win, 0, 0);
	wattroff(win, this->getCursesAttr());
	mvwaddch(win, 0, 1, ' ');

	for (y = 0; y < length; ++y) {
		for (x = 0; x < width; ++x) {
			wattron(win, sq[y][x]->getCursesAttr());
			mvwaddch(win, y +1, x +1, sq[y][x]->getSquare());
			wattroff(win, sq[y][x]->getCursesAttr());
		}
	}
	wrefresh(win);
}

void Grid::drawHero(Hero* hero)
{
	Hero::Coord coord = hero->getCoord();
	wmove(win, coord.y +1, coord.x +1);
	wrefresh(win);
}

void Grid::move(Hero* hero, Direction d)
{
	Hero::Coord coord = hero->getCoord();

	switch (d) {
	case UP:
		if (coord.y > 0 && sq[coord.y -1][coord.x]->isAccesible())
			hero->setCoord(coord.y -1, coord.x);
		break;
	case RIGHT:
		if (coord.x < width -1 && sq[coord.y][coord.x +1]->isAccesible())
			hero->setCoord(coord.y, coord.x +1);
		break;
	case DOWN:
		if (coord.y < length -1 && sq[coord.y +1][coord.x]->isAccesible())
			hero->setCoord(coord.y +1, coord.x);
		break;
	case LEFT:
		if (coord.x > 0 && sq[coord.y][coord.x -1]->isAccesible())
			hero->setCoord(coord.y, coord.x -1);
		break;
	}

	drawHero(hero);
}

vector<Weapon*> Game::parseWeapons(string filename)
{
	vector<Weapon*> item;
	string name;
	int price;
	int level;
	int damage;
	int hands;

	io::CSVReader<5> in(filename);
	in.read_header(io::ignore_extra_column, "name", "price", "level", "damage",
	               "hands");
	while (in.read_row(name, price, level, damage, hands))
		item.push_back(new Weapon(name, price, level, damage, hands));

	return item;
}

vector<Armor*> Game::parseArmor(string filename)
{
	vector<Armor*> item;
	string name;
	int price;
	int level;
	int protection;

	io::CSVReader<4> in(filename);
	in.read_header(io::ignore_extra_column, "name", "price", "level", "protection");
	while (in.read_row(name, price, level, protection))
		item.push_back(new Armor(name, price, level, protection));

	return item;
}

vector<Potion*> Game::parsePotions(string filename)
{
	vector<Potion*> item;
	string name;
	int price;
	int level;
	string boost_field;
	int boost;

	io::CSVReader<5> in(filename);
	in.read_header(io::ignore_extra_column, "name", "price", "level", "boost_field",
	               "boost");
	while (in.read_row(name, price, level, boost_field, boost))
		item.push_back(new Potion(name, price, level, boost_field, boost));

	return item;
}

vector<Spell*> Game::parseSpells(string filename, Spell::Type stype)
{
	vector<Spell*> spell;
	string name;
	int price;
	int level;
	int damage;
	int energy;
	int turns;

	io::CSVReader<6> in(filename);
	in.read_header(io::ignore_extra_column, "name", "price", "level", "damage", "energy",
	               "turns");
	while (in.read_row(name, price, level, damage, energy, turns)) {
		switch (stype) {
		case Spell::FIRE:
			spell.push_back(new FireSpell(name, price, level, damage, energy, turns));
			break;
		case Spell::ICE:
			spell.push_back(new IceSpell(name, price, level, damage, energy, turns));
			break;
		case Spell::LIGHTNING:
			spell.push_back(new LightningSpell(name, price, level, damage, energy, turns));
			break;
		}
	}

	return spell;
}

int main(void)
{
	int height = 25;
	int width = 80;
	Game rpg(Hero::WARRIOR, "Warriah", height, width);

	rpg.play();

	return 0;
}

