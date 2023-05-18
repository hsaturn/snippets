#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

std::string getWord(std::string& s, char sep=' ')
{
  if (s.length()==0) return "";
  std::string r;
  auto pos=s.find(sep);
  if (pos == std::string::npos) pos=s.length();
  r=s.substr(0, pos);
  s.erase(0,pos+1);
  return r;
}

std::string removeRedundantDots(std::string path) {
    std::vector<std::string> dirs;
    std::string dir;
    std::string result;
    bool start = (!path.empty() && path[0] == '/');

    while (path.length())
    {
      dir=getWord(path, '/');
        if (dir == ".." && !dirs.empty())
          dirs.pop_back();
        else if (dir.length())
          dirs.push_back(dir);
    }
    for (const std::string& dir : dirs) {
      if (result.length()) result += '/';
        result += dir;
    }

    if (start) result = "/" + result;

    return result;
}

int main(int argc, const char* argv[])
{
  std::string file="/b/../a////b/c/../d/e.txt";
  cout << "1> " << file << endl;
  cout << "2> " << removeRedundantDots(file) << endl;
	return 0;
}


