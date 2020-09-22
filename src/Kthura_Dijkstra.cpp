// Lic:
// src/Kthura_Dijkstra.cpp
// Kthura Dijkstra Driver
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

#include "Kthura_Dijkstra.hpp"
// TODO Code


namespace NSKthura {

    KthuraLayer* KAPLayer;
    bool KthuraAllowPassage(int x,int y){
        return !KAPLayer->Blocked(x, y);
    }

    std::vector<KthuraPoint> Kthura_Dijkstra::FindPath(KthuraActor* A, int x, int y) {
        std::vector<KthuraPoint> ret;
        Dijkstra D;
        KAPLayer = A->GetParent();        
        //void CalculatePath(DijkstraPassible func, int startX, int startY, int endX, int endY)
        int startx = floor(A->X() / A->GetParent()->GridX);
        int starty = floor(A->Y() / A->GetParent()->GridY);
        D.CalculatePath(KthuraAllowPassage, startx, starty, x, y);
        Success = D.Success();
        for (int i = 0; i < D.Length(); ++i) {
            KthuraPoint P;
            P.GX(D.Node(i).x);
            P.GY(D.Node(i).y);
            ret.push_back(P);
        }
        return ret;
    }
}