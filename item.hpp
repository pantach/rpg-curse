#ifndef ITEM_H
#define ITEM_H

#include <string>

class Item {
	std::string name;
	int price;
	int lvl;

public:

	enum Type {
		WEAPON = 0,
		ARMOR,
		POTION
	};

	Item(std::string nam, int prc, int lvl)
		: name(nam), price(prc), lvl(lvl) {};

	virtual Type getType() const = 0;
	virtual int getHands() const = 0;
	std::string getName() const { return name; };
	int getPrice()     const { return price; };
	int getSellValue() const { return price/2; };
	int getLevel()     const { return lvl; };
};

class Weapon: public Item {
	int damage;
	int hands;

public:
	Weapon(std::string nam, int prc, int lvl, int dmg, int bh)
		: Item(nam, prc, lvl), damage(dmg), hands(bh) {};

	Type getType()   const { return WEAPON; };
	int  getDamage() const { return damage; };
	int  getHands()  const { return hands; };
};

class Armor: public Item {
	int protection;

public:
	Armor(std::string nam, int prc, int lvl, int pr)
		: Item(nam, prc, lvl), protection(pr) {};

	Type getType()       const { return ARMOR; };
	int  getProtection() const { return protection; };
	int  getHands()      const { return 1; };
};

class Potion: public Item {
	std::string boost_field;
	int boost;

public:
	Potion(std::string nam, int prc, int lvl, std::string bfield, int b)
		: Item(nam, prc, lvl), boost_field(bfield), boost(b) {};

	Type getType() const { return POTION; };
	std::string getBoostField() const { return boost_field; };
	int  getBoost() const { return boost; };
	int  getHands() const { return 0; };
};

class Spell {
	std::string name;
	int price;
	int lvl;
	int damage;
	int energy;
	int turns;

public:
	enum Type {
		ICE = 0,
		FIRE,
		LIGHTNING
	};

	Spell(std::string nam, int prc, int lvl, int dmg, int nrg, int tur) :
		name(nam), price(prc), lvl(lvl), damage(dmg), energy(nrg), turns(tur) {};

	virtual std::string getType() = 0;
	std::string getName() const { return name; };
	int getPrice()     const { return price; };
	int getSellValue() const { return price/2; };
	int getLevel()     const { return lvl; };
	int getDamage()    const { return damage; };
	int getEnergy()    const { return energy; };
	int getTurns()     const { return turns; };
};

class IceSpell: public Spell {
public:
	IceSpell(std::string nam, int prc, int lvl, int dmg, int nrg, int tur)
		: Spell(nam, prc, lvl, dmg, nrg, tur) {};

	std::string getType() { return "Ice Spell"; };
};

class FireSpell: public Spell {
public:
	FireSpell(std::string nam, int prc, int lvl, int dmg, int nrg, int tur)
		: Spell(nam, prc, lvl, dmg, nrg, tur) {};

	std::string getType() { return "Fire Spell"; };
};

class LightningSpell: public Spell {
public:
	LightningSpell(std::string nam, int prc, int lvl, int dmg, int nrg, int tur)
		: Spell(nam, prc, lvl, dmg, nrg, tur) {};

	std::string getType() { return "Lightning Spell"; };
};

#endif
