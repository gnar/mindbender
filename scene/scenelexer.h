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

#ifndef SCENE_LEXER_H
#define SCENE_LEXER_H

#include <iostream>
#include <string>


class SceneLexer
{
public:
	enum Token
	{
		TOK_ERROR=-1,

		TOK_EOF=256,
		TOK_INTEGER,
		TOK_FLOAT,
		TOK_STRING,
		TOK_IDENTIFIER,

		TOK_ITEM,		// item
		TOK_BAGITEM,		// bagitem
		TOK_ACTOR,		// actor
		TOK_ROOM,		// room
		TOK_GAME,		// game

		TOK_PROLOG,		// prolog
		TOK_RUN,		// run
		TOK_SCRIPT,		// script
		TOK_FUNCTION,		// function
		TOK_SCENE,		// scene
		
		TOK_TRANSLATE,		// translate
		TOK_SCALE,		// scale

		TOK_FOREGROUND,         // foreground
		TOK_MAINPLANE,          // mainplane
		TOK_BACKGROUND,         // background

		TOK_TRUE,
		TOK_FALSE,
		TOK_NULL,
	};

	struct Lexeme
	{
		Lexeme(Token t) : tok(t) {}
		
		Lexeme(char t)
		{
			tok = (Token)t;
		}

		Token tok;
		std::string str;
		int integer;
		float real;
	};

	SceneLexer(std::istream &input, const std::string &filename = "<input>");

	SceneLexer::Lexeme lex();  // return next token 

	int getLine() { return lineno; }
	const std::string getFile() { return filename; }

private:
	std::istream &input;
	std::string filename;
	int lineno; // line position in file, line position of last returned token
	char tmpstr[2048];

	char ch; // current char
	bool eof;
	bool err;
	void next();
	
	bool eatComment();
	SceneLexer::Lexeme readNumber();
	SceneLexer::Lexeme readString();
	SceneLexer::Lexeme readKeywordOrIdentifier();
	SceneLexer::Lexeme error(const char *reason);
};

#endif

