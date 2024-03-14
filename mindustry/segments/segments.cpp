#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

const string shift="    ";
const int offset = 32;

void dicho(vector<string>& v, int start, int last, int thr, string indent, string label)
{
  int size = last - start + 1;
  if (size == 0) return;
  if (size < thr)
  {
    string else_;
    for(int i=start; i <= last; i++)
    if (v[i].length()) {
      cout << indent << else_ << "if (c == " << i+offset << ") v=" << v[i];
      if (label.length()) cout << " continue " << label << ";";
      cout << endl;
      else_ = "else ";
    }
  }
  else
  {
    int half = start + size / 2;
    cout << indent << "if (c < " << half + offset << ") {" << endl;
    dicho(v, start, half-1, thr, indent + shift, label);
    cout << indent << "} else {" << endl;
    dicho(v, half, last, thr, indent+shift, label);
    cout << indent << "}" << endl;
  }
}

int main(int argc, const char* argv[])
{
  int arg=1;
  int group = 7;
  string file("charset");
  string label(file);
  string indent("    ");
  cerr << "argc " << argc << endl;
  while(arg<argc)
  {
    string opt = argv[arg++];
    if (opt == "-g")
      group = atoi(argv[arg++]);
    else if (opt == "-l")
      label = argv[arg++];
    else if (opt == "-i")
    {
      int ind = atoi(argv[arg++]);
      indent=string(ind, ' ');
    }
    else if (opt == "-h")
    {
      cerr << "usage: segments [file][-g group][-l label]" << endl << endl;
      exit(1);
    }
    else
      file = opt;
  }
  cerr << "file:  " << file << endl;
  cerr << "group: " << group << endl;
  vector<string> charset;
  ifstream f("charset");
  int count=0;
  while(f.good())
  {
    string row;
    getline(f, row);
    charset.push_back(row);
  }
  dicho(charset, 0, charset.size(), group,  indent, label);
  if (label.length())
    cout << indent << label << ':' << endl;
	return 1;
}


