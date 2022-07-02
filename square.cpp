#include <vector>
#include <string>
#include "square.hpp"

using namespace std;
using Curses::MenuDialog;

inline char Common::getSquare() const
{
	return ' ';
}

inline bool Common::isAccesible() const
{
	return true;
};

inline bool Common::isMarket() const
{
	return false;
}

inline NCURSES_ATTR_T Common::getCursesAttr() const
{
	return nattr;
};

inline char NonAccesible::getSquare() const
{
	return ' ';
}

inline bool NonAccesible::isAccesible() const
{
	return false;
};

inline bool NonAccesible::isMarket() const
{
	return false;
}

inline NCURSES_ATTR_T NonAccesible::getCursesAttr() const
{
	return nattr;
};

inline char Market::getSquare() const
{
	return '$';
}

inline bool Market::isAccesible() const
{
	return true;
};

inline bool Market::isMarket() const
{
	return true;
}

inline NCURSES_ATTR_T Market::getCursesAttr() const
{
	return nattr;
};

inline NCURSES_ATTR_T Market::getCursesMenuAttr() const
{
	return nmattr;
};

inline NCURSES_ATTR_T Market::getCursesNegMenuAttr() const
{
	return nnmattr;
};

Market::Market(vector<Weapon*>& weap,
               vector<Armor*>& arm,
               vector<Potion*>& pot,
               vector<Spell*>& fsp,
               vector<Spell*>& isp,
               vector<Spell*>& lsp) :
	weapon(weap), armor(arm), potion(pot), firespell(fsp), icespell(isp),
	lightningspell(lsp)
{
	createMenus(BUY);
	createMenus(SELL);
}

Market::~Market()
{
	size_t i;

	for (i = 0; i < menus; ++i) {
		delete buy_menu[i];
		delete sell_menu[i];
	}
}

void Market::createMenus(MenuCode mcode)
{
	MenuDialog** menu;

	if (mcode == BUY)
		menu = buy_menu;
	else
		menu = sell_menu;

	menu[0] = createWeaponMenu(mcode);
	menu[1] = createArmorMenu(mcode);
	menu[2] = createPotionMenu(mcode);
	menu[3] = createSpellMenu(firespell, "Fire Spells", mcode);
	menu[4] = createSpellMenu(icespell, "Ice Spells", mcode);
	menu[5] = createSpellMenu(lightningspell, "Lightning Spells", mcode);
}

MenuDialog* Market::createWeaponMenu(MenuCode mcode)
{
	vector<vector<string>> entry(weapon.size(), vector<string>(0));
	vector<string> entry_label;
	string menu_title;
	string mcode_str;
	size_t i;

	if (mcode == BUY)
		mcode_str = "Buy";
	else
		mcode_str = "Sell";

	for (i = 0; i < weapon.size(); ++i) {
		entry[i].push_back(weapon[i]->getName());
		entry[i].push_back(to_string(weapon[i]->getLevel()));
		entry[i].push_back(to_string(weapon[i]->getDamage()));
		entry[i].push_back(to_string(weapon[i]->getHands()));
		if (mcode == BUY)
			entry[i].push_back(to_string(weapon[i]->getPrice()));
		else
			entry[i].push_back(to_string(weapon[i]->getSellValue()));
	}

	entry_label.push_back("Name");
	entry_label.push_back("Level");
	entry_label.push_back("Damage");
	entry_label.push_back("Hands");
	if (mcode == BUY)
		entry_label.push_back("Price");
	else
		entry_label.push_back("Value");

	menu_title = " Market (" + mcode_str + ") - Weapons ";

	return new MenuDialog(entry, entry_label, menu_title, getCursesMenuAttr(),
	                      getCursesNegMenuAttr());

}

MenuDialog* Market::createArmorMenu(MenuCode mcode)
{
	vector<vector<string>> entry(armor.size(), vector<string>(0));
	vector<string> entry_label;
	string menu_title;
	string mcode_str;
	size_t i;

	if (mcode == BUY)
		mcode_str = "Buy";
	else
		mcode_str = "Sell";

	for (i = 0; i < armor.size(); ++i) {
		entry[i].push_back(armor[i]->getName());
		entry[i].push_back(to_string(armor[i]->getLevel()));
		entry[i].push_back(to_string(armor[i]->getProtection()));
		if (mcode == BUY)
			entry[i].push_back(to_string(armor[i]->getPrice()));
		else
			entry[i].push_back(to_string(armor[i]->getSellValue()));
	}

	entry_label.push_back("Name");
	entry_label.push_back("Level");
	entry_label.push_back("Protection");
	if (mcode == BUY)
		entry_label.push_back("Price");
	else
		entry_label.push_back("Value");

	menu_title = " Market (" + mcode_str + ") - Armor ";

	return new MenuDialog(entry, entry_label, menu_title, getCursesMenuAttr(),
	                      getCursesNegMenuAttr());
}

