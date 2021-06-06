#include <iostream>
#include <set>
#include <map>
#include <string>
#include <string.h>
#include <functional>

using namespace std;

map<string, function<bool(const char*)>> validators{
  { "byr", [](const char* byr) { return atol(byr)>=1920 and atol(byr)<=2002; }},
  { "iyr", [](const char* iyr) { return atol(iyr)>=2010 and atol(iyr)<=2020; }},
  { "eyr", [](const char* eyr) { return atol(eyr)>=2020 and atol(eyr)<=2030; }},
  { "hgt", [](const char* hgt) {
      int h; char unit[4]; // overflow risk
      if (sscanf(hgt, "%d%s", &h, unit) != 2) return false;
      if (strcmp(unit, "cm")==0) return h>=150 and h<=193;
      if (strcmp(unit, "in")==0) return h>=59 and h<=76;
      return false; }},
  { "hcl", [](const char* hcl) {
      if (strlen(hcl) != 7) return false;
      for(int i=0; i<6; i++)
      {
        hcl++;
        if (*hcl<'0' or *hcl>'f' or (*hcl>'9' and *hcl<'a')) return false;
      }
      return true; }},
  { "ecl", [](const char* ecl) {
    static set<string> valid{"amb","blu","brn","gry","grn","hzl","oth"};
    return valid.find(ecl) != valid.end(); }},
  { "pid", [](const char* pid) {
    string s(pid);
    return s.length()==9 and s.find_first_not_of("0123456789")==string::npos; }}
};

static string getword(string &s, char sep=' ')
{
    string word;
    while(s.length() && s[0]!=sep)
    {
        word += s[0];
        s.erase(0,1);
    }
    while(s.length() && s[0]==sep) s.erase(0,1);
    return word;
}

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
        string value = getword(line);
        string name = getword(value,':');
        if (fields.find(name) == fields.end()) cout << "Warning field (" << name << ")" << endl;
        if (passport.find(name) != passport.end()) cout << "double field (" << name << ")" << endl;
        bool insert=true;
        if (validators.find(name) != validators.end())
        {
          insert = validators[name](value.c_str());
        }
        if (insert) passport.insert(name);
      }
    }
  }
  cout << valid << endl;
	return 0;
}


