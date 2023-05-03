// vim: ts=2 sw=2
#include <fstream>
#include <iostream>
#include <map>
#include <random>

using namespace std;

const char* error = "*** ERROR: ";

const char* getarg(unsigned int argc, const char* argv[], int n)
{
	if (n < argc)
		return argv[n];

	cerr << error << "Missing argument near " << argv[n-1] << endl;
	return "";
}

float opt(unsigned int argc, const char* argv[], int n)
{
	return atof(getarg(argc, argv, n));
}

using uint = unsigned int;

int main(int argc, const char* argv[])
{
	uint n=1;
	uint start=0;
	uint end=1000;
	uint step=10;
	float price=50;
	float max_price=60;
	float min_price=40;
	map<uint, const char*> ticks;
	map<uint, uint> jumps;

	while(n < argc)
	{
		string arg(argv[n]);
		if (arg == "-h")
		{
			cerr << endl;
			cerr << "sample options : generate random price data" << endl;
			cerr << endl;
			cerr << "  -p #            initial price" << endl;
			cerr << "  -pr # #         price range" << endl;
			cerr << "  -tr # #         time range" << endl;
			cerr << "  -s #            step" << endl;
			cerr << "  -j time ticks   jump ticks at time, can appear more than once" << endl;  
			cerr << "  -t time ticks   ticks random evol at time, can appear more than once" << endl;  
			cerr << "     if ticks has a sign, the the evolution of price follows the sign" << endl;
			cerr << endl << end;
			return -1;
		}
		else if (arg == "-pr")
		{
			min_price = opt(argc, argv, ++n);
			max_price = opt(argc, argv, ++n);
		}
		else if (arg == "-tr")
		{
			start = opt(argc, argv, ++n);
			end = opt(argc, argv, ++n);
		}
		else if (arg == "-p")		 price  = opt(argc, argv, ++n);
		else if (arg == "-s")    step   = opt(argc, argv, ++n);
		else if (arg == "-j")
		{
			jumps[opt(argc, argv, n+1)] = opt(argc, argv, n+2);
			n+=2;
		}
		else if (arg == "-t")
		{
			ticks[opt(argc, argv, n+1)] = getarg(argc, argv, n+2);
			n+=2;
		}
		else if (arg[0] == '-')
			cerr << error << "Unknown option " << arg << endl;

		n++;
	}
	cerr << "time range   [" << start << ", " << end << "]" << endl;
	cerr << "start price  " << price << endl;
	cerr << "price range  [" << min_price << ", " << max_price << "]" << endl;

	uint time=start;
	std::random_device rd;
	std::mt19937 mt(rd());
	using distribution = std::uniform_int_distribution<int>;
	distribution dist;
	dist = distribution(-1, 1);

	bool default_vol = true;
	while (time < end)
	{
		time += step;
		cout << time << " " << price << endl;
		if (ticks.size())
		{
			if (time >= ticks.begin()->first)
			{
				const char* evol = ticks.begin()->second;
				float t1, t2;
				float t = std::abs(atof(evol));
				if (evol[0] == '-')
				{
					t1 = -t;
					t2 = 0;
				}
				else if (evol[0] == '+')
				{
					t1 = 0;
					t2 = t;
				}
				else
				{
					t1 = -t;
					t2 = t;
				}

				dist = distribution(t1, t2);
				default_vol = false;
				ticks.erase(ticks.begin());
			}
		}
		if (jumps.size())
		{
			if (time >= jumps.begin()->first)
			{
				price += jumps.begin()->second;
				jumps.erase(jumps.begin());
			}
		}
		price += dist(mt);
		if (price > max_price)
			price = max_price;
		else if (price < min_price)
			price = min_price;
	}
	return 0;
}