MenuDialog* Market::createPotionMenu(MenuCode mcode)
{
	vector<vector<string>> entry(potion.size(), vector<string>(0));
	vector<string> entry_label;
	string menu_title;
	string mcode_str;
	size_t i;

	if (mcode == BUY)
		mcode_str = "Buy";
	else
		mcode_str = "Sell";

	for (i = 0; i < potion.size(); ++i) {
		entry[i].push_back(potion[i]->getName());
		entry[i].push_back(to_string(potion[i]->getLevel()));
		entry[i].push_back(potion[i]->getBoostField());
		entry[i].push_back(to_string(potion[i]->getBoost()));
		if (mcode == BUY)
			entry[i].push_back(to_string(potion[i]->getPrice()));
		else
			entry[i].push_back(to_string(potion[i]->getSellValue()));
	}

	entry_label.push_back("Name");
	entry_label.push_back("Level");
	entry_label.push_back("Boost Field");
	entry_label.push_back("Boost (Pct)");
	if (mcode == BUY)
		entry_label.push_back("Price");
	else
		entry_label.push_back("Value");

	menu_title = " Market (" + mcode_str + ") - Potions ";

	return new MenuDialog(entry, entry_label, menu_title,
	                      getCursesMenuAttr(), getCursesNegMenuAttr());

}

MenuDialog* Market::createSpellMenu(vector<Spell*>& spell, string spell_name,
                                    MenuCode mcode)
{
	vector<vector<string>> entry(spell.size(), vector<string>(0));
	vector<string> entry_label;
	string menu_title;
	string mcode_str;
	size_t i;

	if (mcode == BUY)
		mcode_str = "Buy";
	else
		mcode_str = "Sell";

	for (i = 0; i < spell.size(); ++i) {
		entry[i].push_back(spell[i]->getName());
		entry[i].push_back(to_string(spell[i]->getLevel()));
		entry[i].push_back(to_string(spell[i]->getDamage()));
		entry[i].push_back(to_string(spell[i]->getEnergy()));
		entry[i].push_back(to_string(spell[i]->getTurns()));
		if (mcode == BUY)
			entry[i].push_back(to_string(spell[i]->getPrice()));
		else
			entry[i].push_back(to_string(spell[i]->getSellValue()));
	}

	entry_label.push_back("Name");
	entry_label.push_back("Level");
	entry_label.push_back("Damage");
	entry_label.push_back("Energy");
	entry_label.push_back("Turns");
	if (mcode == BUY)
		entry_label.push_back("Price");
	else
		entry_label.push_back("Value");

	menu_title = " Market (" + mcode_str + ") - " + spell_name + " ";

	return new MenuDialog(entry, entry_label, menu_title, getCursesMenuAttr(),
						  getCursesNegMenuAttr());
}

void Market::update(Hero& hero)
{
	updateBuyMenus(hero);
	updateSellMenus(hero);
}

