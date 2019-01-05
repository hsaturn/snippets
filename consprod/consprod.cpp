#include <stdlib.h>
#include <iostream>
#include <list>
#include <map>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <chrono>
#include <sstream>
#include <tuple>
#include <algorithm>
#include <ncurses.h>

// TODO LIST
// - limit max size of the queue
// - use a conditional var and avoid thread_sleep_for

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

template<typename Type>
class Optional
{
	public:
		Optional() : mSet(false) { }
		Optional(Type value) : mSet(true), mValue(value) { }
		explicit operator bool() const { return mSet; }

		const Type& get() const
		{
			if (mSet) return mValue;
			throw string("Getting an undefined value");
		}

		template <typename T>
		friend ostream& operator<< (ostream& out, const Optional<T>&);

		Optional<Type> operator=(const Type value)
		{
			mSet = true; mValue = value; return *this;
		}

		Optional<Type> operator=(const Optional<Type>& opt)
		{
			mSet = opt.mSet; mValue = opt.value; return *this;
		}


	
	private:
		bool mSet;
		Type mValue;
};

template<typename Type, typename Container=std::deque<Type>>
class LockQueue : protected queue<Type, Container>
{
	protected:
		using parent = queue<Type, Container>;
		using iterator = typename Container::iterator;
		using const_iterator = typename Container::const_iterator;

		iterator begin() { return this->c.begin(); }
		iterator end()   { return this->c.end(); }
		const_iterator   begin() const { return this->c.begin(); }
		const_iterator   end()   const { return this->c.end(); }

	public:

		const Optional<Type> pop()
		{
			lock_guard<mutex> lock(mlock);
			if (this->size_nolock())
			{
				Optional<Type> t=this->back();
				parent::pop();
				return t;
			}
			else
			{
				return Optional<Type>();
			}
		}

		template<typename T, typename container=std::deque<T>>
		friend ostream& operator<< (ostream& out, const LockQueue<T,container>& input)
		{
			size_t size=input.size();
			lock_guard<mutex> lock(input.mlock);
			for(const auto& document: input)
			{
				out << document.i() << ' ';
			}
			out << (size ? ", " : "") << "size=" << size << endl;
			return out;
		}

		void push(Type t)
		{
			lock_guard<mutex> lock(mlock);
			parent::push(t);
		}

		size_t size() const
		{
			lock_guard<mutex> lock(mlock);
			return parent::size();
		}

	protected:
		size_t size_nolock() const
		{
			return parent::size();
		}

	private:
		mutable mutex mlock;
};

template<typename DocumentType>
class IDocument
{
	public:
		static DocumentType generateRandom();
};

class Document : public IDocument<Document>
{
	public:
		Document() : mi(0) {}
		Document(int i) : mi(i) {}

		int i() const { return mi; }

		friend Window& operator << (Window& out, const Document& doc);

		static Document generateRandom()
		{
			return rand() % 1000;
		}

	private:
		int mi;
};

template<typename DocumentType>
class Producer
{
		using QueueType = LockQueue<DocumentType>;
	public:
		Producer(Window& win, QueueType& queue, unsigned int period_min=10, unsigned int period_max=20);

		void stop() { active=false; }

		void work();

	private:
		Window& mwin;
		QueueType& mQueue;
		bool active;
		unsigned int period_min;
		unsigned int period_max;
};

template<typename DocumentType>
Producer<DocumentType>::Producer(Window& win, QueueType& queue, unsigned int pmin, unsigned int pmax)
:
mwin(win),
mQueue(queue),
active(true),
period_min(pmin),
period_max(pmax)
{
}

template<typename DocumentType>
void Producer<DocumentType>::work()
{
	while(active)
	{
		DocumentType newDoc = DocumentType::generateRandom();
		mQueue.push(newDoc);

		unsigned int wait_ms=period_min+(rand()%(period_max-period_min));
		mwin << newDoc << ' ';
		//mwin << "wait for " << wait_ms << "ms. size=" << mQueue.size() << Window::chars::endl;
		this_thread::sleep_for(std::chrono::milliseconds(wait_ms));
	}
}

template<typename DocumentType>
class Consumer
{
		using QueueType = LockQueue<DocumentType>;
	public:
		Consumer(Window& win, QueueType& mQueue);

		void work();
		void stop() { active=false; }
		static void toggleNop() { nop = !nop; }


	private:
		Window& mwin;
		QueueType& mQueue;
		bool active;
		static bool nop;
};

template<typename DocumentType>
bool Consumer<DocumentType>::nop=true;


template<typename DocumentType>
Consumer<DocumentType>::Consumer(Window& win, QueueType& queue)
:
mwin(win),
mQueue(queue),
active(true)
{}

template<typename DocumentType>
void Consumer<DocumentType>::work()
{
	unsigned long wait_ms=5;
	while(active)
	{
		this_thread::sleep_for(std::chrono::milliseconds(wait_ms));
		Optional<DocumentType> doc = mQueue.pop();
		if (doc)
		{
			mwin << doc.get() << ' ';
		}
		else if (nop)
		{
			mwin << "nop ";
		}
	}
	mwin << "Consumer end..." << Window::chars::endl;
}

