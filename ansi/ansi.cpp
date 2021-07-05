#include <iostream>
#include <map>
#include <vector>
#include "ansi.h"
#include <unistd.h>

using namespace std;

int main(int argc, const char* argv[])
{
	cout << ansi::clr;
	cout << "Je " << ansi::savepos << endl;
	cout << "Interrupted" << endl;
	cout << ansi::restorepos << "pense !" << endl;
	cout << ansi::gotoxy(1,1) << "ONE ONE";
	cout << ansi::gotoxy(10,10) << "TEN TEN" << endl;
	const long second=1000000;
	for(int i=0; i<2; i++)
	{
		usleep(second);
		cout << ansi::hide_cur << '.' << flush;
		usleep(second);
		cout << ansi::show_cur << '.' << flush;
	}
	return 0;
}


