// Lic:
// src/Kthura_Core.cpp
// Kthura - Core
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
// C++
#include <iostream>

// Tricky's Units
#include <QuickString.hpp>

// Kthura
#include "Kthura_Core.hpp"
#include "Kthura_Draw.hpp"

namespace NSKthura {
    using namespace TrickyUnits;
    const double Pi = 3.141593;

    static std::map<std::string, KthuraKind> InitMapKind() {
        std::map<std::string, KthuraKind> ret;
        ret["Zone"] = KthuraKind::Zone;
        ret["TiledArea"] = KthuraKind::TiledArea;
        ret["StretchedArea"] = KthuraKind::StretchedArea;
        ret["Obstacle"] = KthuraKind::Obstacle;
        ret["Pic"] = KthuraKind::Pic;
        ret["Pivot"] = KthuraKind::Pivot;
        ret["Exit"] = KthuraKind::Exit;
        return ret;
    }
    static std::map<std::string, KthuraKind> MapKind = InitMapKind();


    float KthuraObject::TrueScaleX() { return (float)ScaleX / 1000; }
    float KthuraObject::TrueScaleY() { return (float)ScaleY / 1000; }
    std::string KthuraObject::Kind() {
        return _Kind;
    }
    KthuraKind KthuraObject::EKind() {
        if (_Kind[0] == '$') return KthuraKind::CustomItem;
        if (!MapKind.count(_Kind)) return KthuraKind::Unknown; // Possible when using maps from later versions, and all.
        return MapKind[_Kind];
    }

    void KthuraObject::X(int newx) {
        _x = newx;
        if (Kthura::AutoMap) parent->RemapDominance();
    }
    int KthuraObject::X() {
        return _x;
    }
    void KthuraObject::Y(int newy) {
        _y = newy;
        if (Kthura::AutoMap) parent->RemapDominance();
    }
    int KthuraObject::Y() {
        return _y;
    }
    void KthuraObject::Tag(std::string newtag) {
        _Tag = newtag;
        if (Kthura::AutoMap) parent->RemapTags();
    }
    std::string KthuraObject::Tag() {
        return _Tag;
    }
    void KthuraObject::Dominance(int newdom) {
        _Dominance = newdom;
        if (Kthura::AutoMap) parent->RemapDominance();
    }
    int KthuraObject::Dominance() {
        return _Dominance;
    }
    void KthuraObject::Labels(std::string nl) {
        _Labels = nl;
        if (Kthura::AutoMap) parent->RemapLabels();
    }
    std::string KthuraObject::Labels() {
        return _Labels;
    }
    void KthuraObject::Impassible(bool imp) {
        _impassible = imp;
        if (Kthura::AutoMap) parent->BuildBlockMap();
    }
    bool KthuraObject::Imapassible() {
        return _impassible;
    }

    void KthuraObject::ForcePassible(bool fp) {
        _forcepassible = fp;
        if (Kthura::AutoMap) parent->BuildBlockMap();
    }

    bool KthuraObject::ForcePassible() {
        return _forcepassible;
    }

    void KthuraObject::RotationRadians(double value) {
        _rotrad = value;
        _rotdeg = (int)(value * (180 / Pi));
    }

    double KthuraObject::RotationRadians() {
        return   _rotrad;
    }

    void KthuraObject::RotationDegrees(int value) {
        _rotdeg = value;
        _rotrad = (double)(((double)value) * (180 / Pi));
    }


    int KthuraObject::RotationDegrees() {
        return _rotdeg;
    }

    void KthuraObject::Alpha255(int value) {
        _alpha255 = value;
        if (_alpha255 < 0) _alpha255 = 0;
        if (_alpha255 > 255) _alpha255 = 255;
        _alpha1000 = (int)((float)(((float)value / (float)255) * (float)1000));
        if (_alpha1000 < 0) _alpha1000 = 0;
        if (_alpha1000 > 1000) _alpha1000 = 1000;
        //Debug.WriteLine($"Alpha255 set. 1000={Alpha1000}; 255={Alpha255};");
    }

