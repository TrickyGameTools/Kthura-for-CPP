// Lic:
// src/Kthura_Core.cpp
// Kthura - Core
// version: 20.09.01
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

#define Kthura_LoadChat

#ifdef Kthura_LoadChat
    #define KLC(msg) std::cout "Kthura_LoadChat: "<< (msg) <<"\n"
#else
    #define KLC(msg) 
#endif

#define kthload_assert(condition,err) if(!(condition)) { if (!StrictLoad) continue; Throw(err); return;}
#define kthload_case(thecase) else if (key==thecase)

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


    KthuraLayer* KthuraObject::GetParent() {
        return parent;
    }

    float KthuraObject::TrueScaleX() { return (float)ScaleX / 1000; }
    float KthuraObject::TrueScaleY() { return (float)ScaleY / 1000; }
    std::string KthuraObject::Kind() {
        return _Kind;
    }
    void KthuraObject::Kind(std::string k, bool force) {
        if (_Kind == "" || _Kind == "??" || force) _Kind = k;
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
    bool KthuraObject::Impassible() {
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
    Kthura* KthuraLayer::GetParent() {
        return parent;
    }
    void KthuraLayer::SetParent(Kthura* prnt) {
        if (!parent) parent = prnt;
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

    KthuraObject* KthuraLayer::LastObject() {
        return &Objects[Objects.size()-1];
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
        Layers[lay].SetParent(this);
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

    bool Kthura::StrictLoad = true;
    void Kthura::Load(jcr6::JT_Dir& sourcedir, std::string Prefix) {
        if (!sourcedir.EntryExists(Prefix + "Objects")) { Throw(Prefix + "Objects has not been found in JCR6 resource!"); return; }
        if (!sourcedir.EntryExists(Prefix + "Data")) { Throw(Prefix + "Objects has not been found in JCR6 resource!"); return; }
        /*Debug stuff that is not really suitable for C++ but kept for reference's sake!
        bool dochat = true;

        void crash(string er) = > throw new Exception($"KthuraLoad: {er}");
        void assert(bool o, string er) {
            if (!o) crash(er);
        }
        void chat(string cm) {
            if (dochat) Debug.WriteLine($"KTHURA.LOAD.CHAT: {cm}");
        }
        */


        // This is the TRUE load routine. All overloads eventually lead to this one! ;-P
        /* Unneeded in C++ due to different approach to matters!
        auto ret = new Kthura{
            MetaData = new SortedDictionary<string, string>(),
            TextureJCR = TexJcr
        };
        */
        using namespace std;
        auto m = sourcedir.StringMap(Prefix + "Data");
        for (auto K : m)
            MetaData[K.first] = K.second;
        auto olines = sourcedir.Lines(Prefix + "Objects");
        auto readlayers = false;
        bool tempautomap = AutoMap; AutoMap = false; // In order to fasten up the process this will be off temporarily. It can be turned on again, afterward!            
        string curlayername = "";
        KthuraObject* obj = NULL;
        KthuraLayer* curlayer = NULL;
        // I should have done better than this, but what works that works!
        auto cnt = 0;
        for (string rl : olines) {
            auto l = Trim(rl);
            cnt++;
            if ((!prefixed(l, "--")) && (!(prefixed(l, "#") && readlayers == false)) && l != "") {
                if (l == "LAYERS")
                    readlayers = true;
                else if (l == "__END")
                    readlayers = false;
                else if (readlayers)
                    Layers[l].GridX += 0; // Just forces to create the layer... Had to do something!
                else if (l == "NEW") {
                    //obj = new KthuraObject("?", ret.Layers[curlayername]);
                    Layers[curlayername].NewObject("??");
                    obj = Layers[curlayername].LastObject();
                    //chat($"New object in {curlayername}");
                } else {
                    auto pi = l.find_first_of('=', 0); if (pi < 0) {
                        if (!StrictLoad) continue;
                        Throw("Syntax error: \"" + l + "\" in " + to_string(cnt));
                        return;
                    }
                    auto key = Upper(Trim(l.substr(0, pi)));
                    auto value = Trim(l.substr(pi + 1));
#ifdef Kthura_LoadChat
                    cout << "Loaded line #" << cnt << "> " << key << " = \"" << value << "\";\n";
#endif
                    vector<string> s;
                    //chat($"{key} = \"{value}\"");
                    // Fine in C#, but in C++ strings don't work here // switch (key) {
                        // Layer as a whole
                    if (key == "LAYER") { //kthload_case("LAYER":
                        obj = NULL;
                        curlayername = value;
                        curlayer = &Layers[value];
                        curlayer->SetParent(this);
#ifdef Kthura_LoadChat
                        cout << "LAYER is now: " << value << "\n";
#endif
                    }// break;
                    kthload_case("GRID") { //kthload_case("GRID":
                        if (curlayer == NULL) {
                            if (!StrictLoad) continue;
                            Throw("GRID needs a layer!");
                            return;
                        }
                        s = Split(value, 'x');
                        if (s.size() != 2) {
                            if (!StrictLoad) continue;
                            Throw("GRID syntax error!"); return;
                        }
                        curlayer->GridX = stoi(Trim(s[0]));
                        curlayer->GridY = stoi(Trim(s[1]));
                    }// break;
                        // Object specific
                    kthload_case("KIND") {
                        kthload_assert(obj, "KIND: No Object");
                        obj->Kind(value, true);
                    } // break;
                    kthload_case("COORD") {
                        kthload_assert(obj, "COORD: No Object");
                        s = Split(value, ',');
                        kthload_assert(s.size() == 2, "COORD syntax error!");
                        obj->X(stoi(Trim(s[0]))); //obj.x = stoi(Trim(s[0]));
                        obj->Y(stoi(Trim(s[1]))); //obj.y = stoi(Trim(s[1]));
                        //chat($"Coordinates set({obj.x},{obj.y}) of {obj.kind}");
                    } //break;
                    kthload_case("INSERT") {
                        kthload_assert(obj, "INSERT: No Object");
                        s = Split(value, ',');
                        kthload_assert(s.size() == 2, "INSERT syntax error!");
                        obj->insertx = stoi(Trim(s[0]));// *(-1);
                        obj->inserty = stoi(Trim(s[1]));// *(-1);
                    }//break;
                    kthload_case("ROTATION") {
                        kthload_assert(obj, "ROTATION: No object");
                        obj->RotationDegrees(stoi(value));
                    }//break;
                    kthload_case("ROTATION_RAD") {
                        kthload_assert(obj, "ROTATION: No object");
                        obj->RotationRadians(stoi(value));
                        // For Kthura Degrees and not Radians will always remain the standard, though!
                    }//break;
                    kthload_case("SIZE") {
                        kthload_assert(obj, "SIZE: No Object");
                        s = Split(value, 'x');
                        kthload_assert(s.size() == 2, "SIZE syntax error!");
                        obj->w = stoi(Trim(s[0]));
                        obj->h = stoi(Trim(s[1]));
                        //break;
                    } kthload_case("TAG") {
                        kthload_assert(obj, "TAG: No Object!");
                        obj->Tag(value);
                        //break;
                    }
                    kthload_case("LABELS") {
                        kthload_assert(obj, "LABELS: No Object!");
                        obj->Labels(value);
                    } //break;
                    kthload_case("DOMINANCE") {
                        kthload_assert(obj, "DOMINANCE: No object");
                        obj->Dominance(stoi(value));
                        //break;
                    }
                    kthload_case("TEXTURE") {
                        kthload_assert(obj, "TEXTURE: No object!");
                        obj->Texture = value;
                    }//    break;
                    kthload_case("CURRENTFRAME") {
                        kthload_assert(obj, "CURRENT FRAME: No object");
                        obj->AnimFrame = stoi(value);
                        //break;
                    }
                    kthload_case("FRAMESPEED") {
                        kthload_assert(obj, "FRAME SPEED: No object");
                        obj->AnimSpeed = stoi(value);
                    } //break;
                    kthload_case("ALPHA") {
                        kthload_assert(obj, "ALPHA: No object");
                        obj->Alpha1000((int)(stod(value) * 1000));
                        // Please note! This version of Alpha has been deprecated. 
                        // It was solely implemented here to make sure I still had full support for older maps!
                    }// break;
                    kthload_case("ALPHA255") {
                        kthload_assert(obj, "ALPHA: No object");
                        obj->Alpha255(stoi(value));
                    }// break;
                    kthload_case("VISIBLE") {
                        kthload_assert(obj, "VISIBLE: No object");
                        obj->Visible = stoi(value) == 1;
                    }//break;
                    kthload_case("COLOR") {
                        kthload_assert(obj, "COLOR: No Object");
                        s = Split(value, ',');
                        kthload_assert(s.size() == 3, "COLOR syntax error!");
                        obj->R = stoi(Trim(s[0]));
                        obj->G = stoi(Trim(s[1]));
                        obj->B = stoi(Trim(s[2]));
                    }//break;
                    kthload_case("IMPASSIBLE") {
                        kthload_assert(obj, "IMPASSIBLE: No object");
                        obj->Impassible(stoi(value) == 1);
                    } // break;
                    kthload_case("FORCEPASSIBLE") {
                        kthload_assert(obj, "FORCE PASSIBLE: No object");
                        obj->ForcePassible(stoi(value) == 1);
                    } // break;
                    kthload_case("SCALE") {
                        kthload_assert(obj, "SCALE: No Object");
                        s = Split(value, ',');
                        kthload_assert(s.size() == 2, "SCALE syntax error!");
                        obj->ScaleX = stoi(Trim(s[0]));
                        obj->ScaleY = stoi(Trim(s[1]));
                    } // break;
                    kthload_case("BLEND") {
                        if (stoi(value) != 0) {
                            Throw("Alternate Blends are only supported in the BlitzMax version of Kthura!"); return;
                            // Although when rading some documentation, it may be possible to support this in C++
                        }
                    } // break;
                    else {
                        if (prefixed(key, "DATA.")) {
                            auto _Key = Trim(key.substr(5));
                            kthload_assert(obj, key+": No object");
                            obj->MetaData[_Key] = Trim(value);
                        }
                        if (!StrictLoad) {
                            Throw("Unknown object key: " + key); return;
                        }
                    } // break;
                }
            }
        }    
        AutoMap = tempautomap;
        //foreach(KthuraLayer lay in ret.Layers.Values) lay.TotalRemap();
        for (auto ilay : Layers) { ilay.second.TotalRemap(); }
        //return ret;
    }
    

    void Kthura::Load(std::string JCR, std::string Prefix) {
        auto J = jcr6::Dir(JCR);
        if (JCR_Error != "") { Throw("JCR6 file \"" + JCR + "\" could notbe loaded.\nJCR6 reported: " + JCR_Error); return; }
        Load(J, Prefix);
    }

    void Kthura::Throw(std::string err) {
        if (Kthura::Panic != NULL) {
            Kthura::Panic(err);
        } else {
            std::cout << "\x7KTHURA ERROR!\n" << err << "\n\n";
        }
    }

    int Kthura::ID() {
        return _id;
    }

    Kthura::Kthura() {
        _id = countup++;
    }

    Kthura::~Kthura() {
        KillMap();
    }

    int Kthura::countup = 0;

    

}