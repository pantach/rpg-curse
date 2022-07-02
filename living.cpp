#include "living.hpp"
#include "tools.hpp"

using namespace std;
using Curses::MenuDialog;

string Living::getName() const
{
	return name;
}

int Living::getLevel() const
{
	return lvl;
}

int Living::getHealth() const
{
	if (healthPower < 0)
		return 0;
	else
		return healthPower;
}

bool Living::isDead() const
{
	return (healthPower <= 0);
}

void Living::setHealth(int h)
{
	healthPower = h;
}

void Living::removeHealth(int h)
{
	healthPower -= h;
}

void Living::regenerate()
{
	healthPower += health_regenerate;
}

void Living::levelUp()
{
	lvl++;
}

Hero::Hero(std::string name, int pow, int str, int dex, int agi, Weapon* weap)
	: Living(name, 1, 100), magicPower(pow), strength(str), dexterity(dex),
	agility(agi), money(base_money), experience(0), weapon_equipped(NULL),
	armor_equipped(NULL)
{
	coord.x = 0;
	coord.y = 0;
	weapon.push_back(weap);
	equip(weap);
	createInventoryMenus();
}

Hero::Coord Hero::getCoord() const
{
	return coord;
}

void Hero::setCoord(int y, int x)
{
	coord.y = y;
	coord.x = x;
}

int Hero::getMagicPower() const
{
	return magicPower;
}

int Hero::getStrength() const
{
	return strength;
}

int Hero::getDexterity() const
{
	return dexterity;
}

int Hero::getAgility() const
{
	return agility;
}

int Hero::getMoney() const
{
	return money;
}

int Hero::getExperience() const
{
	return experience;
}

void Hero::setStrength(int val)
{
	strength = val*getLevel();
}

void Hero::setDexterity(int val)
{
	dexterity = val*getLevel();
}

void Hero::setAgility(int val)
{
	agility = val +getLevel()*2;
}

Weapon* Hero::getWeapon(int index) const
{
	return weapon[index];
}

Armor* Hero::getArmor(int index) const
{
	return armor[index];
}

Potion* Hero::getPotion(int index) const
{
	return potion[index];
}

Spell* Hero::getSpell(int index) const
{
	return spell[index];
}

Weapon* Hero::getWeaponEquipped() const
{
	return weapon_equipped;
}

Armor* Hero::getArmorEquipped() const
{
	return armor_equipped;
}

void Hero::buyItem(Item* item)
{
	switch (item->getType()) {
	case Item::WEAPON:
		weapon.push_back(dynamic_cast<Weapon*>(item));
		break;
	case Item::ARMOR:
		armor.push_back(dynamic_cast<Armor*>(item));
		break;
	case Item::POTION:
		potion.push_back(dynamic_cast<Potion*>(item));
		break;
	}
	money -= item->getPrice();
}

void Hero::buySpell(Spell* spell)
{
	this->spell.push_back(spell);
	money -= spell->getPrice();
}

bool Hero::sellItem(Item* item)
{
	bool found = false;
	size_t i;

	switch (item->getType()) {
	case Item::WEAPON:
		if (weapon.size() == 1)
			return false;

		for (i = 0; i < weapon.size(); ++i)
			if (weapon[i] == item) {
				found = true;
				break;
			}

		if (found) {
			weapon.erase(weapon.begin() +i);
			money += item->getSellValue();
		}

		break;
	case Item::ARMOR:
		for (i = 0; i < armor.size(); ++i)
			if (armor[i] == item) {
				found = true;
				break;
			}

		if (found) {
			armor.erase(armor.begin() +i);
			money += item->getSellValue();
		}

		break;
	case Item::POTION:
		for (i = 0; i < potion.size(); ++i)
			if (potion[i] == item) {
				found = true;
				break;
			}

		if (found) {
			potion.erase(potion.begin() +i);
			money += item->getSellValue();
		}

		break;
	}
	return true;
}

void Hero::sellSpell(Spell* spell)
{
	bool found = false;
	size_t i;

	for (i = 0; i < this->spell.size(); ++i)
		if (this->spell[i] == spell) {
			found = true;
			break;
		}

	if (found) {
		this->spell.erase(this->spell.begin() +i);
		money += spell->getSellValue();
	}
}

