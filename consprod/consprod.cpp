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

		static int createColorPair(int, int);

	private:
		void refresh();

		WINDOW *win;
		WINDOW *boxw; // ugly use decorator instead
		pos mtop;
		pos mleft;
		pos mrows;
		pos mcols;
		pos x;
		pos y;
		int mPair;
		static mutex lock;
		static map<tuple<int,int>,int>	colorPairs;
};

mutex Window::lock;
map<tuple<int,int>,int> Window::colorPairs;

int Window::createColorPair(int fg, int bg)
{
	auto it=colorPairs.find({fg,bg});
	if (it == colorPairs.end())
	{
		int pair = colorPairs.size()+1;
		init_pair(pair, fg,bg);
		colorPairs[{fg,bg}]=pair;
		cerr << "pair(" << fg << ',' << bg << ") created, newsize=" << colorPairs.size() << endl;
		return pair;
	}
	cerr << "pair(" << fg << ',' << bg << ") found=" << it->second << endl;
	return it->second;
}

template<typename Type>
Window& operator<<(Window& win, const Type& value)
{
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
mtop(top), mleft(left), mrows(rows), mcols(columns), x(0), y(0)
{
	mPair = createColorPair(color, COLOR_BLACK);
	cerr << "pair(" << color << ")=" << mPair << endl;
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
	mvwprintw(boxw, 0, x, sTitle.c_str());
	refresh();
}


void Window::refresh()
{
	wattron(boxw, COLOR_PAIR(mPair));
	box(boxw, ACS_VLINE, ACS_HLINE);
	wbkgd(boxw, COLOR_PAIR(mPair));
	wrefresh(boxw);
	wrefresh(win);
}

Window::~Window()
{
	lock_guard<mutex> lck(lock);
	delwin(win);
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
		WindowPlacer(pos width, pos height, pos top)
		:
		mw(width), mh(height),
		mcurx(0), mcury(top)
		{
		}

		pos left() const { return mcurx; }
		pos top() const { return mcury; }

		string title(const string& sClass)
		{
			mClasses[sClass]++;
			stringstream s;
			s << sClass << ' ' << mClasses[sClass];
			return s.str();
		}
		
		void next()
		{
			xy max;
			mcurx += mw+1;
			if (mcurx+width > max.x)
			{
				mcurx=0;
				mcury += mh;
			}
		}

	private:
		pos mw, mh;
		pos mcurx, mcury;
		map<string, int> mClasses;

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
			); // TODO keep thread to join it
		} 

		~WindowedThread()
		{
			stop();
			join();
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

int main(int argc, const char* argv[])
{
	initscr();
	start_color();
	xy max;
	Window status(0,0, status_height,max.x, "Status", COLOR_GREEN);

	LockQueue<Document>	input;
	list<WindowedThread<Consumer<Document>, LockQueue<Document>>*> listConsumers;
	list<WindowedThread<Producer<Document>, LockQueue<Document>>*> listProducers;

	status << "Threads are running together..." << Window::chars::endl;
	bool inside(true);
	while(inside)
	{
		char c;
		cin >> c;
		switch(c)
		{
			case 'c':
				listConsumers.push_back(new WindowedThread<Consumer<Document>, LockQueue<Document>>("consumer", input, COLOR_CYAN));
				status << "New consumer " << listConsumers.size() << Window::chars::endl;
				break;

			case 'p':
				listProducers.push_back(new WindowedThread<Producer<Document>, LockQueue<Document>>("producer", input, COLOR_RED));
				status << "New producer " << listProducers.size() << Window::chars::endl;
				break;

			case 'h':
				status << "Mini help" << Window::chars::endl;
				status << "  c  new consumer" << Window::chars::endl;
				status << "  p  new producer" << Window::chars::endl;
				status << "  n  toggle nop display" << Window::chars::endl;
				status << "  s  stats" << Window::chars::endl;
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

	endwin();

	return 0;
}


