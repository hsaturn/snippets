#pragma once

#include <iostream>

namespace ansi
{
	const char* clr="\033[2J";
	const char* clr_after="\033[J";
	const char* clr_scrollback="\033[3J";

	const char* savepos="\033[s";
	const char* restorepos="\033[u";

	const char* hide_cur="\033[?25l";
	const char* show_cur="\033[?25h";

	const char* black="\033[30m";
	const char* red="\033[31m";
	const char* green="\033[32m";
	const char* yellow="\033[33m";
	const char* blue="\033[34m";
	const char* magenta="\033[35m";
	const char* cyan="\033[36m";
	const char* white="\033[37m";
	
	const char* bg_black="\033[40m";
	const char* bg_red="\033[41m";
	const char* bg_green="\033[42m";
	const char* bg_yellow="\033[43m";
	const char* bg_blue="\033[44m";
	const char* bg_magenta="\033[45m";
	const char* bg_cyan="\033[46m";
	const char* bg_white="\033[47m";

	const char* reset="\033c";

	class gotoxy
	{
		public:
			gotoxy(int row, int col) : row(row), col(col){}
			std::ostream& operator()(std::ostream& out) const
			{
				return out << "\033[" << row << ';' << col << 'H';
			}
		private:
			int row,col;
	};

	std::ostream& operator<<(std::ostream& out, const gotoxy& go)
	{
		return go(out);
	}
};
