// Lic:
// src/Kthura_Draw.hpp
// Kthura - Drawing engine (header)
// version: 20.09.22
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

    class KthuraDraw;


    typedef void (*DelDrawZone)(KthuraObject* obj, int ix, int iy, int scrollx, int scrolly);//, int ix = 0, int iy = 0, int scrollx = 0, int scrolly = 0);
    typedef void (*DelDrawPoint)(KthuraObject* obj, int ix, int iy, int scrollx, int scrolly);//int ix = 0, int iy = 0, int scrollx = 0, int scrolly = 0);

    class KthuraDraw {
    public:
        // #region The Abstract part every Draw Driver must have!
        virtual void DrawTiledArea(KthuraObject* obj, int ix = 0, int iy = 0, int scrollx = 0, int scrolly = 0) = 0;
        virtual void DrawObstacle(KthuraObject* obj, int ix = 0, int iy = 0, int scrollx = 0, int scrolly = 0) = 0;
        //virtual void DrawActor(KthuraActor* obj, int ix = 0, int iy = 0, int scrollx = 0, int scrolly = 0) = 0;
        virtual void DrawActor(KthuraObject* obj, int ix = 0, int iy = 0, int scrollx = 0, int scrolly = 0) = 0;
        virtual void DrawPic(KthuraObject* obj, int ix = 0, int iy = 0, int scrollx = 0, int scrolly = 0) = 0;
        virtual void DrawStretchedArea(KthuraObject* obj, int ix = 0, int iy = 0, int scrollx = 0, int scrolly = 0) = 0;
        virtual void DrawRect(KthuraObject* obj, int ix = 0, int iy = 0, int scrollx = 0, int scrolly = 0) = 0;
        KthuraAnimReset AnimReset = NULL;
        virtual int ObjectWidth(KthuraObject* obj) = 0;
        virtual int ObjectHeight(KthuraObject* obj) = 0;
        virtual bool HasTexture(KthuraObject* obj) = 0;
        // #endregion

        // #region Some static functions for Kthura's functionality in general
        static KthuraDraw* DrawDriver;
        static DelDrawZone DrawZone; // Normally zones should be ignored! Only editors should use this, and thus this delegate!
        static DelDrawPoint DrawPivot; // Only needed in editors
        static DelDrawPoint DrawExit;
        static DelDrawPoint DrawCSpot;
        static bool IgnoreVisibility ; // If set to true, invisible objects will still be shown.
       
        // Draw current layer
        static void DrawMap(KthuraLayer &layer, int scrollx = 0, int scrolly = 0, int x = 0, int y = 0);

        // Draw the current map
        static void DrawMap(Kthura &map, std::string layer, int scrollx = 0, int scrolly = 0, int x = 0, int y = 0); 
        // #endregion
    };
}
#endif