// Lic:
// src/Kthura_Dijkstra.cpp
// Kthura Dijkstra Driver
// version: 21.03.18
// Copyright (C) 2020, 2021 Jeroen P. Broks
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

#include "Kthura_Dijkstra.hpp"

#undef Dijkstra_Debug

#ifdef Dijkstra_Debug
#include <iostream>
using std::cout;
#endif


namespace NSKthura {

    KthuraLayer* KAPLayer;
    bool KthuraAllowPassage(int x,int y){
        return !KAPLayer->Blocked(x, y);
    }

    std::vector<KthuraPoint> Kthura_Dijkstra::FindPath(KthuraObject* A, int x, int y) {
        std::vector<KthuraPoint> ret;
        Dijkstra D;
        KAPLayer = A->GetParent();        
        //void CalculatePath(DijkstraPassible func, int startX, int startY, int endX, int endY)
        int startx = floor(A->X() / KAPLayer->GridX);
        int starty = floor(A->Y() / KAPLayer->GridY);
        D.CalculatePath(KthuraAllowPassage, startx, starty, x, y);
        Success = D.Success();
        for (int i = 0; i < D.Length(); ++i) {
            KthuraPoint P(A->GetParent());
            P.GX(D.Node(i).x);
            P.GY(D.Node(i).y);
            ret.push_back(P);
        }
        return ret;
    }

    std::vector<KthuraPoint> Kthura_Dijkstra::FindPath(int AX, int AY, KthuraLayer* Lay, int x, int y) {
        std::vector<KthuraPoint> ret;
        Dijkstra D;
        KAPLayer = Lay;// A->GetParent();
        //void CalculatePath(DijkstraPassible func, int startX, int startY, int endX, int endY)
        int startx = AX; // floor(AX / KAPLayer->GridX);
        int starty = AY; // floor(AY / KAPLayer->GridY);
        D.CalculatePath(KthuraAllowPassage, startx, starty, x, y);
        Success = D.Success();
#ifdef Dijkstra_Debug
        cout << "Dijkstra debug - succes: " << Success << "; Length: " << D.Length() <<  "("<<startx<<","<<starty<<") -> ("<<x<<","<<y<<")  \n";
#endif
        for (int i = 0; i < D.Length(); ++i) {
            KthuraPoint P(KAPLayer);
            P.GX(D.Node(i).x);
            P.GY(D.Node(i).y);
            ret.push_back(P);
#ifdef Dijkstra_Debug
            cout << "Dijkstra debug: Node #" << i << " (" << P.XX() << "(" << P.GX() << "), " << P.XY() << "(" << P.GY() << "))\n";
#endif
        }
#ifdef Dijkstra_Debug
        for (int iy = 0; iy < 50; iy++){
            printf("%02d> ", iy);
            for (int ix = 0; ix < 50; ix++) {
                if (KAPLayer->Blocked(ix, iy)) {
                    if (KthuraAllowPassage(ix, iy)) cout << "\x1b[31m";
                    cout << "X\x1b[0m";
                } else {
                    bool found = false;
                    for (int i = 0; i < D.Length(); ++i) {
                        if (D.Node(i).x == ix && D.Node(i).y == iy) {
                            found = true;
                            cout << "\x1b[32m*\x1b[0m";
                        }
                    }
                    if (!found) cout << " ";
                }
            }
                cout << std::endl;
        }
#endif
        return ret;
    }
}