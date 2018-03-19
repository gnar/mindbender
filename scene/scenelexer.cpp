/*
    MindBender - The MindBender adventure engine
    Copyright (C) 2006  Gunnar Selke

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "scenelexer.h"
#include <assert.h>

#include <iostream>
using namespace std;

SceneLexer::Lexeme SceneLexer::error(const char *reason)
{
	SceneLexer::Lexeme lexeme(TOK_ERROR);
	lexeme.str = reason;
	err = true;
	return lexeme;
}

SceneLexer::SceneLexer(std::istream &inp, const std::string &filename) : input(inp), filename(filename)
{
	lineno = 1;
	eof = false; err = false;
	ch = -1;

	next();
}

SceneLexer::Lexeme SceneLexer::lex()
{
	char tmp;

	if (err) return error("<error>");

	while (!eof)
	{
		switch (ch)
		{
			case '\n': // eat whitespace and line feed
				++lineno;
			case '\t':
			case '\r':
			case ' ': next(); continue;

			case '/': // divide or comment
				next();
				if (ch == '/' || ch == '*')
				{
					if (!eatComment()) return(error("Comment error")); 
					continue;
				}
				return(error("Unexpected character"));

			case '=':
			case ':':
			case '{': case '}':
				tmp = ch; 
				next(); 
				return(tmp);

			case '"': // begin string
				return(readString());

			default:
				if (isdigit(ch) || ch == '-')
				{
					return(readNumber());
				} else if (isalpha(ch) || ch == '_') {
					return(readKeywordOrIdentifier());
				}
				// Error!
				return(error("Unexpected character"));
				break;
		}
	}

	return(TOK_EOF);
}

void SceneLexer::next()
{
	input.read(&ch, sizeof(char));
	if (input.eof()) eof = true; 
}

SceneLexer::Lexeme SceneLexer::readNumber()
{
	bool isfloat = false;
	int negate_factor = 1;
	int bufpos = 0;

	if (ch == '-')
	{
		negate_factor = -1;
		next();
	}
	
	while ((isdigit(ch) || ch == '.') && !eof)
	{
		if (ch == '.') isfloat = true;	
		tmpstr[bufpos++] = ch;
		next();
	}
	tmpstr[bufpos++] = '\0';
	
	SceneLexer::Lexeme lexeme(isfloat ? TOK_FLOAT : TOK_INTEGER);
	if (isfloat)
		lexeme.real = negate_factor * std::strtod(tmpstr, NULL);
	else
		lexeme.integer = negate_factor * std::atoi(tmpstr);
	return lexeme;
}

SceneLexer::Lexeme SceneLexer::readString()
{
	int bufpos = 0;
	next(); // skip "

	while (ch != '"' && !eof)
	{
		switch (ch)
		{
			case '\n': return error("Newline inside string");
			case '\\': // escape character
				next(); if (eof) return error("End of file inside string");
				switch (ch)
				{
					case '\n': tmpstr[bufpos++] = '\n'; break; // escaped line break
					case '\\': tmpstr[bufpos++] = '\\'; break;
					case '"': tmpstr[bufpos++] = '"'; break;
					case 't': tmpstr[bufpos++] = '\t'; break;
					case 'a': tmpstr[bufpos++] = '\a'; break;
					case 'b': tmpstr[bufpos++] = '\b'; break;
					case 'n': tmpstr[bufpos++] = '\n'; break;
					case 'r': tmpstr[bufpos++] = '\r'; break;
					case 'v': tmpstr[bufpos++] = '\v'; break;
					case 'f': tmpstr[bufpos++] = '\f'; break;
					default: return error("Unrecognized escape character"); 
				}
				break;
			default: tmpstr[bufpos++] = ch; break;
		}
		next();
	}
	tmpstr[bufpos++] = '\0';

	if (eof) return error("End of file inside string");
	next(); // skip ending "

	SceneLexer::Lexeme lexeme(TOK_STRING);
	lexeme.str = tmpstr;
	return lexeme;
}

static struct CLKeywordDef
{
	char *name;
	SceneLexer::Token tok;
} keywords[] = {
	{"item", SceneLexer::TOK_ITEM},
	{"bagitem", SceneLexer::TOK_BAGITEM},
	{"actor", SceneLexer::TOK_ACTOR},
	{"room", SceneLexer::TOK_ROOM},
	{"game", SceneLexer::TOK_GAME},
	{"prolog", SceneLexer::TOK_PROLOG},
	{"run", SceneLexer::TOK_RUN},
	{"script", SceneLexer::TOK_SCRIPT},
	{"function", SceneLexer::TOK_FUNCTION},
	{"scene", SceneLexer::TOK_SCENE},
	{"true", SceneLexer::TOK_TRUE},
	{"false", SceneLexer::TOK_FALSE},
	{"null", SceneLexer::TOK_NULL},
	{"translate", SceneLexer::TOK_TRANSLATE},
	{"scale", SceneLexer::TOK_SCALE},
	{"foreground", SceneLexer::TOK_FOREGROUND},
	{"background", SceneLexer::TOK_BACKGROUND},
	{"mainplane", SceneLexer::TOK_MAINPLANE},
};
static int num_keywords = sizeof(keywords) / sizeof(CLKeywordDef);

SceneLexer::Lexeme SceneLexer::readKeywordOrIdentifier()
{
	int bufpos = 0;

	while ((isalnum(ch) || ch == '_') && !eof)
	{
		tmpstr[bufpos++] = ch;
		next();
	}
	tmpstr[bufpos++] = '\0';

	// check if buffer is a keyword..
	for (int i=0; i<num_keywords; ++i)
	{
		if (std::strcmp(keywords[i].name, tmpstr) == 0) return SceneLexer::Lexeme(keywords[i].tok);
	}

	// buffer is a identifier
	SceneLexer::Lexeme lexeme(TOK_IDENTIFIER);
	lexeme.str = tmpstr;
	return lexeme;
}

bool SceneLexer::eatComment()
{
	if (ch == '/') // C++ style comment
	{
		char pr = -1;
		do
		{
			pr = ch;
			next();

			// parse nested comments
			if (pr == '/' && ch == '*')
			{
				if (!eatComment()) return false;
			}

		} while (ch != '\n' && !eof);
		++lineno;
		next();
		return true;
	} else if (ch == '*') { // C style comment
		char pr = -1;
		do
		{
			pr = ch;
			next();
			if (ch == '\n') ++lineno;

			// parse nested comments
			if (pr == '/' && (ch == '/' || ch == '*'))
			{
				if (!eatComment()) return false;
			}

		} while (!eof && !(pr == '*' && ch == '/'));
		next();

		return true;
	} else {
		return false;
	}

}




