// Lic:
// src/Kthura_Draw.hpp
// Kthura - Drawing engine (header)
// version: 20.08.30
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
#ifndef KTHURA_LOADED_DRAW
#define KTHURA_LOADED_DRAW

#include "Kthura_Core.hpp"

namespace NSKthura {

    typedef void (*DelDrawZone)(KthuraObject obj, int ix , int iy , int scrollx , int scrolly );//, int ix = 0, int iy = 0, int scrollx = 0, int scrolly = 0);
    typedef void (*DelDrawPoint)(KthuraObject obj, int ix , int iy , int scrollx , int scrolly );//int ix = 0, int iy = 0, int scrollx = 0, int scrolly = 0);

    class KthuraDraw {
    public:
        // #region The Abstract part every Draw Driver must have!
        virtual void DrawTiledArea(KthuraObject* obj, int ix = 0, int iy = 0, int scrollx = 0, int scrolly = 0) = 0;
        virtual void DrawObstacle(KthuraObject* obj, int ix = 0, int iy = 0, int scrollx = 0, int scrolly = 0) = 0;
        virtual void DrawActor(KthuraActor* obj, int ix = 0, int iy = 0, int scrollx = 0, int scrolly = 0) = 0;
            virtual void DrawPic(KthuraObject* obj, int ix = 0, int iy = 0, int scrollx = 0, int scrolly = 0)=0;
            virtual void DrawStretchedArea(KthuraObject* obj, int ix = 0, int iy = 0, int scrollx = 0, int scrolly = 0)=0;
            virtual void AnimReset(KthuraObject* obj)=0;
            virtual int ObjectWidth(KthuraObject* obj)=0;
            virtual int ObjectHeight(KthuraObject* obj)=0;
            virtual bool HasTexture(KthuraObject* obj)=0;
        /* MARKER
            #endregion

                #region Some static functions for Kthura's functionality in general
                static public KthuraDraw DrawDriver = null;
            static public DelDrawZone DrawZone = delegate{ }; // Normally zones should be ignored! Only editors should use this, and thus this delegate!
            static public DelDrawPoint DrawPivot = null; // Only needed in editors
            static public DelDrawPoint DrawExit = null;
            static public DelDrawPoint DrawCSpot = null;
            static public bool IgnoreVisibility = false; // If set to true, invisible objects will still be shown.

            static public void DrawMap(Kthura map, string layer, int scrollx = 0, int scrolly = 0, int x = 0, int y = 0) = > DrawMap(map.Layers[layer], scrollx, scrolly, x, y);
            static public void DrawMap(KthuraLayer layer, int scrollx = 0, int scrolly = 0, int x = 0, int y = 0) {
                bool AutoRemapOnHold = Kthura.automap;
                bool actorsmoved = false;
                Kthura.automap = false;
                if (layer.ObjectDrawOrder == null) layer.RemapDominance();
                foreach(KthuraObject obj in layer.ObjectDrawOrder) {
                    if (obj.Visible || IgnoreVisibility) {
                        if (true) { // This looks useless now, but this routine will be used later in optimalisation to see if an object is actually on screen, and if not, ignore it.
                            switch (obj.kind) {
                            case "TiledArea":
                                obj.Animate(DrawDriver.AnimReset);
                                if (DrawDriver != null) DrawDriver.DrawTiledArea(obj, x, y, scrollx, scrolly);
                                break;
                            case "StretchedArea":
                                obj.Animate(DrawDriver.AnimReset);
                                if (DrawDriver != null) DrawDriver.DrawStretchedArea(obj, x, y, scrollx, scrolly);
                                break;
                            case "Obstacle":
                                obj.Animate(DrawDriver.AnimReset);
                                //KthuraEdit.Stages.DBG.Log($"Animation frame for Obstacle {obj.Tag}: {obj.AnimFrame}"); // Must be on comment if not debugging the standard editor or no compile-o!
                                if (DrawDriver != null) DrawDriver.DrawObstacle(obj, x, y, scrollx, scrolly);
                                break;
                            case "Pic":
                                obj.Animate(DrawDriver.AnimReset);
                                if (DrawDriver != null) DrawDriver.DrawPic(obj, x, y, scrollx, scrolly);
                                break;
                            case "Actor": {
                                int oldx = x;
                                int oldy = y;
                                if (DrawDriver != null) DrawDriver.DrawActor((KthuraActor)obj, x, y, scrollx, scrolly);
                                actorsmoved = actorsmoved || oldx != x || oldy != y;
                                break;
                            }
                            case "Zone": DrawZone(obj, x, y, scrollx, scrolly); break;
                            case "Pivot":
                                DrawPivot ? .Invoke(obj, x, y, scrollx, scrolly); break;
                            case "Exit":
                                DrawExit ? .Invoke(obj, x, y, scrollx, scrolly); break;
                            default:
                                if (qstr.Prefixed(obj.kind, "$")) {
                                    DrawCSpot ? .Invoke(obj, x, y, scrollx, scrolly); break;
                                } else {
                                    throw new Exception($"Unknown drawing object kind: {obj.kind}");
                                }
                                //break;
                            }
                        }
                    }
                }
                // Restore Automap
                Kthura.automap = AutoRemapOnHold;
                if (Kthura.automap && actorsmoved) layer.TotalRemap(); // If actors have moved, make sure remapping is done
            }
            #endregion
        //THE END OF THE STUFF*/
    };

#endif