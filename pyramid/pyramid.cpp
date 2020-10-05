#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <time.h>


const int MAXS=30;

using namespace std;

struct value_info
{
	value_info() : _row(-1) {}
	value_info(int row, int col, int value) : _row(row), _col(col), _value(value){}

	operator bool () const { return _row != -1; }

	int _row;
	int _col;
	int _value;
};

class pyramid
{
	public:
		pyramid(int base) : _base(base)
   	{
			for(int i=0; i<base; i++)
				for(int j=0; j<base-i; j++)
					_data[i][j]=-1;
	  }

		friend ostream& operator << (ostream& out, const pyramid &pyr)
		{
			std::string sep=" ";
			int maxs=1;
			for(int i=0; i<pyr._base; i++)
				for(int j=0; j<pyr._base-i; j++)
				{
					if (pyr._data[i][j] == -1) continue;
					int sz = std::to_string(pyr._data[i][j]).size();
					if (sz>maxs)
					{
						maxs=sz;
					}
				}

			if (pyr.is_complete())
				out << "+:" << endl;
			else
				out << "-:" << endl;
			for(int i=pyr._base-1; i>=0; i--)
			{
				stringstream ss;
				if (pyr._base>9 && i<10) out << ' ';
				out << i << ':';
				for(int j=0; j<pyr._base-i; j++)
				{
					string s;
					if (pyr._data[i][j] == -1)
						s='.';
					else
					 	s = to_string(pyr._data[i][j]);

					while(s.size()<maxs)
					{
						s+=' ';
						if (s.size()<maxs)
							s=' '+s;
					}
					ss << s << sep;
				}
				string s;
				while(s.size() < i*(maxs+sep.size())/2) s+=' ';
				out << "  " << s << ss.str() << endl;
			}
			return out;
		}

		void set(const value_info& value)
		{
			if (value)
			{
				_data[value._row][value._col]=value._value;
			}
		}

		void setbase(int idx, int value)
		{
			_data[0][idx] = value;
		}

		void calc()
		{
			for(int i=0; i<_base; i++)
			{
				for(int j=0; j<_base-i-1; j++)
				{
					_data[i+1][j] = _data[i][j] + _data[i][j+1];
				}
			}
		}

		value_info erase_rand()
		{
			int r=rand()%_base;
			int c=rand()%(_base-r);
			int v=_data[r][c];
			if (v != -1)
			{
				_data[r][c] = -1;
				return value_info(r,c,v);
			}
			else
				return value_info{};
		}

		void erase()
		{
			for(int i=0; i<_base; i++)
				for(int j=0; j<_base-i; j++)
					_data[i][j]=-1;
		}

		bool is_complete() const
		{
			for(int i=0; i<_base; i++)
				for(int j=0; j<_base-i; j++)
					if (_data[i][j]==-1)
						return false;
			return true;
		}

		bool can_be_solved() const
		{
			pyramid solving(*this);
			return solving.solve();
		}

		int max() const
		{
			int m=-1;
			for(int i=0; i<_base; i++)
				for(int j=0; j<_base-i; j++)
					if (_data[i][j] > m)
						m = _data[i][j];
			return m;
		}

		bool solve(int count=9999)
		{
			int added=1;
			while(!is_complete() && added>0 && count>0)
			{
				added=0;
				for(int i=0; i<_base; i++)
				{
					for(int j=0; j<_base-i-1; j++)
					{
						int& a=_data[i][j];
						int& b=_data[i][j+1];
						int& c=_data[i+1][j];
						if (a>=0 && b>=0 && c<0)
						{
							c=a+b;
							added++;
						}
						else if (c>=0 && a>=0 && b<0)
						{
							b=c-a;
							added++;
						}
						else if (c>=0 && b>=0 && a<0)
						{
							a=c-b;
							added++;
						}
						else if (a+b != c)
						{
							return false;
						}
						count -= added;
						if (count<=0) return is_complete();
					}
				}
			}
			return is_complete();
		}

	private:
		int _data[MAXS][MAXS];
		int _base;
		bool err;
};

template<class T>
void check(char c, T value, T mini, T maxi)
{
	if (mini <= value && value <= maxi)
	{
		return;
	}
	cerr << "Value of " << c << " is out of range [" << mini << ',' << maxi << "]" << endl;
	exit(1);
}

int main(int argc, char* argv[])
{
	bool anim = false;
	int seed=0;
	int c;
	extern char* optarg;
	extern int optind, opterr;
	int base=5;
	int maxv=9999999;
	int maxi=9;
	bool zero(false);
	
	while((c = getopt(argc, argv, "azhs:m:b:M:"))!= -1)
	{
		switch(c)
		{
			case 'h':
				cout << endl
					<< "usage: " << argv[0] << " [-a][-z][-b numbers][-s seed][-m max][-M maxv]" << endl
					<< "   -a  Animate autosolve" << endl
					<< "   -b  Number of cells at the bottom" << endl
					<< "   -m max value of the base" << endl
					<< "   -z allow for zero" << endl
					<< "   maxv is the max value of the whole pyramid" << endl
					<< "   seed is 0 for time randomizer" << endl
					<< endl;
				exit(1);
				break;

			case 'a':
				anim=true;
				break;

			case 's':
				seed=atol(optarg);
				break;

			case 'b':
				base=atoi(optarg);
				check(c, base, 2, MAXS);
				break;

		  case 'z':
				zero=true;
				break;

			case 'm':
				maxi=atol(optarg);
				check(c, maxi, 1, 99);
				break;

			case 'M':
				maxv=atol(optarg);
				check(c, maxv, 1, 999999);
				break;
		}
	}
	cout << "base=" << base
		<< ", maxi=" << maxi
		<< ", maxv=" << maxv
		<< endl;

	if (seed==0) seed = time(NULL);
	srand(seed);

	int tries=0;
	int min=99999999;
	pyramid d(base);
	do
	{
		d.erase();
		if (zero)
		{
			for(int i=0; i<base; i++)
				d.setbase(i, rand()%(maxi+1));
		}
		else
		{
			for(int i=0; i<base; i++)
			{
				d.setbase(i, rand()%maxi+1);
			}
		}
		d.solve();
		auto max=d.max();
		if (min > max)
			min=max;
		if (tries++ > 1000)
		{
			cerr << "Unable to find a pyramid (maxv too low ?)" << endl;
			cerr << "Minimum value was " << min << endl;
			exit(1);
		}
	} while (d.max()> maxv);
	cout << d << endl;
	d.solve();
	cout << d << endl;

	pyramid e(d);
	tries=0;
	value_info v;
	while(true)
	{
		while(!(v=e.erase_rand()));
		if (e.can_be_solved())
		{
			tries=0;
		}
		else
		{
			e.set(v);
			if (tries++<1000000)
				tries++;
			else
				break;
		}
	}
	cout << e << endl;
	while(anim && !e.is_complete())
	{
		sleep(1);
		e.solve(1);
		cout << e << endl;
	}
	return 1;
}
