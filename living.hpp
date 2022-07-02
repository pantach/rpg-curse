#ifndef LIVING_H
#define LIVING_H

#include <string>
#include <vector>
#include <ncurses.h>
#include "item.hpp"
#include "curses_tools.hpp"

// Forwaqrd declarations
class Monster;

class Living {
private:
	std::string name;
	int lvl;
	int healthPower;

	static const int health_regenerate = 10;
public:
	Living(std::string name, int lvl, int h)
		: name(name), lvl(lvl), healthPower(h) {};

	std::string getName() const;
	int getLevel() const;
	int getHealth() const;
	bool isDead() const;

	void setHealth(int h);
	virtual void removeHealth(int h);
	virtual bool evade() const = 0;
	virtual void regenerate();
	virtual void levelUp();
};

class Hero: public Living {
	friend class Game;

public:
	struct Coord {
		int y;
		int x;
	};

private:
	int magicPower;
	int strength;
	int dexterity;
	int agility;
	int money;
	int experience;
	Coord coord;

	std::vector<Weapon*> weapon;
	std::vector<Armor*>  armor;
	std::vector<Potion*> potion;
	std::vector<Spell*>  spell;

	Weapon* weapon_equipped;
	Armor*  armor_equipped;

	enum Inventory_Index {
		INDEX_WEAPONS = 0,
		INDEX_ARMOR,
		INDEX_POTIONS,
		INDEX_SPELLS
	};

	static const int magic_regenerate = 20;
	static const int base_money_prize = 20;
	static const int base_exp_prize = 20;
protected:
	static const int base_magicPower = 20;
	static const int base_strength = 20;
	static const int base_dexterity = 20;
	static const int base_agility = 20;
	static const int base_money = 50;

private:
	// Curses
	static const size_t inventory_menus = 4;
	class Curses::MenuDialog* inventory[inventory_menus];

	void createInventoryMenus();
	void updateInventoryMenus();
	Curses::MenuDialog* createWeaponMenu();
	Curses::MenuDialog* createArmorMenu();
	Curses::MenuDialog* createPotionMenu();
	Curses::MenuDialog* createSpellMenu();

	static const NCURSES_ATTR_T nattr   = COLOR_PAIR(GameOpt::RED_DEFAULT);
	static const NCURSES_ATTR_T nmattr  = COLOR_PAIR(GameOpt::WHITE_BLUE)|WA_BOLD;
	static const NCURSES_ATTR_T nnmattr = COLOR_PAIR(GameOpt::BLACK_BLUE)|WA_BOLD;

	NCURSES_ATTR_T getCursesAttr() const;
	NCURSES_ATTR_T getCursesMenuAttr() const;
	NCURSES_ATTR_T getCursesNegMenuAttr() const;

public:
	enum Category {
		WARRIOR = 0,
		SORCERER,
		PALADIN
	};

	Hero(std::string name, int pow, int str, int dex, int agi, Weapon* weap);

	void  setCoord(int y, int x);
	Coord getCoord() const;

	int getMagicPower() const;
	int getStrength() const;
	int getDexterity() const;
	int getAgility() const;
	int getMoney() const;
	int getExperience() const;

	void setStrength(int val);
	void setDexterity(int val);
	void setAgility(int val);

	Weapon* getWeapon(int index) const;
	Armor*  getArmor(int index) const;
	Potion* getPotion(int index) const;
	Spell*  getSpell(int index) const;
	Weapon* getWeaponEquipped() const;
	Armor* getArmorEquipped() const;

	void buyItem(Item* item);
	void buySpell(Spell* spell);
	bool sellItem(Item* item);
	void sellSpell(Spell* spell);
	bool ownsItem(Item* item) const;
	bool ownsSpell(Spell* spell) const;
	bool isEquipped(Item* item) const;

	bool equip(Item* item);
	void unequipArmor();
	void use(Item* item);

	bool attack(Monster* mon) const;
	bool evade() const;
	void regenerate();
	void revive();
	void getPrize();
	virtual void levelUp();
};

class Warrior: public Hero {
	static const int extra_strength = 7;
	static const int extra_agility = 6;

public:
	Warrior(std::string name, Weapon* weap)
		: Hero(name, base_magicPower, base_strength +extra_strength, base_dexterity,
		       base_agility +extra_agility, weap) {};

	void levelUp();
};

class Sorcerer: public Hero {
	static const int extra_dexterity = 8;
	static const int extra_agility = 5;

public:
	Sorcerer(std::string name, Weapon* weap)
		: Hero(name, base_magicPower, base_strength, base_dexterity +extra_dexterity,
		       base_agility +extra_agility, weap) {};

	void levelUp();
};

class Paladin: public Hero {
	static const int extra_strength = 6;
	static const int extra_dexterity = 7;

public:
	Paladin(std::string name, Weapon* weap)
		: Hero(name, base_magicPower, base_strength +extra_strength, base_dexterity
		       +extra_dexterity, base_agility, weap) {};

	void levelUp();
};

class Monster: public Living {
	int damage;
	int defense;
	int evade_pct;

protected:
	static const int base_damage = 10;
	static const int base_defense = 10;
	static const int base_evade_pct = 20;
	static const int max_health = 40;

public:
	enum Category {
		DRAGON = 0,
		EXOSKELETON,
		SPIRIT,
		CATEGORIES_TOTAL
	};

	Monster(std::string name, int lvl, int h, int dam, int def, int eva)
		: Living(name, lvl, h), damage(dam*lvl), defense(def*lvl), evade_pct(eva +lvl*2) {};
	virtual ~Monster() {};

	int getDamage()  const { return damage; };
	int getDefense() const { return defense; };
	int getEvade()   const { return evade_pct; };

	virtual std::string getCategory() const = 0;
	bool attack(Hero* hero) const;
	bool evade() const;
};

class Dragon: public Monster {
	static const int extra_damage = 5;

public:
	Dragon(std::string name, int lvl)
		: Monster(name, lvl, max_health, base_damage +extra_damage, base_defense,
		          base_evade_pct) {};

	std::string getCategory() const { return "Dragon"; };
};

class Exoskeleton: public Monster {
	static const int extra_defense = 7;

public:
	Exoskeleton(std::string name, int lvl)
		: Monster(name, lvl, max_health, base_damage, base_defense +extra_defense,
		          base_evade_pct) {};

	std::string getCategory() const { return "Exoskeleton"; };
};

class Spirit: public Monster {
	static const int extra_evade_pct = 3;

public:
	Spirit(std::string name, int lvl)
		: Monster(name, lvl, max_health, base_damage, base_defense, base_evade_pct
		          +extra_evade_pct) {};

	std::string getCategory() const { return "Spirit"; };
};

#endif
