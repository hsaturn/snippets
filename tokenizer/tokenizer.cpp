#include <iostream>
#include <string>

using namespace std;


class Tokenizer
{
	public:

		class const_iterator
		{
			public:
				const_iterator(const Tokenizer* token, string::size_type cur=0) : t(token), cursor(cur)
				{
					trimCursor();
				}

				bool operator!=(const const_iterator& c) const { return t==c.t && cursor!=c.cursor; }

				const_iterator& operator ++() {
					while(cursor < t->getString().length() && !t->isSeparator(cursor)) cursor++;
					trimCursor();
					return *this;
				}

				const_iterator operator++(int) { const_iterator old(*this); return (*this)++; }

				const string operator*()
				{
					return t->getWord(cursor);
				}

			private:
				void trimCursor()
				{
					if (cursor==string::npos) return;
					while(cursor < t->getString().length() && t->isSeparator(cursor)) cursor++;
					if (cursor>=t->getString().length()) cursor=string::npos;
				}
				const Tokenizer* t;
				string::size_type cursor;
		};

		Tokenizer(const string& s, const string separators=" ") : data(s), mseparators(separators), it_begin(this), it_end(this, string::npos) {}

		const const_iterator begin() { return it_begin; }

		const const_iterator end() { return it_end; } 

		const string getWord(string::size_type cursor) const{
			static string empty;
			if (cursor>=data.length()) return empty;
			string::size_type end = data.find_first_of(mseparators, cursor);
			if (end==string::npos)
				end=data.length();
			return data.substr(cursor, end-cursor);
		}

		bool isSeparator(string::size_type pos) const {
			return mseparators.find(data.at(pos)) != string::npos;
		}

		const string& getString() const { return data; }
		string::size_type stringLength() const { return data.length(); }

	private:
		const string data;
		const string mseparators;
		const const_iterator it_begin;
		const const_iterator it_end;
};

int main(int argc, const char* argv[])
{
	cout << "-------------------------------" << endl;
	for(auto word : Tokenizer("alpha beta charlie delta"))
		cout << "GOT : " << word << endl;

	cout << "-------------------------------" << endl;
	for(auto letter : Tokenizer("a+b+c+","+"))
		cout << letter << endl;

	cout << "-------------------------------" << endl;
	for(auto letter : Tokenizer("x y z+t", " |+"))
		cout << letter << endl;
	return 0;
}

