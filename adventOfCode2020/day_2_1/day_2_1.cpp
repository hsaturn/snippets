#include <iostream>
#include <string>
#include <sstream>

using namespace std;

// Count valid passwords
// 1-3 a: abcde     : 1-3 a => password must contain 1-3 'a'
int main(int argc, const char* argv[])
{
  int valid(0);
  while(cin)
  {
    string input;
    int min,max;
    char letter;
    char password[128];

    getline(cin, input);
    if (sscanf(input.c_str(), "%d-%d %c: %s", &min, &max, &letter, password) == 4)
    {
      const char* p(password);
      int count(0);
      while(*p) if (*p++ == letter) count++;
      if (count>=min && count<=max) valid++;
    }
  }
  cout << valid << endl;
	return 0;
}