void Market::updateBuyMenus(Hero& hero)
{
	size_t i;

	for (i = 0; i < weapon.size(); ++i) {
		if (hero.getLevel() < weapon[i]->getLevel() ||
		    hero.getMoney() < weapon[i]->getPrice() ||
		    hero.ownsItem(weapon[i]))
			item_opts_off(buy_menu[0]->item[i], O_SELECTABLE);
		else
			item_opts_on(buy_menu[0]->item[i], O_SELECTABLE);
	}

	for (i = 0; i < armor.size(); ++i) {
		if (hero.getLevel() < armor[i]->getLevel() ||
		    hero.getMoney() < armor[i]->getPrice() ||
		    hero.ownsItem(armor[i]))
			item_opts_off(buy_menu[1]->item[i], O_SELECTABLE);
		else
			item_opts_on(buy_menu[1]->item[i], O_SELECTABLE);
	}

	for (i = 0; i < potion.size(); ++i) {
		if (hero.getLevel() < potion[i]->getLevel() ||
		    hero.getMoney() < potion[i]->getPrice() ||
		    hero.ownsItem(potion[i]))
			item_opts_off(buy_menu[2]->item[i], O_SELECTABLE);
		else
			item_opts_on(buy_menu[2]->item[i], O_SELECTABLE);
	}

	for (i = 0; i < firespell.size(); ++i) {
		if (hero.getLevel() < firespell[i]->getLevel() ||
		    hero.getMoney() < firespell[i]->getPrice() ||
		    hero.ownsSpell(firespell[i]))
			item_opts_off(buy_menu[3]->item[i], O_SELECTABLE);
		else
			item_opts_on(buy_menu[3]->item[i], O_SELECTABLE);
	}

	for (i = 0; i < icespell.size(); ++i) {
		if (hero.getLevel() < icespell[i]->getLevel() ||
		    hero.getMoney() < icespell[i]->getPrice() ||
		    hero.ownsSpell(icespell[i]))
			item_opts_off(buy_menu[4]->item[i], O_SELECTABLE);
		else
			item_opts_on(buy_menu[4]->item[i], O_SELECTABLE);
	}

	for (i = 0; i < lightningspell.size(); ++i) {
		if (hero.getLevel() < lightningspell[i]->getLevel() ||
		    hero.getMoney() < lightningspell[i]->getPrice() ||
		    hero.ownsSpell(lightningspell[i]))
			item_opts_off(buy_menu[5]->item[i], O_SELECTABLE);
		else
			item_opts_on(buy_menu[5]->item[i], O_SELECTABLE);
	}
}

void Market::updateSellMenus(Hero& hero)
{
	size_t i;

	for (i = 0; i < weapon.size(); ++i) {
		if (hero.ownsItem(weapon[i]))
			item_opts_on(sell_menu[0]->item[i], O_SELECTABLE);
		else
			item_opts_off(sell_menu[0]->item[i], O_SELECTABLE);
	}

	for (i = 0; i < armor.size(); ++i) {
		if (hero.ownsItem(armor[i]))
			item_opts_on(sell_menu[1]->item[i], O_SELECTABLE);
		else
			item_opts_off(sell_menu[1]->item[i], O_SELECTABLE);
	}

	for (i = 0; i < potion.size(); ++i) {
		if (hero.ownsItem(potion[i]))
			item_opts_on(sell_menu[2]->item[i], O_SELECTABLE);
		else
			item_opts_off(sell_menu[2]->item[i], O_SELECTABLE);
	}

	for (i = 0; i < firespell.size(); ++i) {
		if (hero.ownsSpell(firespell[i]))
			item_opts_on(sell_menu[3]->item[i], O_SELECTABLE);
		else
			item_opts_off(sell_menu[3]->item[i], O_SELECTABLE);
	}

	for (i = 0; i < icespell.size(); ++i) {
		if (hero.ownsSpell(icespell[i]))
			item_opts_on(sell_menu[4]->item[i], O_SELECTABLE);
		else
			item_opts_off(sell_menu[4]->item[i], O_SELECTABLE);
	}

	for (i = 0; i < lightningspell.size(); ++i) {
		if (hero.ownsSpell(lightningspell[i]))
			item_opts_on(sell_menu[5]->item[i], O_SELECTABLE);
		else
			item_opts_off(sell_menu[5]->item[i], O_SELECTABLE);
	}
}

void Market::buy(Hero& hero, int menu_index, ITEM* item)
{
	if (!(item_opts(item) & O_SELECTABLE))
		return;

	int i = item_index(item);

	switch(menu_index) {
	case 0:
		hero.buyItem(weapon[i]);
		break;
	case 1:
		hero.buyItem(armor[i]);
		break;
	case 2:
		hero.buyItem(potion[i]);
		break;
	case 3:
		hero.buySpell(firespell[i]);
		break;
	case 4:
		hero.buySpell(icespell[i]);
		break;
	case 5:
		hero.buySpell(lightningspell[i]);
		break;
	}

	update(hero);
}

bool Market::sell(Hero& hero, int menu_index, ITEM* item)
{

	if (!(item_opts(item) & O_SELECTABLE))
		return true;

	bool err;
	int i = item_index(item);

	switch(menu_index) {
	case 0:
		err = hero.sellItem(weapon[i]);
		if (err)
			return false;
		break;
	case 1:
		hero.sellItem(armor[i]);
		break;
	case 2:
		hero.sellItem(potion[i]);
		break;
	case 3:
		hero.sellSpell(firespell[i]);
		break;
	case 4:
		hero.sellSpell(icespell[i]);
		break;
	case 5:
		hero.sellSpell(lightningspell[i]);
		break;
	}

	update(hero);

	return true;
}
