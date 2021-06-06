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
    for(const auto &old: input)
      if (old + num == 2020)
      {
        cout << old * num << endl;
        return 0;
      }
    input.insert(num);
  }
  cout << "NO ANSWER" << endl;
	return -1;
}


