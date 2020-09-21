
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