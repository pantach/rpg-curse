#include <cstdio>
#include <cstdlib>
#include <termios.h>
#include <ctime>
#include <unistd.h>
#include "tools.hpp"

using namespace std;

/* Max should be >= min */
int randgen(int min, int max)
{
	int randnum;
	static int firstcall = 1;

	if (firstcall == 1) {
		srand48(time(NULL));
		firstcall = 0;
	}

	max -= min;
	randnum = lrand48()%(max+1) + min;

	return randnum;
}

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

int circleNum(int num, int start, int end)
{
	if (num == end)
		return start;
	else
		return num++;
}

int circleNumNeg(int num, int start, int end)
{
	if (num == start)
		return end;
	else
		return num--;
}

