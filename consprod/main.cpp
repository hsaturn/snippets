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
#include <atomic>
#include <condition_variable>
#include "console.h"
#include "optional.h"

// TODO remove theses
static int speed_consumer=10;
static int speed_producer=10;

// TODO LIST
// - limit max size of the queue

using namespace std;

template<typename Type, typename Container=std::deque<Type>>
class LockQueue : protected queue<Type, Container>
{
	protected:
		using parent = queue<Type, Container>;

	public:

		const Optional<Type> pop()
		{
			lock_guard lock(mlock);
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
			lock_guard lock(input.mlock);
			for(const auto& document: input)
			{
				out << document.i() << ' ';
			}
			out << (size ? ", " : "") << "size=" << size << endl;
			return out;
		}

		size_t push(Type t)
		{
			lock_guard lock(mlock);
			parent::push(t);
			return parent::size();
		}

		size_t size() const
		{
			lock_guard lock(mlock);
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

class Document
{
	public:
		Document() : mi(0) {}
		Document(int i) : mi(i) {}

		int i() const { return mi; }

		friend Window& operator << (Window& out, const Document& doc);

		static Document next()
		{
			return msi++;
			//return rand() % 1000;
		}

	private:
		int mi;
		static atomic<int> msi;
};

atomic<int> Document::msi=0;

template<typename DocumentType>
class Producer
{
		using QueueType = LockQueue<DocumentType>;
	public:
		Producer(Window& win, QueueType& queue, unsigned int period_min=10, unsigned int period_max=20);

		void stop() { active=false; }

		void work();

		static condition_variable cond_var;

	private:
		Window& mwin;
		QueueType& mQueue;
		bool active;
		unsigned int period_min;
		unsigned int period_max;
};

template<typename DocumentType>
condition_variable Producer<DocumentType>::cond_var;

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
		DocumentType newDoc = DocumentType::next();
		mwin << newDoc << ' ';
		if (mQueue.push(newDoc) == 1)
		{
			cond_var.notify_one();
			mwin << "one\n";
		}
		else
		{
			cond_var.notify_all();
			mwin << "all\n";
		}


		unsigned int wait_ms=period_min+(rand()%(period_max-period_min));
		this_thread::sleep_for(std::chrono::milliseconds(wait_ms*speed_producer));
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
		static mutex sync;
};

template<typename DocumentType>
bool Consumer<DocumentType>::nop=true;

template<typename DocumentType>
mutex Consumer<DocumentType>::sync;

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
	while(active)
	{
		unique_lock lock(sync);
		auto status=Producer<DocumentType>::cond_var.wait_until(
			lock,
			chrono::system_clock::now() +std::chrono::milliseconds(10*speed_consumer));

		if (status == cv_status::no_timeout)
		{
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
	}
	mwin << "Consumer end..." << Window::chars::endl;
}

Window& operator<< (Window& win, const Document& doc) 
{
	return win << doc.mi;
}


const int width=15;
const int height=10;
const int status_height = 12;
WindowPlacer placer(width,height, status_height);


template<typename ThreadClass, typename Queue>
class WindowedThread
{
	public:
		WindowedThread(string title, Queue& input, int color)
		: mNormalBorderColor(color)
		{
			mwin = new Window(placer.top(), placer.left(), height, width, placer.title(title));
			updateWinBorder();
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

		void updateWinBorder()
		{
			mwin->setBorderColor(Window::isDisplayEnabled() ? mNormalBorderColor : COLOR_WHITE);
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
		int mNormalBorderColor;
};

void modifySpeed(Window& status, string what, int& speed, int delta)
{
	speed -= delta;
	if (speed<1) speed=1;
	if (speed>100) speed=100;
	status << "Speed of next " << what << " is now " << 101-speed << '\n';
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
		this_thread::sleep_for(std::chrono::milliseconds(poll_key_interval));
		char c(keys.pop());
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
				// TODO strange bug there it happens when playing with d/c/p keys
				// that sometime, windows of producers appear disabled (white border) when consumers
				// windows have a normal border (cyan). Maybe a race condition occurs there
				// See the race_bug.png file (display is enabled)
				{
					Window::toggleDisplay();
					for(auto wt: listConsumers) wt->updateWinBorder();
					for(auto wt: listProducers) wt->updateWinBorder();
				}
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
				status << "  d  toggle display      n  toggle nops display" << Window::chars::endl;
				status << "  s  stats               S  toggle continuous stats" << Window::chars::endl;
				status << "  < > / + -              decrease increase speed of producers/consumers" << Window::chars::endl;
				status << "  q  quit" << Window::chars::endl;
				status << Window::chars::endl;
				break;

			case '<':
				modifySpeed(status,"producers", speed_producer, -1);
				break;

			case '>':
				modifySpeed(status,"producers", speed_producer, 1);
				break;

			case '-':
				modifySpeed(status,"consumers", speed_producer, -1);
				break;

			case '+':
				modifySpeed(status,"consumers", speed_producer, 1);
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
	for(auto wt: listConsumers) wt->stop();
	for(auto wt: listProducers) wt->stop();
	status << "All stopped, deleting...\n";
	for(auto pconsumer : listConsumers)
	{
		delete pconsumer;
	}
	int i=0;
	for(auto pproducer: listProducers)
	{
		status << "Producer deleted " << i++ << Window::chars::endl;
		delete pproducer;
	}
}


int main(int argc, const char* argv[])
{
	if (argc>1)
	{
		for(const char* p=argv[0]; *p; p++)
			keys.push(*p);
	}
	initscr();
	start_color();
	mainLoop();
	keys.stop();
	endwin();
}
