#include <iostream>
#include <set>
#include <string>

using namespace std;

int main(int argc, const char* argv[])
{
  set<string> fields{"byr","iyr","eyr","hgt","hcl","ecl","pid","cid"};
  set<string> passport;
  int valid(0);

  while(cin)
  {
    string line;
    getline(cin, line);
    if (line.length()==0)
    {
      int required = 8;
      if (passport.find("cid")==passport.end()) required--;
      if (passport.size()==required) valid++;

      passport.clear();
    }
    else
    {
      while(line.length())
      {
        if (line[3] != ':') cout << "Warning (" << line << ")" << endl;
        string field(line.substr(0,3));
        if (fields.find(field) == fields.end()) cout << "Warning field (" << field << ")" << endl;
        if (passport.find(field) != passport.end()) cout << "double field (" << field << ")" << endl;
        passport.insert(field);
        while(line.length() && line[0] != ' ') line.erase(0,1);
        while(line.length() && line[0] == ' ') line.erase(0,1);
      }
    }
  }
  cout << valid << endl;
	return 0;
}