bool Hero::ownsItem(Item* item) const
{
	bool found = false;
	size_t i;

	switch (item->getType()) {
	case Item::WEAPON:
		for (i = 0; i < weapon.size(); ++i)
			if (weapon[i]->getName() == item->getName()) {
				found = true;
				break;
			}
		break;
	case Item::ARMOR:
		for (i = 0; i < armor.size(); ++i)
			if (armor[i]->getName() == item->getName()) {
				found = true;
				break;
			}
		break;
	case Item::POTION:
		for (i = 0; i < potion.size(); ++i)
			if (potion[i]->getName() == item->getName()) {
				found = true;
				break;
			}
		break;
	}

	return found;
}

bool Hero::ownsSpell(Spell* spell) const
{
	bool found = false;
	for (size_t i = 0; i < this->spell.size(); ++i)
		if (this->spell[i] == spell) {
			found = true;
			break;
		}

	return found;
}

bool Hero::isEquipped(Item* item) const
{
	return (item == weapon_equipped || item == armor_equipped);
}

bool Hero::equip(Item* item)
{
	if ((item->getType() == Item::POTION) ||
		(item->getHands() == 2 && armor_equipped) ||
		(weapon_equipped && weapon_equipped->getHands() == 2 &&
		 item->getType() == Item::ARMOR))
		return true;

	if (item->getType() == Item::WEAPON)
		weapon_equipped = dynamic_cast<Weapon*>(item);
	else if (item->getType() == Item::ARMOR)
		armor_equipped = dynamic_cast<Armor*>(item);

	return false;
}

void Hero::unequipArmor()
{
	armor_equipped = NULL;

}

void Hero::use(Item* item)
{
	string boost_field;
	bool found = false;
	size_t i;

	if (item->getType() != Item::POTION)
		return;

	for (i = 0; i < potion.size(); ++i)
		if (potion[i] == item) {
			found = true;
			break;
		}

	if (found) {
		boost_field = potion[i]->getBoostField();
		if (boost_field == "strength")
			strength += potion[i]->getBoost();
		else if (boost_field == "agility")
			agility += potion[i]->getBoost();
		else if (boost_field == "dexterity")
			dexterity += potion[i]->getBoost();

		potion.erase(potion.begin() +i);
	}
}

void Hero::regenerate()
{
	Living::regenerate();
	magicPower += magic_regenerate;
}

void Hero::revive()
{
	setHealth(50);
	money = money/2;
}

void Hero::getPrize()
{
	money += base_money_prize*getLevel();
	experience += base_exp_prize/getLevel();
	if (experience >= 100)
		levelUp();
}

bool Hero::attack(Monster* mon) const
{
	if (mon->evade())
		return false;

	mon->removeHealth(strength +weapon_equipped->getDamage() -mon->getDefense());
	return true;
}

bool Monster::attack(Hero* hero) const
{
	Armor* armor;

	if (hero->evade())
		return false;

	if ((armor = hero->getArmorEquipped())) {
		if (damage > armor->getProtection())
			hero->removeHealth(damage -armor->getProtection());
	}
	else
		hero->removeHealth(damage);

	return true;
}

bool Hero::evade() const
{
	if (randgen(1, 100) <= agility)
		return true;
	else
		return false;
}

bool Monster::evade() const
{
	if (randgen(1, 100) <= evade_pct)
		return true;
	else
		return false;
}

void Hero::levelUp()
{
	Living::levelUp();
	experience = 0;
}

void Warrior::levelUp()
{
	Hero::levelUp();
	setStrength(base_strength +extra_strength);
	setDexterity(base_dexterity);
	setAgility(base_agility +extra_agility);
}

void Sorcerer::levelUp()
{
	Hero::levelUp();
	setStrength(base_strength);
	setDexterity(base_dexterity +extra_agility);
	setAgility(base_agility +extra_agility);
}

void Paladin::levelUp()
{
	Hero::levelUp();
	setStrength(base_strength +extra_strength);
	setDexterity(base_dexterity +extra_dexterity);
	setAgility(base_agility);
}

inline NCURSES_ATTR_T Hero::getCursesAttr() const
{
	return nattr;
}

inline NCURSES_ATTR_T Hero::getCursesMenuAttr() const
{
	return nmattr;
}

inline NCURSES_ATTR_T Hero::getCursesNegMenuAttr() const
{
	return nnmattr;
}

