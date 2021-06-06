#include <iostream>
#include <map>
#include <vector>

using namespace std;

int main(int argc, const char* argv[])
{
  bool seats[913];
  for(bool &b: seats) b=false;
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
    seats[row*8+col] = true;
  }
  bool ok=false;
  for(int i=0;i<913;i++)
  {
    if (seats[i]==true) ok=true;
    else if (ok and (seats[i]==false))
    {
      cout << i << endl;
      return 0;
    }
  }
	return 0;
}


