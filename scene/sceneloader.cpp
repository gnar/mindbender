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

#include <iomanip>

#include <memory>

using namespace std;

#include "object/item.h"
#include "object/bagitem.h"
#include "object/sprite.h"
#include "object/shape.h"

SceneLoader::SceneLoader(CLContext *context, const std::string &fn) : SceneParser(fn), context(context) {
}

SceneLoader::~SceneLoader() {
}

// Parse a scene file
void SceneLoader::Parse() {
    lex(); // read first token

    bool done = false;
    while (!done) {
        switch (l.tok) {
            case SceneLexer::TOK_ROOM: {
                lex();
                assert(l.tok == SceneLexer::TOK_IDENTIFIER);
                std::string id = l.str;
                lex();

                cout << "Adding room: " << id << endl;
                expect(SceneLexer::Token('{'));
                CLValue room = ParseRoom();
                expect(SceneLexer::Token('}'));

                CLValue root = context->getRootTable();
                CLValue key = CLValue(new CLString(context, id.c_str()));
                root.set(key, room);

                break;
            }

            case SceneLexer::TOK_BAGITEM: {
                lex();
                assert(l.tok == SceneLexer::TOK_IDENTIFIER);
                std::string id = l.str;
                lex();

                //cout << "Adding bagitem: " << id << endl;
                expect(SceneLexer::Token('{'));
                CLValue bagitem = ParseBagItem();
                expect(SceneLexer::Token('}'));

                CLValue root = context->getRootTable();
                CLValue key(new CLString(context, "Inv"));
                CLValue inv = root.get(key);
                if (inv.isNull()) // create Inv namespace if necessary
                {
                    inv = CLValue(new CLTable(context));
                    root.set(key, inv);
                }
                key = CLValue(new CLString(context, id));
                inv.set(key, bagitem);

                break;
            }

            case SceneLexer::TOK_SCRIPT: {
                lex();
                expect(SceneLexer::Token(':'));
                std::string fn = ParseString();
                //cout << "Attaching script: " << fn << endl;
                scripts.push_back(fn);
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

void SceneLoader::RunAttachedScripts() {
    std::list<std::string>::iterator it, end = scripts.end();
    for (it = scripts.begin(); it != end; ++it) {
        const std::string &fn = *it; // filename

        CLValue thread(new CLThread(context));
        GET_THREAD(thread)->init(CLCompiler::compile(context, fn));
        GET_THREAD(thread)->enableYield(false);
        GET_THREAD(thread)->run();
    }
}

CLValue SceneLoader::ParseRoom() {
    Room *room = new Room(context, 640, 360);

    bool done = false;
    while (!done) {
        switch (l.tok) {
            case SceneLexer::TOK_IDENTIFIER: {
                std::string key = l.str;
                lex();
                if (l.tok == ':') {
                    lex();
                    ParseRoomProperty(key, room);
                } else if (l.tok == '=') {
                    lex();
                    ParseSetSlot(key, room);
                } else {
                    assert(0);
                }

                break;
            }

            case SceneLexer::TOK_MAINPLANE:
                lex();
                expect(SceneLexer::Token('{'));
                ParsePlane(room, Room::MAIN_PLANE);
                expect(SceneLexer::Token('}'));
                break;

            case SceneLexer::TOK_FOREGROUND:
                lex();
                expect(SceneLexer::Token('{'));
                ParsePlane(room, Room::FOREGROUND_PLANE);
                expect(SceneLexer::Token('}'));
                break;

            case SceneLexer::TOK_BACKGROUND:
                lex();
                expect(SceneLexer::Token('{'));
                ParsePlane(room, Room::BACKGROUND_PLANE);
                expect(SceneLexer::Token('}'));
                break;

            case SceneLexer::Token('}'):
                done = true;
                break;

            default:
                assert(0);
        }
    }

    return CLValue(room);
}

void SceneLoader::ParsePlane(Room *room, Room::PlaneID pid) {
    bool done = false;
    while (!done) {
        switch (l.tok) {
            case SceneLexer::TOK_IDENTIFIER: {
                std::string key = l.str;
                lex();
                expect(SceneLexer::Token(':'));
                ParsePlaneProperty(key, room, pid);
                break;
            }

            case SceneLexer::TOK_ITEM:
                lex(); // TOK_ITEM

                if (l.tok == SceneLexer::TOK_IDENTIFIER) {
                    std::string id = l.str;
                    lex();

                    expect(SceneLexer::Token('{'));
                    CLValue item = ParseItem();
                    expect(SceneLexer::Token('}'));

                    CLValue key = CLValue(new CLString(context, id.c_str()));
                    CLValue val = CLValue(item);
                    room->set(key, val);
                    room->AddObject(GET_ITEM(item), pid);
                } else { // anonymous item
                    expect(SceneLexer::Token('{'));
                    CLValue item = ParseItem();
                    expect(SceneLexer::Token('}'));
                    room->AddObject(GET_ITEM(item), pid);
                }
                break;

            case SceneLexer::Token('}'):
                done = true;
                break;

            default:
                assert(false);
        }
    }
};

void SceneLoader::ParsePlaneProperty(const std::string id, Room *room, Room::PlaneID pid) {
    if (id == "size") {
        int width = ParseInt();
        int height = ParseInt();
        room->SetSize(width, height, pid);
    } else if (id == "image") {
        CLValue bgd = ParseSprite();
        room->SetBackground(bgd, pid);
    } else {
        assert(false);
    }
}

void SceneLoader::ParseRoomProperty(const std::string id, Room *room) {
    if (id == "tilt") {
        int tilt = ParseInt();
        room->SetYXRatio(tilt);
    } else if (id == "path") {
        CLValue path = ParseShape();
        room->SetPath(path);
    } else if (id == "scaling") {
        assert(l.tok == SceneLexer::TOK_IDENTIFIER);
        std::string type = l.str;
        lex();
        if (type == "perspective") {
            int a0 = ParseInt();
            int a1 = ParseInt();
            room->SetPerspectiveScaling(a0, a1);
        } else if (type == "constant") {
            float a0 = float(ParseInt()) / 100.0f;
            room->SetConstantScaling(a0);
        }
    } else {
        assert(false);
    }
}

CLValue SceneLoader::ParseItem() {
    auto *item = new Item(context);

    bool done = false;
    while (!done) {
        switch (l.tok) {
            case SceneLexer::TOK_IDENTIFIER: {
                std::string key = l.str;
                lex();
                if (l.tok == ':') {
                    lex();
                    ParseItemProperty(key, item);
                } else if (l.tok == '=') {
                    lex();
                    ParseSetSlot(key, item);
                } else {
                    assert(false);
                }

                break;
            }

            case SceneLexer::Token('}'):
                done = true;
                break;

            default:
                assert(false);
        }
    }

    return CLValue(item);
}

void SceneLoader::ParseItemProperty(const std::string id, Item *item) {
    if (id == "pos") {
        int x = ParseInt();
        int y = ParseInt();
        int layer = ParseInt();
        item->SetPosition(x, y);
        item->SetLayer(layer);
    } else if (id == "name") {
        std::string name = ParseString();
        item->SetName(name);
    } else if (id == "hotspot") {
        CLValue hotspot = ParseShape();
        item->SetHotspot(hotspot);
    } else if (id == "sprite") {
        CLValue sprite = ParseSprite();
        item->SetSprite(sprite);
    } else if (id == "walk") {
        CLValue key, val;
        key = CLValue(new CLString(context, "walkto_x"));
        val = CLValue(ParseInt());
        item->set(key, val);
        key = CLValue(new CLString(context, "walkto_y"));
        val = CLValue(ParseInt());
        item->set(key, val);
        key = CLValue(new CLString(context, "walkto_dir"));
        val = CLValue(ParseInt());
        item->set(key, val);
    } else if (id == "hide") {
        bool hide = ParseBoolean();
        item->Hide(hide);
    } else {
        assert(false);
    }
}

CLValue SceneLoader::ParseBagItem() {
    auto *bagitem = new BagItem(context);

    bool done = false;
    while (!done) {
        switch (l.tok) {
            case SceneLexer::TOK_IDENTIFIER: {
                std::string key = l.str;
                lex();
                if (l.tok == ':') {
                    lex();
                    ParseBagItemProperty(key, bagitem);
                } else if (l.tok == '=') {
                    lex();
                    ParseSetSlot(key, bagitem);
                } else {
                    assert(false);
                }

                break;
            }

            case SceneLexer::Token('}'):
                done = true;
                break;

            default:
                assert(false);
        }
    }

    return CLValue(bagitem);
}

void SceneLoader::ParseBagItemProperty(const std::string id, BagItem *bagitem) {
    if (id == "sprite") {
        CLValue sprite = ParseSprite();
        bagitem->SetSprite(sprite);
    } else if (id == "name") {
        std::string name = ParseString();
        bagitem->SetName(name);
    } else {
        assert(false);
    }
}

CLValue SceneLoader::ParseShape() {
    if (l.tok == SceneLexer::TOK_STRING) {
        std::string polygon_id = ParseString();
        CLValue shape = CLValue(new PolygonShape(context, polygon_id));
        return shape;
    }

    if (l.tok == SceneLexer::TOK_INTEGER) {
        int i1 = ParseInt();
        int i2 = ParseInt();
        int i3 = ParseInt();
        if (l.tok == SceneLexer::TOK_INTEGER) {
            // rect
            int i4 = ParseInt();
            return CLValue(new RectangularShape(context, i1, i2, i3, i4));
        } else {
            // circle
            return CLValue(new CircularShape(context, i1, i2, i3));
        }
    }

    assert(false);
    return CLValue::Null();
}


void SceneLoader::ParseSetSlot(const std::string id, CLObject *obj) {
    CLValue key = CLValue(new CLString(context, id));
    CLValue val;

    switch (l.tok) {
        case SceneLexer::TOK_INTEGER:
            val = CLValue(ParseInt());
            break;
        case SceneLexer::TOK_STRING:
            val = CLValue(new CLString(context, ParseString()));
            break;
        case SceneLexer::TOK_TRUE:
            lex();
            val = CLValue::True();
            break;
        case SceneLexer::TOK_NULL:
            lex();
            val = CLValue::Null();
            break;
        case SceneLexer::TOK_FALSE:
            lex();
            val = CLValue::False();
            break;

        default:
            assert(false);
    }

    obj->set(key, val);
}

CLValue SceneLoader::ParseSprite() {
    std::string sprite_id = ParseString();

    CLValue spr = CLValue(new Sprite(context, sprite_id));

    bool done = false;
    while (!done) {
        switch (l.tok) {
            // optional sprite translation
            case SceneLexer::TOK_TRANSLATE: {
                lex();
                int dx = ParseInt();
                int dy = ParseInt();
                GET_SPRITE(spr)->Translate(dx, dy);
                break;
            }

                // option scaling
            case SceneLexer::TOK_SCALE: {
                lex();
                int scale = ParseInt();
                GET_SPRITE(spr)->SetScaling(float(scale) / 100.0f);
                break;
            }

            default:
                done = true;
        }
    }

    return spr;
}

