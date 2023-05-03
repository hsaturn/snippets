// vim: ts=2 sw=2
#include <fstream>
#include <iostream>
#include <string>

using namespace std;
using uint=unsigned int;

template<class type>
class Peak
{
	public:
		Peak(uint window): window(window){}
		
		type add(uint time, type currval)
		{
			uint delta = time - last_time;
		  last_time = time;
			if ((last_time == 0) or (delta >= window))
			{
				cerr << "time: " << time << ", resetting value to " << currval << endl;
				value = currval;
			}
			else
			{
				cerr << "time: " << time << ", value=" << value << ", currval=" << currval;
				type p = (float)delta / (float)window;
				value = value * (1-p) + currval*p; 
			 	cerr << ", p=" << p << ", new_value=" << value << endl;
			}
			return value;
		}

	private:
		int window;
		type value; 
		uint last_time = 0;
};

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

int main(int argc, const char* argv[])
{
	string datain("data.in");

  int n=1;
	int threshold = 5;
	float peak= 0;
	bool autopeak;
	bool steps = true;
	uint window = 20;

	while(n < argc)
	{
		string arg(argv[n]);

		if (arg=="option")
		{
		}
		else	if (arg == "-h")
		{
			cerr << endl;
			cerr << "filter [option] [datafile]" << endl;
			cerr << endl;
			cerr << "  -s           points added for making horizontal segments" << endl;
			cerr << "  -ns          no steps" << endl;
			cerr << "  -t  #        threshold" << endl;
			cerr << "  -p  #        value for peak curve" << endl;
			cerr << "  -w  #        average window" << endl;
			cerr << endl << endl;;
			return -1;
		}
		else if (arg == "-ns")  steps     = false;
		else if (arg == "-s")   steps     = true;
		else if (arg == "-w") 	window    = opt(argc, argv, ++n);
		else if (arg == "-p")	  peak      = opt(argc, argv, ++n);
		else if (arg == "-t")   threshold = opt(argc, argv, ++n);
		else if (arg[0] == '-')
			cerr << error << "Unknown option : [" << arg << "]" << endl;
		else
			datain = arg;
		n++;
	}
	autopeak = peak == 0;
	cerr << "Data      = " << datain << endl;
	cerr << "Peak      = "; if (autopeak) cerr << "auto" << endl; else cerr << peak << endl;
	cerr << "Threshold = " << threshold << endl;
	cerr << "Window    = " << window << endl;

	int oldx = 0;

	Peak<float> Filter(window);
	ifstream in(datain);
	if (not in.good())
		cerr << error << "No data (file=" << datain << ')' << endl;
	while(in.good())
	{
		int x,y;
		int alarm=0;
		in >> x;
		in >> y;
		if (autopeak and y/20 > peak) peak = y / 10;
		auto p = Filter.add(x,y);
		auto dp = std::abs(p-y);
		alarm = 100 * dp / p;

		alarm = alarm > threshold ? peak : 0;

		if (steps) cout << oldx << " " << " " << y << " " << p << " " << alarm << endl;
		cout << x << " " << " " << y << " " << p << " " << alarm << endl;
		oldx = x;
	}
	return 0;
}
