#include <iostream>
#include <iomanip>

using namespace std;

int main(int argc, const char* argv[])
{
  if (argc<2) return 1;
  int last=atol(argv[1]);
  string doors="1";
  string step="1";
  while(doors.length()<last)
  {
    step="00"+step;
    doors+=step;
  }
  if (doors.length()>last) doors.erase(last);
  cout << setw(4) <<  "" << " : " << doors << endl;
	return 0;
}


