CC = g++
LDFLAGS = -lpanel -lmenu -lncurses -lpthread

rpg-curse: rpg.o curses_tools.o item.o living.o panels.o square.o tools.o
	$(CC) -o $@ $^ $(LDFLAGS)

rpg.o: rpg.cpp fast-cpp-csv-parser/csv.h curses_tools.hpp game_opt.hpp \
 tools.hpp rpg.hpp square.hpp item.hpp living.hpp panels.hpp
curses_tools.o: curses_tools.cpp curses_tools.hpp game_opt.hpp tools.hpp
item.o: item.cpp
living.o: living.cpp living.hpp item.hpp curses_tools.hpp game_opt.hpp \
 tools.hpp
panels.o: panels.cpp panels.hpp curses_tools.hpp game_opt.hpp
square.o: square.cpp square.hpp game_opt.hpp item.hpp living.hpp \
 curses_tools.hpp
tools.o: tools.cpp tools.hpp

.PHONY: clean
clean:
	rm -rf rpg-curse *.o