void Hero::createInventoryMenus()
{
	inventory[0] = createWeaponMenu();
	inventory[1] = createArmorMenu();
	inventory[2] = createPotionMenu();
	inventory[3] = createSpellMenu();
}

void Hero::updateInventoryMenus()
{
	for (size_t i = 0; i < inventory_menus; ++i)
		delete inventory[i];

	createInventoryMenus();
}

MenuDialog* Hero::createWeaponMenu()
{
	vector<vector<string>> entry(weapon.size(), vector<string>(0));
	vector<string> entry_label;
	size_t i;

	for (i = 0; i < weapon.size(); ++i) {
		entry[i].push_back(weapon[i]->getName());
		entry[i].push_back(to_string(weapon[i]->getLevel()));
		entry[i].push_back(to_string(weapon[i]->getDamage()));
		entry[i].push_back(to_string(weapon[i]->getHands()));
		entry[i].push_back(to_string(weapon[i]->getSellValue()));
		if (isEquipped(weapon[i]))
			entry[i].push_back("Y");
		else
			entry[i].push_back("X");
	}

	entry_label.push_back("Name");
	entry_label.push_back("Level");
	entry_label.push_back("Damage");
	entry_label.push_back("Hands");
	entry_label.push_back("Value");
	entry_label.push_back("Equipped");

	return new MenuDialog(entry, entry_label, " Inventory - Weapons ", getCursesMenuAttr(),
	                      getCursesNegMenuAttr());

}

MenuDialog* Hero::createArmorMenu()
{
	vector<vector<string>> entry(armor.size(), vector<string>(0));
	vector<string> entry_label;
	size_t i;

	for (i = 0; i < armor.size(); ++i) {
		entry[i].push_back(armor[i]->getName());
		entry[i].push_back(to_string(armor[i]->getLevel()));
		entry[i].push_back(to_string(armor[i]->getProtection()));
		entry[i].push_back(to_string(armor[i]->getSellValue()));
		if (isEquipped(armor[i]))
			entry[i].push_back("Y");
		else
			entry[i].push_back("X");
	}

	entry_label.push_back("Name");
	entry_label.push_back("Level");
	entry_label.push_back("Protection");
	entry_label.push_back("Value");
	entry_label.push_back("Equipped");

	return new MenuDialog(entry, entry_label, " Inventory - Armor ", getCursesMenuAttr(),
	                      getCursesNegMenuAttr());
}

MenuDialog* Hero::createPotionMenu()
{
	vector<vector<string>> entry(potion.size(), vector<string>(0));
	vector<string> entry_label;
	size_t i;

	for (i = 0; i < potion.size(); ++i) {
		entry[i].push_back(potion[i]->getName());
		entry[i].push_back(to_string(potion[i]->getLevel()));
		entry[i].push_back(potion[i]->getBoostField());
		entry[i].push_back(to_string(potion[i]->getBoost()));
		entry[i].push_back(to_string(potion[i]->getSellValue()));
	}

	entry_label.push_back("Name");
	entry_label.push_back("Level");
	entry_label.push_back("Boost Field");
	entry_label.push_back("Boost (Pct)");
	entry_label.push_back("Value");

	return new MenuDialog(entry, entry_label, " Inventory - Potions ",
	                      getCursesMenuAttr(), getCursesNegMenuAttr());

}

MenuDialog* Hero::createSpellMenu()
{
	vector<vector<string>> entry(spell.size(), vector<string>(0));
	vector<string> entry_label;
	size_t i;


	for (i = 0; i < spell.size(); ++i) {
		entry[i].push_back(spell[i]->getName());
		entry[i].push_back(spell[i]->getType());
		entry[i].push_back(to_string(spell[i]->getLevel()));
		entry[i].push_back(to_string(spell[i]->getDamage()));
		entry[i].push_back(to_string(spell[i]->getEnergy()));
		entry[i].push_back(to_string(spell[i]->getTurns()));
		entry[i].push_back(to_string(spell[i]->getSellValue()));
	}

	entry_label.push_back("Name");
	entry_label.push_back("Type");
	entry_label.push_back("Level");
	entry_label.push_back("Damage");
	entry_label.push_back("Energy");
	entry_label.push_back("Turns");
	entry_label.push_back("Value");

	return new MenuDialog(entry, entry_label, " Inventory - Spells ", getCursesMenuAttr(),
						  getCursesNegMenuAttr());
}

