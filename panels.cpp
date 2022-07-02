#include "panels.hpp"

using namespace std;

PanelManager::PanelManager() : index(0)
{
	panel[GRID] = new GridPanel();
	panel[MARKET_BUY] = new MarketBuyPanel();
	panel[MARKET_SELL] = new MarketSellPanel();
	panel[INVENTORY] = new InventoryPanel();
	panel[BATTLE] = new BattlePanel();
}

PanelManager::~PanelManager()
{
	for (size_t i = 0; i < panels; ++i)
		delete panel[i];
}

void PanelManager::add(Type type, WINDOW* win)
{
	panel[type]->add(win);
}

void PanelManager::switchTo(Type type, int i)
{
	if (index != GRID)
		hide();
	index = type;

	show(i);
}

bool PanelManager::is(Type type)
{
	return (index == type);
}

int PanelManager::getchar()
{
	return panel[index]->getchar();
}

size_t PanelManager::getIndex()
{
	return panel[index]->getIndex();
}

void PanelManager::clear()
{
	return panel[index]->clear();
}

void PanelManager::show(int i)
{
	return panel[index]->show(i);
}

void PanelManager::hide()
{
	return panel[index]->hide();
}

void PanelManager::next()
{
	hide();
	panel[index]->next();
	show();
}

void PanelManager::prev()
{
	hide();
	panel[index]->prev();
	show();
}

Panel::~Panel()
{
	for (size_t i = 0; i < pwin.size(); ++i)
		del_panel(pwin[i].panel);
}

void Panel::add(WINDOW* win)
{
	PanelWindow pwin_temp;
	pwin_temp.panel = new_panel(win);
	pwin_temp.win = win;
	pwin.push_back(pwin_temp);
	hide_panel(pwin_temp.panel);
}

int Panel::getchar()
{
	int ch = wgetch(pwin[index].win);
	return ch;
}

size_t Panel::getIndex()
{
	if (pwin.size() == 0)
		return -1;
	else
		return index;
}

void Panel::clear()
{
	for (size_t i = 0; i < pwin.size(); ++i) {
		index = i;
		hide();
		del_panel(pwin[index].panel);
	}
	pwin.clear();
}

void Panel::show(int i)
{
	if (i != -1)
		index = i;

	top_panel(pwin[index].panel);
	update_panels();
	doupdate();
}

void Panel::hide()
{
	hide_panel(pwin[index].panel);
	update_panels();
	doupdate();
}

void Panel::next()
{
	if (index == pwin.size() -1)
		index = 0;
	else
		index++;
}

void Panel::prev()
{
	if (index == 0)
		index = pwin.size() -1;
	else
		index--;
}

void GridPanel::show(int i)
{
	Panel::show(i);
	curs_set(1);
}

void MarketBuyPanel::show(int i)
{
	Panel::show(i);
	curs_set(0);
}

void MarketSellPanel::show(int i)
{
	Panel::show(i);
	curs_set(0);
}

void InventoryPanel::show(int i)
{
	Panel::show(i);
	curs_set(0);
}

void BattlePanel::show(int i)
{
	Panel::show(i);
	curs_set(0);
}
