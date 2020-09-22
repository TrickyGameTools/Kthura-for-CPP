// Lic:
// src/Kthura_SDL_Driver.cpp
// Kthura - Driver to make Kthura use SDL
// version: 20.09.21
// Copyright (C) 2020 Jeroen P. Broks
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software. If you use this software
// in a product, an acknowledgment in the product documentation would be
// appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
// EndLic

// C++
#include <iostream>

// Kthura
#include "Kthura_Core.hpp"
#include "Kthura_Draw.hpp"
#include "Kthura_SDL_Driver.hpp"


// Make sure you have your project linked to TQSG and Tricky Units, or this ain't gonna work! Sorry folks!
#include <TQSG.hpp>

#undef Kthura_TEST_TA
#undef Kthura_TEST_SA
#undef Kthura_TEST_OB

#define byte unsigned char

namespace NSKthura{

    using namespace TrickyUnits;
    using namespace std;

    TQSG_Image* GetTex(KthuraObject* obj);

    void SDL_AnimReset(KthuraObject * O){
        auto Tex = GetTex(O);
        if (O->AnimFrame >= Tex->Frames()) O->AnimFrame = 0;
    }

    typedef struct TKSDT {
        TQSG_Image Img;
        int LastCall = 0;
        int LastMapID = 0;
    } TKSDT;

    map<string, TKSDT> KSDT; // Kthura SDL Driver Textures

    void LoadTex(string tag,string tex,Kthura*Map){
        static int actions = 0;
        cout << "Load Tex " << tex << " onto tag " << tag << "\n";
        if (!Map) {
            Kthura::Throw("Internal error! LoadTex did not properly receive a Kthura Map");
            return;
        }
        if (!Map->TexDir) {
            Kthura::Throw("No Texture JCR resource set");
            return;
        }
        if (!Map->TexDir->EntryExists(tex)) {
            Kthura::Throw("Texture \"" + tex + "\" not found!");
            return;
        }
        auto Now = floor(SDL_GetTicks() / 1000);
        KSDT[tag].LastCall = Now;
        KSDT[tag].LastMapID = Map->ID();
        KSDT[tag].Img.Create(*(Map->TexDir), tex);
        if (prefixed(tag, "Obstacle::")) KSDT[tag].Img.HotBottomCenter();
        // remove texture which were not used in a too long time.
        actions = (actions + 1) % 250;
        if (!actions) {
            vector<string>KILL;
            for (auto& T : KSDT) {
                if (abs(Now - T.second.LastCall) > 1800 + ((T.second.LastMapID == Map->ID()) * 3200)) {
                    KILL.push_back(T.first);
                }
            }
            for (auto K : KILL) KSDT.erase(K);
        }

    }

    TQSG_Image* GetTex(KthuraObject*obj) {
        TQSG_Image* ret = NULL;
        string Tag = obj->Kind() + "::" + obj->Texture;
        auto lay = obj->GetParent();
        auto map = lay->GetParent();
        // cout << "Addr.Lay = " << lay << "; Addr.Map = " << map << "\n";
        if (!KSDT.count(Tag))
            LoadTex(Tag, obj->Texture,map);
        if (!KSDT.count(Tag)) {
            Kthura::Throw("Could not retrieve texture: " + obj->Texture);
            return NULL; // Safety precaution
        }
        auto TEX = &KSDT[Tag];
        TEX->LastCall = floor(SDL_GetTicks() / 1000);
        TEX->LastMapID = map->ID();
        return &(TEX->Img);        
    }

    
    void Kthura_Draw_SDL_Driver::Init() {
        if (KthuraDraw::DrawDriver) delete KthuraDraw::DrawDriver;
        Kthura_Draw_SDL_Driver *Me = new Kthura_Draw_SDL_Driver(); 
        KthuraDraw::DrawDriver = Me;
        Me->AnimReset = SDL_AnimReset;
    }
    void Kthura_Draw_SDL_Driver::DrawTiledArea(KthuraObject* obj, int ix, int iy, int scrollx, int scrolly) {
#ifdef Kthura_TEST_TA
        TQSG_Color(255, 0, 0);
        TQSG_Rect(obj->X() + ix-scrollx, obj->Y() + iy-scrolly, obj->w, obj->h);
#else
        TQSG_Color(obj->R, obj->G, obj->B);
        GetTex(obj)->Tile(obj->X() + ix - scrollx, obj->Y() + iy - scrolly, obj->w, obj->h,obj->AnimFrame,obj->insertx,obj->inserty);        
#endif
    }