    int KthuraObject::Alpha255() {
        return _alpha255;
    }

    int KthuraObject::ID() {
        return _id;
    }

    void KthuraObject::Animate(KthuraAnimReset RESET) {
        auto kind = Kind(); // Lazy! I don't you tot ell me!
        if (kind != "Obstacle" && kind != "Pic" && kind != "TiledArea" && kind != "StrechedArea") return;
        if (AnimSpeed < 0) return;
        AnimFrameSkip++;
        if (AnimFrameSkip >= AnimSpeed) {
            AnimFrameSkip = 0;
            AnimFrame++;
            // Please note that the core can NEVER know what driver is loaded. This feature should take care of that!
            if (RESET) RESET(this);
        }
    }

    bool KthuraObject::IsInZone(std::string ztag) {
        if (!parent->HasTag(ztag)) return false;
        auto zone = parent->TagMap(ztag);
        if (Kind() != "Obstacle" && Kind() != "Actor") Kthura::Throw("KthuraMap.Object.IsInzone(\"" + ztag + "\"): Main Object must be either Object or Actor");
        if (zone->Kind() != "TiledArea" && zone->Kind() != "Zone") Kthura::Throw("KthuraMap.Object.IsInzone\"" + ztag + "\"): Zone Object must be either Zone or TiledArea");
        return X() >= zone->X() && Y() >= zone->Y() && X() <= zone->X() + zone->w && Y() <= zone->Y() + zone->h;
    }


    void KthuraObject::Alpha1000(int value)				 {
        _alpha1000 = value;
        if (_alpha1000 < 0) _alpha1000 = 0;
        if (_alpha1000 > 1000) _alpha1000 = 1000;
        _alpha255 = (int)((float)(((float)value / 1000) * 255));
        if (_alpha255 < 0) _alpha255 = 0;
        if (_alpha255 > 255) _alpha255 = 255;
        //Debug.WriteLine($"Alpha1000 set. 1000={Alpha1000}; 255={Alpha255};"); // Yeah, I just imported this from C#, mind you!
    }


    
    int KthuraObject::Alpha1000() {
        return _alpha1000;
    }

    KthuraObject KthuraObject::Create(std::string Kind,KthuraLayer* p) {
        KthuraObject ret;
        ret._Kind = Kind;
        ret.parent = p;
        ret._id = p->nextID();
        return ret;
    }
    bool KthuraObject::CheckParent(KthuraLayer* p) {
        return parent==p; // Should compare the memory addresses... not the values stored in them!
    }
    int KthuraLayer::nextID() {        
        return idinc++; // dirty code!
    }
    std::map<int, KthuraObject*> KthuraLayer::GetIDMap() {
        if (ID_Map.size() == 0) {
            RemapDominance();
            RemapID();
        }
        return ID_Map;
    }
    KthuraObject* KthuraLayer::TagMap(std::string Tag) {
        if (_TagMap.count(Tag)) return _TagMap[Tag];
        Kthura::Throw("There is no object tagged: " + Tag);
        return nullptr;
    }

    std::vector<KthuraObject*>* KthuraLayer::LabelMap(std::string label) {
        if (_LabelMap.count(label)) return &_LabelMap[label];
        return &std::vector<KthuraObject*>();
    }

    bool NSKthura::KthuraLayer::HasTag(std::string Tag) {
        return _TagMap.count(Tag);
    }

    bool KthuraLayer::Blocked(int x, int y) {
        if (x < 0 || y < 0 || x >= BM_W || y >= BM_H) return true;
        return _BlockMap[(y*BM_W)+x];
    }

    bool KthuraLayer::BlockedPix(int x, int y) {
        return Blocked(floor(x/GridX),floor(y/GridY));
    }

