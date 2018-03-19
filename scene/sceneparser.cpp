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

#include "sceneloader.h"

#include <iostream>
#include <iomanip>
#include <fstream>

#include <assert.h>
#include <memory>

using namespace std;

#include "object/sprite.h"
#include "object/shape.h"

SceneParser::SceneParser(const std::string &fn) 
	: l(SceneLexer::TOK_ERROR) 
{
	input = new std::ifstream(fn.c_str());
	lexer = new SceneLexer(*input, fn);
}

SceneParser::~SceneParser()
{
	delete lexer;
	delete input;
}

void SceneParser::expect(SceneLexer::Token tok)
{
	if (tok != l.tok)
	{
		cout << "Expected other token" << endl;
		assert(0);
	}
	lex();
}

void SceneParser::lex()
{
	l = lexer->lex();
	if (l.tok == SceneLexer::TOK_ERROR)
	{
		cout << "Syntax error: " << l.str << endl;
		assert(0);
	}
}

int SceneParser::ParseInt()
{
	assert(l.tok == SceneLexer::TOK_INTEGER);
	int v = l.integer;
	lex();
	return v;
}

std::string SceneParser::ParseString()
{
	assert(l.tok == SceneLexer::TOK_STRING);
	std::string v = l.str;
	lex();
	return v;
}

bool SceneParser::ParseBoolean()
{
	switch (l.tok)
	{
		case SceneLexer::TOK_TRUE: return true;
		case SceneLexer::TOK_FALSE:
		case SceneLexer::TOK_NULL: return false;
		default: assert(0);
	}
	assert(0);
	return false;
}

