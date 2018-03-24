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

#ifndef SCENEPARSER_H
#define SCENEPARSER_H

#include "scenelexer.h"
#include "cl2/cl2.h"

class SceneParser {
public:
    explicit SceneParser(const std::string &fn);
    virtual ~SceneParser();

protected:
    void lex();
    void expect(SceneLexer::Token tok);

    std::istream *input;
    SceneLexer *lexer;
    SceneLexer::Lexeme l;

    int ParseInt();
    std::string ParseString();
    bool ParseBoolean(); // true or false
};

#endif

