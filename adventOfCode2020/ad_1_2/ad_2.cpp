#include <iostream>
#include <set>

using namespace std;

int main(int argc, const char* argv[])
{
  set<int> input;
  while(cin)
  {
    int num;
    cin >> num;
    for(const auto &i1: input)
      for (const auto &i2 : input)
      if (i1 + i2 + num == 2020)
      {
        cout << i1 * i2 * num << endl;
        return 0;
      }
    input.insert(num);
  }
  cout << "NO ANSWER" << endl;
	return -1;
}


