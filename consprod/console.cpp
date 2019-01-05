#include "console.h"
#include <ncurses.h>

mutex Window::lock;
map<tuple<int,int>,int> Window::colorPairs;
bool Window::enable_display=true;

int Window::createColorPair(int fg, int bg)
{
	auto it=colorPairs.find({fg,bg});
	if (it == colorPairs.end())
	{
		int pair = colorPairs.size()+1;
		init_pair(pair, fg,bg);
		colorPairs[{fg,bg}]=pair;
		return pair;
	}
	return it->second;
}


Window& operator<<(Window& win, const Window::chars& c)
{
	return win << (char)c;
}

Window::Window(pos top, pos left, pos rows, pos columns, const string& title, int color)
:
mtop(top), mleft(left), mrows(rows), mcols(columns), x(0), y(0),
mforceDisplay(false)
{
	mPair = createColorPair(color, COLOR_BLACK);
	if (rows<3) rows=3;
	if (columns<3) columns=3;
	lock_guard<mutex> lck(lock);
	boxw = newwin(mrows, mcols, mtop, mleft); 
	win = newwin(mrows-2, mcols-2, mtop+1, mleft+1); 

	scrollok(win, true);
	setTitle(title);
}

void Window::setTitle(const string& sTitle)
{
	int x=(mcols-sTitle.length())/2;
	if (x<0) x=0;
	wattron(boxw, COLOR_PAIR(mPair));
	box(boxw, ACS_VLINE, ACS_HLINE);
	mvwprintw(boxw, 0, x, sTitle.c_str());
	wbkgd(boxw, COLOR_PAIR(mPair));
	wrefresh(boxw);
	wrefresh(win);
}

Window::~Window()
{
	lock_guard<mutex> lck(lock);
	delwin(win);
	wclear(boxw);
	wrefresh(boxw);
	delwin(boxw);
}