Window& operator<< (Window& win, const Document& doc) 
{
	return win << doc.mi;
}

template<typename T, typename Container>
void dumpQueue(const LockQueue<T,Container>& input, string header="Queue: ")
{
}

struct xy
{
	xy() { getmaxyx(stdscr, y,x); }
	int x;
	int y;
};

const int width=15;
const int height=10;
const int status_height = 12;

class WindowPlacer
{
	public:
		using pos=Window::pos;
		using placementType = tuple<Window::pos, Window::pos>;
		WindowPlacer(pos width, pos height, pos top)
		:
		mw(width), mh(height),
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
			mcurx += mw+1;
			if ((int)(mcurx+width) > max.x)
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
		pos mw, mh;
		pos mcurx, mcury;
		map<string, int> mClasses;
		list<placementType> mfreeSpace;

};
WindowPlacer placer(width,height, status_height);


template<typename ThreadClass, typename Queue>
class WindowedThread
{
	public:
		WindowedThread(string title, Queue& input, int color)
		{
			mwin = new Window(placer.top(), placer.left(), height, width, placer.title(title),color);
			mthreadClass = new ThreadClass(*mwin, input);
			placer.next();
			mpthread = new thread(
				[this]()
				{ this->mthreadClass->work(); }
			); // use smart pointers
		} 

		~WindowedThread()
		{
			stop();
			join();
			placer.freeSpace(mwin);
			delete mwin;
			delete mthreadClass;
		}

		void stop()
		{
			mthreadClass->stop();
		}

		void join()
		{
			mpthread->join();
		}

	private:
		ThreadClass* mthreadClass;
		Window* mwin;
		thread* mpthread;
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
	lock_guard<mutex> lock(mlock);
	keys.push(c);
}

char Keys::pop()
{
	lock_guard<mutex> lock(mlock);
	if (keys.size())
	{
		char c(keys.front());
		keys.pop();
		return c;
	}
	return 0;
}

Keys keys;
void mainLoop()
{
	xy max;
	Window status(0,0, status_height,max.x, "Status", COLOR_GREEN);
	status.forceDisplay();

	LockQueue<Document>	input;
	list<WindowedThread<Consumer<Document>, LockQueue<Document>>*> listConsumers;
	list<WindowedThread<Producer<Document>, LockQueue<Document>>*> listProducers;

	status << "Threads are running together..." << Window::chars::endl;
	bool inside(true);
	const long poll_key_interval=1;
	const long stats_interval=1000;
	unsigned long stats=0;


	keys.push('h');
	while(inside)
	{
		char c(keys.pop());
		this_thread::sleep_for(std::chrono::milliseconds(poll_key_interval));
		if (stats)
		{
			stats-=poll_key_interval;
			if (stats<=0)
			{
				keys.push('s');
				stats+=stats_interval;
			}
		}
		switch(c)
		{
			case 0:
				break;

			case 'c':
				listConsumers.push_back(new WindowedThread<Consumer<Document>, LockQueue<Document>>("Consumer", input, COLOR_CYAN));
				status << "New consumer " << listConsumers.size() << Window::chars::endl;
				break;

			case 'p':
				listProducers.push_back(new WindowedThread<Producer<Document>, LockQueue<Document>>("Producer", input, COLOR_RED));
				status << "New producer " << listProducers.size() << Window::chars::endl;
				break;

			case 'd':
				Window::toggleDisplay();
				break;

			case 'S':
				if (stats)
				{
					stats=0;
				}
				else
				{
					stats=stats_interval;
				}
				break;

			case 'C':
				if (listConsumers.size())
				{
					delete listConsumers.back();
					listConsumers.pop_back();
				}
				else
					status << "No more consumer." << Window::chars::endl;
				break;

			case 'P':
				if (listProducers.size())
				{
					delete listProducers.back();
					listProducers.pop_back();
				}
				else
					status << "No more producer." << Window::chars::endl;
				break;

			case 'h':
				status << "Help" << Window::chars::endl;
				status << "  c  new consumer        p  new producter" << Window::chars::endl;
				status << "  C  remove one consumer P  remove one producter" << Window::chars::endl;
				status << "  d  toggle display      t  toggle nops display" << Window::chars::endl;
				status << "  s  stats               S  toggle continuous stats" << Window::chars::endl;
				status << "  q  quit" << Window::chars::endl;
				status << Window::chars::endl;
				break;

			case 's':
				status << "Queue size: " << input.size() << ", ";
				status << "producers: " << listProducers.size() << ", ";
				status << "consumers: " << listConsumers.size() << '.' << Window::chars::endl;
				break;

			case 'n':
				Consumer<Document>::toggleNop();
				break;

			case 'q':
				inside=false;
				break;

			default:
				status << "Unknown command " << c << Window::chars::endl;
				break;
		}
	}
	for(auto pconsumer : listConsumers)
	{
		delete pconsumer;
	}
	for(auto pproducer: listProducers)
	{
		delete pproducer;
	}
}


int main(int argc, const char* argv[])
{
	initscr();
	start_color();
	mainLoop();
	keys.stop();
	endwin();
}
