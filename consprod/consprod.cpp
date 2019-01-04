#include <stdlib.h>
#include <iostream>
#include <map>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <chrono>

using namespace std;

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

		friend ostream& operator << (ostream& out, const Document& doc);

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
		Producer(QueueType& queue, unsigned int period_min, unsigned int period_max);

		void stop() { active=false; }

		void work();

	private:
		QueueType& mQueue;
		bool active;
		unsigned int period_min;
		unsigned int period_max;
};

template<typename DocumentType>
Producer<DocumentType>::Producer(QueueType& queue, unsigned int pmin, unsigned int pmax)
:
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
		cout << "wait for " << wait_ms << "ms. size=" << mQueue.size() << endl;
		this_thread::sleep_for(std::chrono::milliseconds(wait_ms));
	}
}

template<typename DocumentType>
class Consumer
{
		using QueueType = LockQueue<DocumentType>;
	public:
		Consumer(QueueType& mQueue);

		void work();
		void stop() { active=false; }


	private:
		QueueType& mQueue;
		bool active;
};

template<typename DocumentType>
Consumer<DocumentType>::Consumer(QueueType& queue)
:
mQueue(queue),
active(true)
{}

template<typename DocumentType>
void Consumer<DocumentType>::work()
{
	unsigned long wait_ms=5;
	while(active)
	{
		cout << "Consumer is waiting..." << wait_ms << endl;
		this_thread::sleep_for(std::chrono::milliseconds(wait_ms));
		Optional<DocumentType> doc = mQueue.pop();
		if (doc)
		{
			cout << "Consumer working on " << doc.get() << endl;
		}
		else
		{
			cout << "Nothing to work on" << endl;
		}
	}
	cout << "Consumer end..." << endl;
}

ostream& operator<< (ostream& out, const Document& doc) 
{
	cout << doc.mi;
}

template<typename T, typename Container>
void dumpQueue(const LockQueue<T,Container>& input, string header="Queue: ")
{
}

int main(int argc, const char* argv[])
{
	unsigned long wait_ms=1000;
	LockQueue<Document>	input;
	cout << input << endl;

	Producer<Document> producer(input, 10, 20);
	Consumer<Document> consumer(input);

	thread thread_prod([&producer](){ producer.work(); });
	thread thread_cons([&consumer](){ consumer.work(); });

	cout << "Threads are running together..." << endl;
	this_thread::sleep_for(std::chrono::milliseconds(wait_ms));

	producer.stop();
	consumer.stop();

	thread_prod.join();
	thread_cons.join();
	
	return 0;
}