    void Kthura_Draw_SDL_Driver::DrawObstacle(KthuraObject* obj, int ix, int iy, int scrollx, int scrolly) {
#ifdef Kthura_TEST_OB
        TQSG_Color(255, 180, 0);
        for (int i = 0; i < 50; i++) TQSG_Circle(obj->X() + ix - scrollx, obj->Y() + iy - scrolly, i);
#else
        TQSG_Color(obj->R, obj->G, obj->B);
        auto oldr = TQSG_Rotate();
        TQSG_Rotate(obj->RotationDegrees());
        GetTex(obj)->XDraw(obj->X(), obj->Y(),obj->AnimFrame);
        TQSG_Rotate(oldr);
#endif
    }

    void Kthura_Draw_SDL_Driver::DrawActor(KthuraActor* obj, int ix, int iy, int scrollx, int scrolly) {
        //Kthura::Throw("Actors not yet supported");
        auto tx = GetTex(obj);
        if (tx) {
            cout << obj->Walking << obj->Moving << "???\n";
            obj->UpdateMoves();
            TQSG_Color((byte)obj->R, (byte)obj->G, (byte)obj->B);
            //TQMG.SetAlphaFloat((float)obj.Alpha1000 / 1000);
            TQSG_SetAlpha((byte)obj->Alpha255());
            //TQMG.RotateRAD((float)obj.RotationRadians);
            TQSG_Rotate(obj->RotationDegrees());                        
            SetScale(obj->ScaleX/1000, obj->ScaleY/1000);
            if (obj->AnimFrame >= tx->Frames()) obj->AnimFrame = 0;
            tx->XDraw(obj->X() + ix - scrollx, obj->Y() + iy - scrolly, obj->AnimFrame);
            SetScale(1, 1);
            TQSG_RotateRAD(0);
            TQSG_SetAlpha(255);
        } else {
            //CrashOnNoTex ? .Invoke($"Actor-texture '{obj.Texture}' did somehow not load?");
            Kthura::Throw("Actor-texture '" + obj->Texture + "' did somehow not load?");
        }
    }

    void Kthura_Draw_SDL_Driver::DrawPic(KthuraObject* obj, int ix, int iy, int scrollx, int scrolly) {
        Kthura::Throw("No Kthura-PIC support (yet)");
        // This is an unofficial object kind only put in in order to convert Teddybear levels
    }

    void Kthura_Draw_SDL_Driver::DrawStretchedArea(KthuraObject* obj, int ix, int iy, int scrollx, int scrolly) {
#ifdef Kthura_TEST_SA
        TQSG_Color(0, 255, 0);
        TQSG_Rect(obj->X() + ix - scrollx, obj->Y() + iy - scrolly, obj->w, obj->h);
#else
        TQSG_Color(obj->R, obj->G, obj->B);
        GetTex(obj)->StretchDraw(obj->X() + ix - scrollx, obj->Y() + iy - scrolly, obj->w, obj->h);
#endif
    }

    int Kthura_Draw_SDL_Driver::ObjectWidth(KthuraObject* obj) {
        switch (obj->EKind()) {
        case KthuraKind::TiledArea:
            case KthuraKind::StretchedArea:
            case KthuraKind::Zone:
                return obj->w;
            case KthuraKind::Obstacle:
#ifdef Kthura_TEST_OB
                return 50;
#else
                Kthura::Throw("I cannot yet see the width of an obstacle");
                return 0;
#endif
            case KthuraKind::Pivot:
            case KthuraKind::CustomItem:
                return 0;
            case KthuraKind::Actor:
                Kthura::Throw("I cannot yet see the width of an actor");
                return 0;
            default:
                Kthura::Throw("Unknown object kind " + obj->Kind());                
        }
        return 0;
    }

    int Kthura_Draw_SDL_Driver::ObjectHeight(KthuraObject* obj) {
        switch (obj->EKind()) {
        case KthuraKind::TiledArea:
        case KthuraKind::StretchedArea:
        case KthuraKind::Zone:
            return obj->h;
        case KthuraKind::Obstacle:
#ifdef Kthura_TEST_OB
            return 50;
#else
            Kthura::Throw("I cannot yet see the width of an obstacle");
            return 0;
#endif
        case KthuraKind::Pivot:
        case KthuraKind::CustomItem:
            return 0;
        case KthuraKind::Actor:
            Kthura::Throw("I cannot yet see the width of an actor");
            return 0;
        default:
            Kthura::Throw("Unknown object kind " + obj->Kind());
        }
        return 0;
    }

    bool Kthura_Draw_SDL_Driver::HasTexture(KthuraObject* obj) {
        // TODO: Make this happen!
        return false;
    }



}