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

#include "gameloader.h"
#include "sceneloader.h"

#include <iostream>
#include <iomanip>
#include <fstream>

#include <assert.h>
#include <memory>

using namespace std;

GameLoader::GameLoader(CLContext *context, const std::string &fn) : SceneParser(fn), context(context)
{
}

GameLoader::~GameLoader()
{
	std::list<SceneLoader*>::iterator it, end = scene_loaders.end();
	for (it=scene_loaders.begin(); it!=end; ++it)	
	{
		delete *it;
	}
}

// Parse a scene file
void GameLoader::Parse()
{
	lex(); // read first token

	bool done = false;
	while (!done)
	{
		switch (l.tok)
		{
			case SceneLexer::TOK_PROLOG:
			{
				lex();
				expect(SceneLexer::TOK_SCRIPT);
				expect(SceneLexer::Token(':'));
				std::string fn = ParseString();
				prolog_scripts.push_back(fn);
				//cout << "Prolog: " << fn << endl;
				break;
			}

			case SceneLexer::TOK_RUN:
			{
				lex();
				expect(SceneLexer::TOK_SCRIPT);
				expect(SceneLexer::Token(':'));
				std::string fn = ParseString();
				run_scripts.push_back(fn);
				//cout << "Run: " << fn << endl;
				break;
			}

			case SceneLexer::TOK_SCENE:
			{
				lex();
				expect(SceneLexer::Token(':'));
				std::string fn = ParseString();
				SceneLoader *sl = new SceneLoader(context, fn);
				scene_loaders.push_back(sl);
				//cout << "Scene: " << fn << endl;
				break;
			}

			case SceneLexer::TOK_EOF:
				done = true;
				break;

			default:
				assert(0);
		}
	}
}


void GameLoader::RunPrologScripts()
{
	std::list<std::string>::iterator it, end = prolog_scripts.end();
	for (it=prolog_scripts.begin(); it != end; ++it)
	{
		const std::string &fn = *it;
		cout << "Running prolog script " << fn << endl;

		CLValue thread(new CLThread(context));
		GET_THREAD(thread)->init(CLCompiler::compile(context, fn));
		GET_THREAD(thread)->enableYield(false);
		GET_THREAD(thread)->run();
	}
}

void GameLoader::ParseScenes()
{
	std::list<SceneLoader*>::iterator it, end = scene_loaders.end();
	for (it=scene_loaders.begin(); it!=end; ++it)	
	{
		(*it)->Parse();
	}
}

void GameLoader::RunSceneScripts()
{
	std::list<SceneLoader*>::iterator it, end = scene_loaders.end();
	for (it=scene_loaders.begin(); it!=end; ++it)	
	{
		(*it)->RunAttachedScripts();
	}
}

void GameLoader::RunStartScripts()
{
	std::list<std::string>::iterator it, end = run_scripts.end();
	for (it=run_scripts.begin(); it != end; ++it)
	{
		const std::string &fn = *it;
		cout << "Running starting script " << fn << endl;

		CLValue thread(new CLThread(context));
		GET_THREAD(thread)->init(CLCompiler::compile(context, fn));
		GET_THREAD(thread)->enableYield(false);
		GET_THREAD(thread)->run();
	}
}

