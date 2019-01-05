#pragma once

#include <string>
#include <map>
#include <tuple>
#include <mutex>
#include <sstream>
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


		static int createColorPair(int, int);
		static void toggleDisplay() { enable_display = !enable_display; }
		
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
		static mutex lock;
		static map<tuple<int,int>,int>	colorPairs;
		static bool enable_display;
};

template<typename Type>
Window& operator<<(Window& win, const Type& value)
{
	if (!win.mforceDisplay && !Window::enable_display) return win;
	stringstream s;
	s << value;
	// mvwprintw(win.win, win.x+1, win.y+1, s.str().c_str());

	lock_guard<mutex> lck(Window::lock);
	for(char c : s.str())
	{
		waddch(win.win,c);
	}
	wrefresh(win.win);

	return win;
}

