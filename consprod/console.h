#pragma once

#include <string>
#include <map>
#include <tuple>
#include <mutex>
#include <sstream>
#include <list>
#include <thread>
#include <queue>
#include <ncurses.h>

using namespace std;

class Window
{
	public:
		using pos=unsigned int;
		enum class chars : char
		{
			endl='\n'
		};

		Window(pos top, pos left, pos rows, pos columns, const string& title="",int color=COLOR_WHITE); 
		~Window();

		template<typename Type>
		friend Window& operator << (Window&, const Type&);
		friend Window& operator << (Window&, const chars& c);

		void setTitle(const string&);
		void setBorderColor(int color);
		void forceDisplay() { mforceDisplay=true; }
		void clear() { werase(win); wrefresh(win); }

		pos top() const { return mtop; }
		pos left() const { return mleft; }

		void refresh();

		static int createColorPair(int, int);
		static bool toggleDisplay() { display_enabled = !display_enabled; return display_enabled; }
		static bool isDisplayEnabled() { return display_enabled; }
		
	private:
		WINDOW *win;
		WINDOW *boxw; // ugly use decorator instead
		pos mtop;
		pos mleft;
		pos mrows;
		pos mcols;
		pos x;
		pos y;
		bool mforceDisplay;
		int mPair;
		string msTitle;
		static mutex lock;
		static map<tuple<int,int>,int>	colorPairs;
		static bool display_enabled;
};

template<typename Type>
Window& operator<<(Window& win, const Type& value)
{
	if (!win.mforceDisplay && !Window::display_enabled) return win;
	stringstream s;
	s << value;
	if (s.str().length() )
	{
		// mvwprintw(win.win, win.x+1, win.y+1, s.str().c_str());

		lock_guard lck(Window::lock);
		for(char c : s.str())
		{
			waddch(win.win,c);
		}
		wrefresh(win.win);
	}

	return win;
}

struct xy
{
	xy() { getmaxyx(stdscr, y,x); }
	int x;
	int y;
};


class WindowPlacer
{
	public:
		using pos=Window::pos;
		using placementType = tuple<Window::pos, Window::pos>;
		WindowPlacer(pos width, pos height, pos top)
		:
		mwidth(width), mh(height),
		mcurx(0), mcury(top)
		{
		}

		pos left() const {
			if (mfreeSpace.size()) return std::get<1>(mfreeSpace.back());
			return mcurx;
		}

		pos top() const {
			if (mfreeSpace.size()) return std::get<0>(mfreeSpace.back());
			return mcury;
		}

		string title(const string& sClass)
		{
			mClasses[sClass]++;
			stringstream s;
			s << sClass << ' ' << mClasses[sClass];
			return s.str();
		}
		
		void next()
		{
			if (mfreeSpace.size())
			{
				mfreeSpace.pop_back();
				return;
			}
			xy max;
			mcurx += mwidth+1;
			if ((int)(mcurx+mwidth) > max.x)
			{
				mcurx=0;
				mcury += mh;
			}
		}

		void freeSpace(Window* win)
		{
			mfreeSpace.push_back({win->top(), win->left()});
			mfreeSpace.sort();
			mfreeSpace.reverse();
		}

	private:
		pos mwidth, mh;
		pos mcurx, mcury;
		map<string, int> mClasses;
		list<placementType> mfreeSpace;

};



class Keys
{
	public:
		Keys();
		~Keys();

		char pop();
		void push(char c);
		void stop();

		static void key_thread(Keys*);

	private:
		mutex mlock;
		thread* thr;
		queue<char>	keys;
};