    void KthuraLayer::RemapDominance() {
        _DomMap.clear();
        for (auto& Obj : Objects) {
            char str[35];
            sprintf_s(str, "%010d:%010d:%010d", Obj.Dominance(), Obj.Y(), Obj.X());
            _DomMap[str] = &Obj;
        }
    }
    void KthuraLayer::RemapTags() {
        _TagMap.clear();
        for (auto& Obj : Objects) {
            if (Obj.Tag() != "") {
                if (_TagMap.count(Obj.Tag()))
                    Kthura::Throw("Duplicate tag \"" + Obj.Tag() + "\"");
                else
                    _TagMap[Obj.Tag()] = &Obj;
            }
        }
    }

    void KthuraLayer::RemapLabels() {
        _LabelMap.clear();
        for (auto& Obj : Objects) {
            auto labels = TrickyUnits::Split(Obj.Labels(), ',');
            for (auto label : labels) _LabelMap[label].push_back(&Obj);
        }
    }

    void KthuraLayer::BuildBlockMap() {
    }

    void KthuraLayer::RemapID() {
        ID_Map.clear();
        for (auto& Obj : Objects) {
            ID_Map[Obj.ID()] = &Obj;
        }
    }

    void KthuraLayer::TotalRemap() {
        RemapDominance();
        RemapTags();
        RemapLabels();
        BuildBlockMap();
    }

    void KthuraLayer::NewObject(std::string Kind) {
        Objects.push_back(KthuraObject::Create(Kind, this));
        if (Kthura::AutoMap) TotalRemap();
    }

    void KthuraLayer::Kill(KthuraObject* O) {
        int idx = -1;
        if (!O->CheckParent(this)) { Kthura::Throw("Trying to kill an object that is not part of the requested layer!"); return; }
        for (int i = 0; i < Objects.size(); ++i) if (&Objects[i] == O) idx = i;
        if (idx == -1) { Kthura::Throw("Object to kill not found!"); return; }
        O = NULL;
        _TagMap.clear();
        _LabelMap.clear();
        ID_Map.clear();        
        Objects.erase(Objects.begin() + idx);
        if (Kthura::AutoMap) TotalRemap();
    }

    void KthuraLayer::Kill(int ID) {
        if (ID_Map.count(ID)) Kill(ID_Map[ID]);
    }

    void KthuraLayer::Kill(std::string Tag) {
        if (_TagMap.count(Tag)) Kill(_TagMap[Tag]);
    }

    void KthuraLayer::KillAllObjects() {
        _TagMap.clear();
        ID_Map.clear();
        _LabelMap.clear();
        _BlockMap.clear();
        Objects.clear();
        TotalRemap(); // Just an extra safety pre-caution.
    }

    


    bool Kthura::AutoMap = true;
    void (*Kthura::Panic) (std::string err) = NULL;
    KthuraLayer* Kthura::Layer(std::string lay) {
        lay = Upper(lay);
        if (!Layers.count(lay)) { Throw("No layer called \"" + lay + "\" found!"); return NULL; }
        return &Layers[lay];
    }
    void Kthura::NewLayer(std::string lay, bool force) {
        lay = Upper(lay);
        if (Layers.count(lay)) {
            if (force) 
                KillLayer(lay);
            else {
                Throw("Layer named \"" + lay + "\" already exists!");
                return;
            }
        }
        Layers[lay].TotalRemap();
    }
    void Kthura::KillLayer(std::string lay) {
        if (!Layers.count(TrickyUnits::Upper(lay))) return;
        Layers[TrickyUnits::Upper(lay)].KillAllObjects();
        Layers.erase(TrickyUnits::Upper(lay));
    }

    void Kthura::KillAllLayers() {
        for (auto& l : Layers) {
            l.second.KillAllObjects();
        }
        Layers.clear();
    }

    void Kthura::KillMap() {
        KillAllLayers();
        MetaData.clear();
    }

    void Kthura::Throw(std::string err) {
        if (Kthura::Panic != NULL) {
            Kthura::Panic(err);
        } else {
            std::cout << "\x7KTHURA ERROR!\n" << err << "\n\n";
        }
    }


    

}