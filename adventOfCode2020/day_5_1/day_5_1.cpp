#include <iostream>
#include <map>
#include <vector>

using namespace std;

int main(int argc, const char* argv[])
{
  int highest=0;
  while(cin)
  {
    string line;
    getline(cin, line);
    if (line.length()!=10) continue;
    int b=64;
    int row=0;

    for(int i=0; i<7; i++)
    {
      if (line[i]=='B') row += b;
      b >>=1;
    }
    b = 4;
    int col=0;
    for(int i=7; i<10; i++)
    {
      if (line[i]=='R') col += b;
      b >>=1;
    }
    highest = std::max(highest, row*8+col);
  }
  cout << highest << endl;
	return 0;
}


