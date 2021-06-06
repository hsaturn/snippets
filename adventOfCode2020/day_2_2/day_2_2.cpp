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
    int pos1,pos2;
    char letter;
    char password[128];

    getline(cin, input);
    if (sscanf(input.c_str(), "%d-%d %c: %s", &pos1, &pos2, &letter, password) == 4)
    {
      if ((password[pos1-1]==letter) ^ (password[pos2-1]==letter)) valid++;
    }
  }
  cout << valid << endl;
	return 0;
}


