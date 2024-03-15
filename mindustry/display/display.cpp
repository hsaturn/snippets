#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

const string shift="    ";
const int offset = 32;


int first_char = 32;
int last_char = 95;

string make_label(int c)
{
  return string("drawChar_")+to_string(c+first_char);
}


vector<string> makeRoutine(uint32_t v, int c)
{
  vector<string> routine;

  routine.push_back(make_label(c)+':');
  if (v & 0x1)    routine.push_back("draw line _xss _y3 _x3ss _y3");
  if (v & 0x2)    routine.push_back("draw line _x3ss _y3 _x3s _y2");
  if (v & 0x4)    routine.push_back("draw line _x3s _y2 _x3 cur_y");
  if (v & 0x8)    routine.push_back("draw line cur_x cur_y _x3 cur_y");
  if (v & 0x10)   routine.push_back("draw line _xs _y2 cur_x cur_y");
  if (v & 0x20)   routine.push_back("draw line _xss _y3 _xs _y2");
  if (v & 0x40)   routine.push_back("draw line _xs _y2 _x2s _y2");
  if (v & 0x80)   routine.push_back("draw line _x2s _y2 _x3s _y2");
  if (v & 0x100)  routine.push_back("draw line _xss _y3 _x2s _y2");
  if (v & 0x200)  routine.push_back("draw line _x2ss _y3 _x2s _y2");
  if (v & 0x400)  routine.push_back("draw line _x3ss _y3 _x2s _y2");
  if (v & 0x800)  routine.push_back("draw line cur_x cur_y _x2s _y2");
  if (v & 0x1000) routine.push_back("draw line _x2 cur_y _x2s _y2");
  if (v & 0x2000) routine.push_back("draw line _x2s _y2 _x3 cur_y");
  if (v & 0x4000) {
    routine.push_back("op add _draw_char_x3 _x3 font_skew");
    routine.push_back("op sub _draw_char_x4 _draw_char_x3 1");
    routine.push_back("draw line _draw_char_x3 cur_y _draw_char_x4 cur_y");
  }

  routine.push_back("jump draw_char_advance always");

  return routine;
}

int main(int argc, const char* argv[])
{
  int arg=1;
  int group = 7;
  bool build_asm = false;
  string file("charset.14");
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
    else if (opt == "-asm")
      build_asm = true;
    else if (opt == "-i")
    {
      int ind = atoi(argv[arg++]);
      indent=string(ind, ' ');
    }
    else if (opt == "-h")
    {
      cerr << "usage: segments [file][-asm][-g group][-l label]" << endl << endl;
      exit(1);
    }
    else
      file = opt;
  }
  cerr << "file:  " << file << endl;
  cerr << "group: " << group << endl;
  vector<uint32_t> charset;
  ifstream f(file);
  int count=0;
  while(f.good())
  {
    string row;
    getline(f, row);
    cerr << "row " << count << ' ' << row << '=';
    uint32_t hexa = strtoul(row.c_str(), nullptr, 16);
    cerr << hexa << endl;
    charset.push_back(hexa);
  }

  int offset = 0;
  map<int, vector<string>> routines;
  vector<std::string> code;

  last_char = first_char + charset.size() -1;

  code.push_back("set cur_x 10");
  code.push_back("set cur_y 10");
  code.push_back("set font_width 8");
  code.push_back("set font_height 16");
  code.push_back("set font_stroke 1");
  code.push_back("set font_vspace 2");
  code.push_back("set font_hspace 4");
  code.push_back("set font_skew 0");
  code.push_back("op add _drawChar @counter 1");
  code.push_back("jump draw_char_end always");

  code.push_back("op div _sk2 font_skew 2");
  code.push_back("jump draw_char_advance lessThan char " + to_string(first_char));
  code.push_back("jump draw_char_advance greaterThan char " + to_string(last_char));
  code.push_back("op add _xs cur_x _sk2");
  code.push_back("op add _xss cur_x font_skew");
  code.push_back("op div _x2 font_width 2");
  code.push_back("op add _x2 _x2 cur_x");
  code.push_back("op add _x2s _x2 _sk2");
  code.push_back("op add _x2ss _x2 font_skew");
  code.push_back("op add _x3 cur_x font_width");
  code.push_back("op add _x3s _x3 _sk2");
  code.push_back("op add _x3ss _x3 font_skew");
  code.push_back("op div _y2 font_height 2");
  code.push_back("op add _y2 _y2 cur_y");
  code.push_back("op add _y3 cur_y font_height");
  code.push_back("draw stroke font_stroke");
  code.push_back("op sub _char char "+to_string(first_char-1));
  code.push_back("op add _char _char @counter");
  code.push_back("set @counter _char");

  for(int c=0; c < charset.size(); c++)
  {
    code.push_back(string("jump ")+make_label(c)+" always");
  }

  for(int c=0; c < charset.size(); c++)
    routines[c] = makeRoutine(charset[c], c);

  for(const auto& [c, routine]: routines)
    for(const auto& line: routine)
    {
      code.push_back(line);
    };

  code.push_back("draw_char_advance:");
  code.push_back("op add cur_x cur_x font_hspace");
  code.push_back("op add cur_x cur_x font_width");
  code.push_back("set @counter _draw_char_return");
  code.push_back("draw_char_end:");

  int row=0;
  for(const auto& line: code)
    if (build_asm)
      cout << indent << "asm `" << line << "`;" << endl;
    else
      cout << line << endl;

  cout << endl;
  cout << endl;

	return 0;
}


