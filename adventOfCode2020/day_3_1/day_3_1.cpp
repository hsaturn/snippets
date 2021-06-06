#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, const char* argv[])
{
  vector<string> patterns;
  while(cin)
  {
    string pattern;
    getline(cin, pattern);
    if (pattern.length()) patterns.push_back(pattern);
  }
  int x=0, y=0, trees=0;
  const int dx=3, dy=1;
  while(y<patterns.size())
  {
    if (patterns[y][x % patterns[y].length()] == '#') trees++;
    x+=dx; y+=dy;
  }
  cout << trees << endl;
	return 0;
}


