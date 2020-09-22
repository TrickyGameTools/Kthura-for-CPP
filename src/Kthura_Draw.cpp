// Lic:
// src/Kthura_Draw.cpp
// Kthura - Drawing engine
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

#include "Kthura_Draw.hpp"
#include <iostream>

namespace NSKthura {
	KthuraDraw* KthuraDraw::DrawDriver=NULL;
	static void EmptyZoneFunction(KthuraObject* obj, int ix, int iy, int scrollx, int scrolly) {}
	DelDrawZone KthuraDraw::DrawZone = EmptyZoneFunction;
	DelDrawPoint KthuraDraw::DrawPivot = NULL;
	DelDrawPoint KthuraDraw::DrawExit = NULL;
	DelDrawPoint KthuraDraw::DrawCSpot = NULL;
	bool KthuraDraw::IgnoreVisibility = false;

	void KthuraDraw::DrawMap(KthuraLayer &layer, int scrollx, int scrolly, int x, int y) {
        bool AutoRemapOnHold = Kthura::AutoMap;
        bool actorsmoved = false;
        Kthura::AutoMap = false;
        //if (layer.ObjectDrawOrder == null) layer.RemapDominance(); // No longer needed! The C++ class now handles this automatically.
        
        //foreach(KthuraObject obj in layer.ObjectDrawOrder) {
        for(auto&objid:layer._DomMap){
            auto obj=objid.second;
            if (obj->Visible() || IgnoreVisibility) {
                // std::cout << obj->Kind() << " << Draw\n";
                if (true) { // This looks useless now, but this routine will be used later in optimalisation to see if an object is actually on screen, and if not, ignore it.
                    if (!DrawDriver) { Kthura::Throw("I cannot draw without a DrawDriver!"); return; }
                        if (!DrawDriver->AnimReset) { Kthura::Throw("DrawDriver has no AnimReset function!"); return; }
                    switch (obj->EKind()) {
                    case KthuraKind::TiledArea:
                        obj->Animate(DrawDriver->AnimReset);
                        DrawDriver->DrawTiledArea(obj, x, y, scrollx, scrolly);
                        break;
                    case KthuraKind::StretchedArea:
                        obj->Animate(DrawDriver->AnimReset);
                        DrawDriver->DrawStretchedArea(obj, x, y, scrollx, scrolly);
                        break;
                    case KthuraKind::Obstacle:
                        obj->Animate(DrawDriver->AnimReset);
                        //KthuraEdit.Stages.DBG.Log($"Animation frame for Obstacle {obj.Tag}: {obj.AnimFrame}"); // Must be on comment if not debugging the standard editor or no compile-o!
                        DrawDriver->DrawObstacle(obj, x, y, scrollx, scrolly);
                        break;
                    case KthuraKind::Pic:
                        obj->Animate(DrawDriver->AnimReset);
                        DrawDriver->DrawPic(obj, x, y, scrollx, scrolly);
                        break;
                    case KthuraKind::Actor: {
                        int oldx = x;
                        int oldy = y;
                        //DrawDriver->DrawActor((KthuraActor*)obj, x, y, scrollx, scrolly);
                        DrawDriver->DrawActor(obj, x, y, scrollx, scrolly);
                        actorsmoved = actorsmoved || oldx != x || oldy != y;
                        break;
                    }
                    case KthuraKind::Zone: DrawZone(obj, x, y, scrollx, scrolly); break;
                    case KthuraKind::Pivot:
                        if (DrawPivot) DrawPivot(obj, x, y, scrollx, scrolly); break;
                    case KthuraKind::Exit:
                        if (DrawExit) DrawExit(obj, x, y, scrollx, scrolly); break;
                    case KthuraKind::CustomItem:
                        if (DrawCSpot) DrawCSpot(obj, x, y, scrollx, scrolly); break;
                    default:
                        Kthura::Throw("Unknown drawing object kind: "+obj->Kind());
                        break;
                    }
                }
            }
        }
        // Restore Automap
        Kthura::AutoMap = AutoRemapOnHold;
        if (Kthura::AutoMap && actorsmoved) layer.TotalRemap(); // If actors have moved, make sure remapping is done
    }

	void KthuraDraw::DrawMap(Kthura& map, std::string layer, int scrollx, int scrolly, int x, int y) { DrawMap(map.Layers[layer], scrollx, scrolly, x, y); }

}