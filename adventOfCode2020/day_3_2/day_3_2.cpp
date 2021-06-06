#include <iostream>
#include <string>
#include <vector>

struct slope
{
  int dx;
  int dy;
};

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
  vector<slope> slopes{{1,1},{3,1},{5,1},{7,1},{1,2}};
  long result=1;
  for(const auto &slope: slopes)
  {
    int x=0, y=0;
    long trees=0;
    while(y<patterns.size())
    {
      if (patterns[y][x % patterns[y].length()] == '#') trees++;
      x+=slope.dx; y+=slope.dy;
    }
    result *= trees;
  }
  cout << result << endl;
	return 0;
}


