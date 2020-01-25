#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

using namespace std;

void foo(int n)
{ 
    int a[n];

	for (int i:a)
	{
		cout << " random " << i << endl;
	}
}

void indirect(auto fonc, auto i)
{
	fonc(i);
}

int main(int argc, const char* argv[]) 
{
	foo(3);

	cout << endl;

	// Declaration, définition et appel
	[] { cout << "Hello, my Greek friends"; }();

	[ x = 42] { cout << " what ? " << x << endl; }();

	int j=100;

	auto bar = [&j] (int i=0)
	{
		cout << "BAR IS BACK " << i << " / " << j << endl;
	};

	bar();

	indirect(bar,13);

	j++;

	indirect(bar,14);

	cout << endl;

	// Utilisation de <algorithm>
	vector<int> v;

	v.push_back(1);
	v.push_back(2);
	v.push_back(3);

	auto outVal = [](int val)
	{
		cout << "Value=" << val << endl;
	};

	for_each( v.begin(), v.end(),
			[](int val) 
			{
				cout << "Valeur val = " << val << endl;
			} );

	cout << endl;
	for_each( v.begin(), v.end(), outVal);
	// Type de retour d'une lambda
	auto retFun = [](int i) ->int { return i+i; };

	cout << retFun(5) << endl;
	// generic lambda
	auto incr = [](auto n) { return n++; };

	incr(j);
	// template with lambda
	auto add = [] <typename T> (T a, T b) 
	{
		return a + b;
	};

	// digit separator
	long l=1'000'000;
	int n = 0b0001'0000'0000'0000'0000'0000;
	cout << l << endl;
	cout << n << endl;
	cout << hex << "0x" << n << resetiosflags(ios_base::basefield) << endl;

	return 0;
}


