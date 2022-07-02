#ifndef PANELS_H
#define PANELS_H

#include <vector>
#include <ncurses.h>
#include <panel.h>
#include "curses_tools.hpp"

class Panel;

class PanelManager {
	static const size_t panels = 5;
	Panel* panel[panels];
	size_t index;

public:
	enum Type {
		GRID = 0,
		MARKET_BUY,
		MARKET_SELL,
		INVENTORY,
		BATTLE
	};

	PanelManager();
	~PanelManager();

	void add(Type, WINDOW*);
	void switchTo(Type, int i = -1);
	bool is(Type);
	int  getchar();
	size_t getIndex();
	void clear();
	void show(int i = -1);
	void hide();
	void next();
	void prev();
};

class Panel {
	class PanelWindow {
	public:
		PANEL* panel;
		WINDOW* win;
	};

	std::vector<PanelWindow> pwin;
	size_t index;

public:
	Panel() : index(0) {};
	virtual ~Panel();

	virtual void add(WINDOW* win);
	virtual int  getchar();
	virtual size_t getIndex();
	virtual void clear();

	virtual void show(int i = -1);
	virtual void hide();
	virtual void next();
	virtual void prev();
};

class GridPanel : public Panel {
public:
	void show(int i = -1);
};

class MarketBuyPanel : public Panel {
public:
	void show(int i = -1);
};

class MarketSellPanel : public Panel {
public:
	void show(int i = -1);
};

class InventoryPanel : public Panel {
public:
	void show(int i = -1);
};

class BattlePanel : public Panel {
public:
	void show(int i = -1);
};

#endif
