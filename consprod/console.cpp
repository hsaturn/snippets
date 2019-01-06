#include "console.h"
#include <iostream>
#include <ncurses.h>

mutex Window::lock;
bool Window::display_enabled=true;

int Window::createColorPair(int fg, int bg)
{
	static map<tuple<int,int>,int>	colorPairs;
	static mutex lck_pairs;
	lock_guard lock(lck_pairs);

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
	{
		lock_guard lck(lock);
		if (rows<3) rows=3;
		if (columns<3) columns=3;
		boxw = newwin(mrows, mcols, mtop, mleft); 
		win = newwin(mrows-2, mcols-2, mtop+1, mleft+1); 

		setBorderColor(color);
		scrollok(win, true);
		setTitle(title);
	}
	refresh();
}

void Window::setBorderColor(int color)
{
	mPair = createColorPair(color, COLOR_BLACK);
	refresh();
}

void Window::setTitle(const string& sTitle)
{
	msTitle=sTitle;
	refresh();
}

void Window::refresh()
{
	unique_lock lck(lock, try_to_lock);
	if (lck.owns_lock())
	{
		wbkgd(boxw, COLOR_PAIR(mPair));
		box(boxw, ACS_VLINE, ACS_HLINE);
		int x=(mcols-msTitle.length())/2;
		if (x<0) x=0;
		mvwprintw(boxw, 0, x, msTitle.c_str());
		wrefresh(boxw);
		touchwin(win);
		wrefresh(win);
	}
}

Window::~Window()
{
	lock_guard lck(lock);
	delwin(win);
	wclear(boxw);
	wrefresh(boxw);
	delwin(boxw);
}

Keys::Keys()
{
	thr=new thread(key_thread,this);
	thr->detach();
}

Keys::~Keys()
{
}

void Keys::stop()
{
	if (thr)
	{
		delete thr;
		thr=0;
	}
}

void Keys::key_thread(Keys* keys)
{
	for(;;)
	{
		char c;
		cin >> c;
		keys->push(c);
	}
}

void Keys::push(char c)
{
	lock_guard lock(mlock);
	keys.push(c);
}

char Keys::pop()
{
	lock_guard lock(mlock);
	if (keys.size())
	{
		char c(keys.front());
		keys.pop();
		return c;
	}
	return 0;
}
