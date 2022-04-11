// Lic:
// src/Kthura_Core.cpp
// Kthura - Core
// version: 22.01.06
// Copyright (C) 2020, 2021, 2022 Jeroen P. Broks
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

// This is for loading older maps without an "Options" entry, to prevent conflicts.
// Please note that this is a deprecated feature, in order to prevent trouble on older
// Maps written with the BlitzMax and C# versions of the Kthura editor.
#define DEFAULT_CASESENSENSITIVE_LOAD

#define BLOCKMAP_IGNOREOBSTACLES
#define BLOCKMAP_IGNOREPIC

#undef DEBUG_CASEINSENSITVETAGS

// C++
#include <iostream>

// Tricky's Units
#include <QuickString.hpp>


// Kthura
#include "Kthura_Core.hpp"
#include "Kthura_Draw.hpp"


#define HideAndShowDebug

#undef Kthura_LoadChat

#ifdef Kthura_LoadChat
	#define KLC(msg) std::cout<< "Kthura_LoadChat: "<< (msg) <<"\n"
#else
	#define KLC(msg) std::cout<<"";
#endif

#define kthload_assert(condition,err) if(!(condition)) { if (!StrictLoad) continue; Throw(err); return;}
#define kthload_case(thecase) else if (key==thecase)

#define kthobjret(what)  if (A) return A->O.what; else return O->what
#define kthobjretf(what) if (A) return A->O.what(); else return O->what()
#define kthobjdef(what)  if (A) A->O.what = value; else O->what = value
#define kthobjset(what)  if (A) A->O.what(value); else O->what(value)
#define kthactdef(what)  if (!A) Kthura::Throw("Definition Actor-Only value"); else A->what=value
#define kthactret(what)  if (!A) Kthura::Throw("Trying to return from Actor-Only member"); else return A->what
#define kthactset(what)  if (!A) Kthura::Throw("Setting Actor-Only property") else A->what(value);


namespace NSKthura {
	using namespace TrickyUnits;
	using namespace std;
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
		ret["Actor"] = KthuraKind::Actor;
		ret["Rect"] = KthuraKind::Rect;
		ret["Custom"] = KthuraKind::Custom;
		return ret;
	}
	static std::map<std::string, KthuraKind> MapKind = InitMapKind();


	KthuraLayer* KthuraRegObject::GetParent() {
		return parent;
	}

	float KthuraRegObject::TrueScaleX() { return (float)ScaleX / 1000; }
	float KthuraRegObject::TrueScaleY() { return (float)ScaleY / 1000; }
	std::string KthuraRegObject::Kind() {
		return _Kind;
	}
	void KthuraRegObject::Kind(std::string k, bool force) {
		if (_Kind == "" || _Kind == "??" || force) _Kind = k;
	}

	KthuraKind KthuraRegObject::EKind() {
		if (_Kind[0] == '$') return KthuraKind::CustomItem;
		if (!MapKind.count(_Kind)) return KthuraKind::Unknown; // Possible when using maps from later versions, and all.
		return MapKind[_Kind];
	}

	void KthuraRegObject::X(int newx) {
		_x = newx;
		//if (!parent) Kthura::Throw("KthuraObject::X(" + to_string(newx) + "): parent is null pointer!");
		if (Kthura::AutoMap) parent->RemapDominance();
	}
	int KthuraRegObject::X() {
		return _x;
	}
	void KthuraRegObject::Xp(int add) { X(X() + add); }
	void KthuraRegObject::Yp(int add) { Y(Y() + add); }
	void KthuraRegObject::Xm(int sub) { X(X() - sub); }
	void KthuraRegObject::Ym(int sub) { Y(Y() - sub); }
	void KthuraRegObject::Y(int newy) {
		_y = newy;
		if (Kthura::AutoMap) parent->RemapDominance();
	}
	int KthuraRegObject::Y() {
		return _y;
	}
	void KthuraRegObject::Tag(std::string newtag) {
		_Tag = newtag;
		if (Kthura::AutoMap) parent->RemapTags();
	}
	void KthuraRegObject::TagNoRemap(std::string newtag) {
		_Tag = newtag;
	}
	std::string KthuraRegObject::Tag() {
		return _Tag;
	}
	void KthuraRegObject::Dominance(int newdom) {
		_Dominance = newdom;
		if (Kthura::AutoMap) parent->RemapDominance();
	}
	int KthuraRegObject::Dominance() {
		return _Dominance;
	}
	void KthuraRegObject::Labels(std::string nl) {
		_Labels = nl;
		if (Kthura::AutoMap) parent->RemapLabels();
	}
	std::string KthuraRegObject::Labels() {
		return _Labels;
	}
	void KthuraRegObject::Impassible(bool imp) {
		_impassible = imp;
		if (Kthura::AutoMap) parent->BuildBlockMap();
	}
	bool KthuraRegObject::Impassible() {
		return _impassible;
	}

	void KthuraRegObject::ForcePassible(bool fp) {
		_forcepassible = fp;
		if (Kthura::AutoMap) parent->BuildBlockMap();
	}

	bool KthuraRegObject::ForcePassible() {
		return _forcepassible;
	}

	void KthuraRegObject::RotationRadians(double value) {
		_rotrad = value;
		_rotdeg = (int)(value * (180 / Pi));
	}

	double KthuraRegObject::RotationRadians() {
		return   _rotrad;
	}

	void KthuraRegObject::RotationDegrees(int value) {
		_rotdeg = value;
		_rotrad = (double)(((double)value) * (180 / Pi));
	}


	int KthuraRegObject::RotationDegrees() {
		return _rotdeg;
	}

	void KthuraRegObject::Alpha255(int value) {
		_alpha255 = value;
		if (_alpha255 < 0) _alpha255 = 0;
		if (_alpha255 > 255) _alpha255 = 255;
		_alpha1000 = (int)((float)(((float)value / (float)255) * (float)1000));
		if (_alpha1000 < 0) _alpha1000 = 0;
		if (_alpha1000 > 1000) _alpha1000 = 1000;
		//Debug.WriteLine($"Alpha255 set. 1000={Alpha1000}; 255={Alpha255};");
	}

	int KthuraRegObject::Alpha255() {
		return _alpha255;
	}

	int KthuraRegObject::ID() {
		return _id;
	}

	void KthuraObject::Animate(KthuraAnimReset RESET) {
		auto kind = Kind(); // Lazy! I don't you tot ell me!
		if (kind != "Obstacle" && kind != "Pic" && kind != "TiledArea" && kind != "StrechedArea") return;
		if (AnimSpeed() < 0) return;
		AnimFrameSkip++;
		if (RESET) RESET(this);
		if (AnimFrameSkip >= AnimSpeed()) {
			AnimFrameSkip = 0;
			AnimFrame(AnimFrame() + 1);
			// Please note that the core can NEVER know what driver is loaded. This feature should take care of that!
			if (RESET) RESET(this);
		}
	}

	void KthuraObject::Ym(int value) { kthobjset(Ym); }
	void KthuraObject::Kind(std::string k, bool force) {
		if (A) { Kthura::Throw("Kind of actors cannot be changed. Once an actor always an actor!"); return; }
		if (k == "Actor") { Kthura::Throw("Actors cannot be created from regular objects!"); return; }
		O->Kind(k, force);
	}
	bool KthuraObject::IsInZone(string ztag) {
		//kthobjret(IsInZone(zone)); 
		auto parent = GetParent();
		if (!parent->HasTag(ztag)) return false;
		auto zone = parent->TagMap(ztag);
		if (Kind() != "Obstacle" && Kind() != "Actor") Kthura::Throw("KthuraMap.Object.IsInzone(\"" + ztag + "\"): Main Object must be either Object or Actor (it is a " + Kind() + ")");
		if (zone->Kind() != "TiledArea" && zone->Kind() != "Zone" && zone->Kind()!="StretchedArea" && zone->Kind() != "Rect") Kthura::Throw("KthuraMap.Object.IsInzone\"" + ztag + "\"): Zone Object must be either Zone or TiledArea");
		return X() >= zone->X() && Y() >= zone->Y() && X() <= zone->X() + zone->W() && Y() <= zone->Y() + zone->H();
	}
	bool KthuraObject::CheckParent(KthuraLayer* p) { kthobjret(CheckParent(p)); }

	bool KthuraObject::PixInObj(int x, int y) {
		switch (EKind()) {
		case KthuraKind::Zone:
		case KthuraKind::TiledArea:
		case KthuraKind::StretchedArea:
		case KthuraKind::Rect:
			return (x >= X() && x <= X() + W() && y >= Y() && y <= Y() + H());
		case KthuraKind::Actor:
		case KthuraKind::Obstacle:
		{
			int hw = floor(KthuraDraw::DrawDriver->ObjectWidth(this) / 2);
			int hh = floor(KthuraDraw::DrawDriver->ObjectHeight(this) );
			int sx = X() - hw;
			int sy = Y() - hh;
			int ex = X() + hw;
			int ey = Y();
			//cout << "Check (" << x << "," << y << ") within (" << sx << "," << sy << ")-(" << ex << "," << ey << ") >> " << (x >= sx && x <= ex && y >= sy && y <= ey) << "\n";
			return (x >= sx && x <= ex && y >= sy && y <= ey);
		}
		case KthuraKind::Pic:
			return (x >= X() && Y() >= Y() && X() <= X() + KthuraDraw::DrawDriver->ObjectWidth(this) && Y() <= Y() + KthuraDraw::DrawDriver->ObjectHeight(this));
		default:
			return false;
		}
	}

	std::string KthuraObject::PixArea() {
		switch (EKind()) {
		case KthuraKind::Zone:
		case KthuraKind::TiledArea:
		case KthuraKind::StretchedArea:
		case KthuraKind::Rect:
			return "A:("+to_string(X())+","+to_string(Y())+") - (" + to_string(X()+W()) + "," + to_string(Y()+H()) + ")";
		case KthuraKind::Actor:
		case KthuraKind::Obstacle:
		{
			int hw = floor(KthuraDraw::DrawDriver->ObjectWidth(this) / 2);
			int hh = floor(KthuraDraw::DrawDriver->ObjectHeight(this));
			int sx = X() - hw;
			int sy = Y() - hh;
			int ex = X() + hw;
			int ey = Y();
			return "O:(" + to_string(sx) + "," + to_string(sy) + ") - (" + to_string(ex) + "," + to_string(ey) + ")";
		}
		case KthuraKind::Pic:
			return "Pic not yet supported";
		default:
			return "No PixArea suppoed for "+Kind();
		}
	}

	std::string KthuraObject::DumpData() {
		string ret{ "" };
		cout << "Receiving from " <<Kind()<<" "<< Tag() << ":"<<ID()<<"\n";
		if (EKind() == KthuraKind::Actor) {
			return "";
		}

		auto md = &(O->MetaData);
		for (auto fy : *md) {
			ret += fy.first + " = " + fy.second+"\n";
		}
		return ret;
	}

	

	bool KthuraObject::Walking() { kthactret(Walking); }
	void KthuraObject::Walking(bool value) { kthactdef(Walking); }
	void KthuraObject::Moving(bool value) { kthactdef(Moving); }
	void KthuraObject::NotInMotionThen0(bool value) { kthactdef(NotInMotionThen0); }
	void KthuraObject::Wind(std::string value) { kthactdef(Wind); }
	void KthuraObject::MoveTo(int x, int y) { if (!A) Kthura::Throw("Actors-Only method: UpdateMoves()"); A->MoveTo(x, y); }
	void KthuraObject::MoveTo(KthuraObject* obj) { if (!A) Kthura::Throw("Actors-Only method: UpdateMoves()"); A->MoveTo(obj); }
	void KthuraObject::MoveTo(std::string ObjTag) { MoveTo(GetParent()->TagMap(ObjTag)); }
	void KthuraObject::WalkTo(int x, int y, bool real) { if (!A) Kthura::Throw("Actors-Only method: UpdateMoves()"); A->WalkTo(x, y, real); }
	void KthuraObject::WalkTo(KthuraObject* obj) { if (!A) Kthura::Throw("Actors-Only method: UpdateMoves()"); A->WalkTo(obj); }
	void KthuraObject::WalkTo(std::string ObjTag) { WalkTo(GetParent()->TagMap(ObjTag)); }
	bool KthuraObject::Moving() { return A && A->Moving; }
	bool KthuraObject::NotInMotionThen0() { kthactret(NotInMotionThen0); }
	string KthuraObject::Wind() { kthactret(Wind); }
	void KthuraObject::UpdateMoves() { if (!A) Kthura::Throw("Actors-Only method: UpdateMoves()"); A->UpdateMoves(); }
	void KthuraObject::Xm(int value) { kthobjset(Xm); }
	void KthuraObject::Yp(int value) { kthobjset(Yp); }
	void KthuraObject::Xp(int value) { kthobjset(Xp); }

	bool KthuraRegObject::IsInZone(std::string ztag) {
		if (!parent->HasTag(ztag)) return false;
		auto zone = parent->TagMap(ztag);
		if (Kind() != "Obstacle" && Kind() != "Actor") Kthura::Throw("KthuraMap.Object.IsInzone(\"" + ztag + "\"): Main Object must be either Object or Actor (it is a " + Kind() + ")");
		if (zone->Kind() != "TiledArea" && zone->Kind() != "Zone" && zone->Kind() != "Rect" && zone->Kind() != "Rect") Kthura::Throw("KthuraMap.Object.IsInzone\"" + ztag + "\"): Zone Object must be either Zone or TiledArea");
		return X() >= zone->X() && Y() >= zone->Y() && X() <= zone->X() + zone->W() && Y() <= zone->Y() + zone->H();
	}


	void KthuraRegObject::Alpha1000(int value) {
		_alpha1000 = value;
		if (_alpha1000 < 0) _alpha1000 = 0;
		if (_alpha1000 > 1000) _alpha1000 = 1000;
		_alpha255 = (int)((float)(((float)value / 1000) * 255));
		if (_alpha255 < 0) _alpha255 = 0;
		if (_alpha255 > 255) _alpha255 = 255;
		//Debug.WriteLine($"Alpha1000 set. 1000={Alpha1000}; 255={Alpha255};"); // Yeah, I just imported this from C#, mind you!
	}



	int KthuraRegObject::Alpha1000() {
		return _alpha1000;
	}

	KthuraShObject KthuraObject::Create(std::string Kind, KthuraLayer* p) {
		//KthuraObject ret(Kind, p);
		auto ret = make_shared<KthuraObject>(Kind, p);
		//ret._Kind = Kind;
		//ret.parent = p;
		ret->_id = p->nextID();
		return ret;
	}
	KthuraShObject KthuraObject::Import(KthuraActor* Act) {
		//KthuraObject ret(Act);
		auto ret = make_shared<KthuraObject>(Act);
		ret->_id = Act->GetParent()->nextID();
		Act->SetObject(ret.get());
		cout << "Debug: Actor imported on layer: " << ret->A->GetParent()->GetCreationName() << endl;
		cout << "Debug: Extra check: " << ret->GetParent()->GetCreationName() << endl;
		if (ret->GetParent() != ret->A->GetParent()) cout << "DEBUG: \x7 WOWOWOWOW! Parent mismatch!" << endl;
		if (ret.get() != Act->GetObject()) cout << "DEBUG: \x7 WOWOWOW! Object data not in order!" << endl;
		return ret;
	}
	bool KthuraRegObject::CheckParent(KthuraLayer* p) {
		return parent == p; // Should compare the memory addresses... not the values stored in them!
	}
	KthuraRegObject::KthuraRegObject() {}
	KthuraRegObject::KthuraRegObject(KthuraLayer* p) { parent = p; _Kind = "???"; }
	KthuraRegObject::KthuraRegObject(KthuraLayer* p, std::string setKind) { parent = p; _Kind = setKind; }
	void KthuraRegObject::PSync(KthuraActor* A) {
		if (&A->O != this) { Kthura::Throw("Sync error!"); }
		parent = A->GetParent();
	}
	Kthura* KthuraLayer::GetParent() {
		return parent;
	}
	void KthuraLayer::SetParent(Kthura* prnt, std::string lname) {
		_layername = lname;
		if (!parent) parent = prnt;
	}
	std::string KthuraLayer::GetCreationName() {
		return _layername;
	}
	int KthuraLayer::nextID() {
		return idinc++; // dirty code!
	}
	int KthuraLayer::CountObjects() {
		return Objects.size();// +Actors.size();
	}
	KthuraObject* KthuraLayer::ObjFIdx(int index) {
		//int aindex = index - Objects.size();
		//cout << "DEBUG OBJFIDX: Object Index: " << index << "/"<<Objects.size()<<"; Actor Index: " << aindex << "/"<<Actors.size()<<".\n";
		if (index < Objects.size())
			return Objects[index].get();
		//else if (aindex < Actors.size())
		//    return &Actors[index];
		return NULL;
	}
	KthuraShObject KthuraLayer::FindShObjByID(int index) {
		for (auto o : Objects) if (o->ID() == index) return o;
		return nullptr;
	}
	std::map<int, KthuraObject*> KthuraLayer::GetIDMap() {
		if (ID_Map.size() == 0) {
			RemapDominance();
			RemapID();
		}
		return ID_Map;
	}
	KthuraObject* KthuraLayer::TagMap(std::string Tag) {
		if (parent->_ignorecase_tags) Tag = Upper(Tag);
		if (_TagMap.count(Tag)) return _TagMap[Tag];
		//for (auto lck : parent->Layers) std::cout << "Layer: " << lck.first << "; check: " << (&lck.second == this) << "!";
		std::cout << "Error in layer: '" << GetCreationName() << "'\n";
		for (auto& dbg : _TagMap) std::cout << "TagNotFound - CheckUp: " << dbg.second->Kind() << " " << dbg.first << "\n";
		Kthura::Throw("There is no object tagged: " + Tag);
		return nullptr;
	}

	std::string KthuraLayer::TagList() {
		std::string ret = "";
		for (auto& tl : _TagMap) {
			if (ret != "") ret += ",";
			ret += tl.first;
		}
		return ret;
	}

	std::vector<KthuraObject*>* KthuraLayer::LabelMap(std::string label) {
		if (parent->_ignorecase_labels) label = Upper(label);
		if (_LabelMap.count(label)) return &_LabelMap[label];
		return &std::vector<KthuraObject*>();
	}

	KthuraObject* KthuraLayer::LastObject() {
		return Objects[Objects.size() - 1].get();
	}

	std::string KthuraLayer::BlockMapStringDump() {
		BuildBlockMap();
		string ret = "";
		for (int y = 0; y <= BlockMapBoundH; ++y) {
			if (y) ret += "\n";
			for (int x = 0; x <= BlockMapBoundW; ++x) {
				if (Blocked(x, y)) ret += "X"; else ret += "_";
			}
		}
		return ret;
	}

	bool NSKthura::KthuraLayer::HasTag(std::string Tag) {
#ifdef DEBUG_CASEINSENSITVETAGS
		cout << "HasTag(" << Tag << ") Ignore case: " << parent->_ignorecase_tags;
#endif
		if (parent->_ignorecase_tags) Tag = Upper(Tag);
#ifdef DEBUG_CASEINSENSITVETAGS
		cout << "'; \tChecking: " << Tag << ";\tResult: " << _TagMap.count(Tag) << endl;
		for (auto f : _TagMap) cout << "This layer has tag: " << f.first << endl;
#endif
		return _TagMap.count(Tag);
	}

	bool KthuraLayer::Blocked(int x, int y) {
		if (x < 0 || y < 0 || x >= BlockMapBoundW || y >= BlockMapBoundH) return true;
		return _BlockMap[(y * (BlockMapBoundW + 1)) + x];
	}

	bool KthuraLayer::BlockedPix(int x, int y) {
		return Blocked(floor(x / GridX), floor(y / GridY));
	}

	void KthuraLayer::RemapDominance() {
		_DomMap.clear();
		//for (auto& Obj : Objects) {
		for (int i = 0; i < CountObjects(); i++) {
			auto Obj = ObjFIdx(i);
			char str[50];
			sprintf_s(str, "%010d:%010d:%010d:%010d", Obj->Dominance(), Obj->Y(), Obj->X(),Obj->ID());
			_DomMap[str] = Obj;
		}
	}
	void KthuraLayer::RemapTags() {
		parent->_ignorecase_tags = Upper(parent->Options.Value("IgnoreCase", "Tags")) == "YES";
		_TagMap.clear();
		//        for (auto& Obj : Objects) {
		for (int i = 0; i < CountObjects(); i++) {
			auto Obj = ObjFIdx(i);
			auto ObjTag = Obj->Tag();
			if (parent->_ignorecase_tags) ObjTag = Upper(ObjTag);
			if (ObjTag != "") {
				if (_TagMap.count(ObjTag)) {
					if (parent->_autoretag) {
						int nti{ 0 };
						string nt;
						do { nt = ObjTag + "__" + to_string(++nti) + "___"; } while (_TagMap.count(nt));
						Obj->TagNoRemap(nt);
						_TagMap[nt] = Obj;
					} else {
						Kthura::Throw("Duplicate tag \"" + ObjTag + "\" (Layer:" + Obj->GetParent()->GetCreationName() + ". Kind: " + Obj->Kind() + ")");
					}
				} else
					_TagMap[ObjTag] = Obj;
			}
		}
	}

	void KthuraLayer::RemapLabels() {
		parent->_ignorecase_labels = Upper(parent->Options.Value("IgnoreCase", "Labels")) == "YES";
		_LabelMap.clear();
		//        for (auto& Obj : Objects) {
		for (int i = 0; i < CountObjects(); i++) {
			auto Obj = ObjFIdx(i);
			auto labels = TrickyUnits::Split(Obj->Labels(), ',');
			for (auto label : labels) {
				if (parent->_ignorecase_labels)
					_LabelMap[Upper(label)].push_back(Obj);
				else
					_LabelMap[label].push_back(Obj);
			}
		}
	}

	void KthuraLayer::BuildBlockMap() {
		auto GW = GridX;
		auto GH = GridY;
		int X, Y, W, H; //BX, BY,
		int TX, TY, AX, AY, TW, TH;
		int BoundX = 0, BoundY = 0;
		int iw, tiw, ih, tih;
		// Let's first get the bounderies
		for (auto& O : Objects) {
			X = O->X(); if (X < 0) X = 0;
			Y = O->Y(); if (Y < 0) Y = 0;
			W = O->W() - 1; if (W < 0) W = 0;
			H = O->H() - 1; if (H < 0) H = 0;
			switch (O->EKind()) {
			case KthuraKind::TiledArea:
			case KthuraKind::Zone:
			case KthuraKind::StretchedArea:
				TX = ceil((double)((X + W) / GW));
				TY = ceil((double)((Y + H) / GH));
				if (TX > BoundX) BoundX = TX;
				if (TY > BoundY) BoundY = TY;
				break;
			case KthuraKind::Obstacle:
#ifndef BLOCKMAP_IGNOREOBSTACLES
				TX = floor((double)(X / GW));
				TY = floor((double)(Y / GH));
				if (TX > BoundX) BoundX = TX;
				if (TY > BoundY) BoundY = TY;
#endif
				break;
			case KthuraKind::Pic:
				TX = floor((double)(X / GW));
				TY = floor((double)(Y / GH));
				if (TX > BoundX) BoundX = TX;
				if (TY > BoundY) BoundY = TY;
				break;
			}
		}
		BlockMapBoundW = BoundX;
		BlockMapBoundH = BoundY;
		BM_W = BoundX;
		BM_H = BoundY;
		//BlockMap = new bool[BoundX + 1, BoundY + 1];
		_BlockMap.clear(); for (int i = 0; i < (BoundX + 1) * (BoundY + 1); ++i) _BlockMap.push_back(false); // Primitive, but for now all I got.
		// And now for the REAL work.		
		for (auto O : Objects) {
			if (O->Impassible()) {
				//Debug.WriteLine($"Checking object {O->kind}; {O->Texture}; {O->Labels}");
				X = O->X(); if (X < 0) X = 0;
				Y = O->Y(); if (Y < 0) Y = 0;
				W = O->W() - 1; if (W < 0) W = 0;
				H = O->H() - 1; if (H < 0) H = 0;
				switch (O->EKind()) {
				case KthuraKind::TiledArea:
				case KthuraKind::Zone:
				case KthuraKind::StretchedArea:
					//Kthura.EDITTORLOG($"Working on Impassible {O.kind} {O.Tag}");
					TX = floor((double)X / GW);
					TY = floor((double)Y / GH);
					TW = ceil((double)((X + W) / GW));
					TH = ceil((double)((Y + H) / GH));
					//Print "DEBUG: Blockmapping area ("+TX+","+TY+") to ("+TW+","+TH+")"
					for (AX = TX; AX <= TW; AX++) {
						for (AY = TY; AY <= TH; AY++) {
							//for (AX = TX; AX < TW; AX++) {
							//    for (AY = TY; AY < TH; AY++) {
							try {
								//Kthura.EDITTORLOG($"Blocking {AX},{AY}");
								//cout <<"IMPASSIBLE: "<< O.Kind() << ": " << O.Tag() << " (" << O.X() << "," << O.Y() << ") " << O.W() << "x" << O.H() << " fordata X(" << AX << ":" << TX << ":" << TW << ") Y(" << AY << ":" << TY << ":" << TH << ")\n";
								_BlockMap[AX + (AY * (BoundX + 1))] = true;
							} catch (...) {
								//throw new Exception($"BlockMap[{AX},{AY}]: Out of bounds ({BlockMap.GetLength(0)}x{BlockMap.GetLength(1)})");
								Kthura::Throw("BlockMap[" + to_string(AX) + "," + to_string(AY) + "]: Out of bounds (" + to_string(BlockMapBoundW) + "x" + to_string(BlockMapBoundH) + ")");
							}
						}
					}
					break;
				case KthuraKind::Obstacle:
#ifndef BLOCKMAP_IGNOREOBSTACLES
					TX = floor((double)(X / GW));
					TY = floor((double)((Y - 1) / GH));
					//BlockMap[TX, TY] = true;
					if (AX >= BoundX || AY >= BoundY || AX < 0 || AY < 0) {
#ifdef BLOCKMAP_YELL
						cout << "\7\x1b[31mBLOCKMAP ERROR!\x1b[0m Out of bounds (" << AX << "," << AY << ")  " << BoundX << "x" << BoundY << endl;
#endif
					} else {
						_BlockMap[AX + (AY * (BoundX + 1))] = true;
					}
					if (KthuraDraw::DrawDriver == NULL) Kthura::Throw("Draw Driver is null!");
					if (KthuraDraw::DrawDriver->HasTexture(O.get()))
						iw = KthuraDraw::DrawDriver->ObjectWidth(O.get());
					else
						iw = 0;
					tiw = ceil((double)iw / GW) - 1;

					for (AX = TX - (tiw); AX <= TX + (tiw); AX++) {
						if (AX >= 0 && AX <= BoundX && TY <= BoundY && TY >= 0) _BlockMap[AX + (AY * (BoundX + 1))] = true; //BlockMap[AX, TY] = true;
					}
#endif
					break;
				case KthuraKind::Pic:
#ifndef  BLOCKMAP_IGNOREPIC
					TX = floor((double)X / GW);
					TY = floor((double)Y / GH);										
					//BlockMap[TX, TY] = true;
					_BlockMap[AX + (AY * (BoundX + 1))] = true;
					if (KthuraDraw::DrawDriver->HasTexture(O.get())) {
						iw = KthuraDraw::DrawDriver->ObjectWidth(O.get()); //ImageWidth(o.textureimage)
						tiw = ceil((double)iw / GW);
						ih = KthuraDraw::DrawDriver->ObjectHeight(O.get()); //ImageHeight(o.textureimage)
						tih = ceil((double)ih / GH);
						for (AX = TX; AX < TX + (tiw); AX++) for (AY = TY; AY < TY + tih; AY++) {
							if (AX >= 0 && AX <= BoundX && AY <= BoundY && AY >= 0) _BlockMap[AX + (AY * (BoundX + 1))] = true;// BlockMap[AX, AY] = true;
						}
					}
#else
//					O->ForcePassible(false);
//					O->Impassible(false);
#endif // ! BLOCKMAP_IGNOREPIC
					break;
				}
			}
		}
		// And this will force a way open if applicable	
		for (auto O : Objects) {
			if (O->ForcePassible()) {
				X = O->X(); if (X < 0) X = 0;
				Y = O->Y(); if (Y < 0) Y = 0;
				W = O->W() - 1; if (W < 0) W = 0;
				H = O->H() - 1; if (H < 0) H = 0;
				switch (O->EKind()) {
				case KthuraKind::TiledArea:
				case KthuraKind::Zone:
				case KthuraKind::StretchedArea:
					//Kthura.EDITTORLOG($"Working on Impassible {O.kind} {O.Tag}");
					TX = floor((double)X / GW);
					TY = floor((double)Y / GH);
					TW = ceil((double)((X + W) / GW));
					TH = ceil((double)((Y + H) / GH));
					//Print "DEBUG: Blockmapping area ("+TX+","+TY+") to ("+TW+","+TH+")"
					for (AX = TX; AX <= TW; AX++) {
						for (AY = TY; AY <= TH; AY++) {
							//for (AX = TX; AX < TW; AX++) {
							//    for (AY = TY; AY < TH; AY++) {
							try {
								//Kthura.EDITTORLOG($"Blocking {AX},{AY}");
								//cout << "FCPASSIBLE: " << O.Kind() << ": " << O.Tag() << " (" << O.X() << "," << O.Y() << ") " << O.W() << "x" << O.H() << " fordata X(" << AX << ":" << TX << ":" << TW << ") Y(" << AY << ":" << TY << ":" << TH << ")\n";
								_BlockMap[AX + (AY * (BoundX + 1))] = false;
							} catch (...) {
								//throw new Exception($"BlockMap[{AX},{AY}]: Out of bounds ({BlockMap.GetLength(0)}x{BlockMap.GetLength(1)})");
								Kthura::Throw("BlockMap[" + to_string(AX) + "," + to_string(AY) + "]: Out of bounds (" + to_string(BlockMapBoundW) + "x" + to_string(BlockMapBoundH) + ")");
							}
						}
					}
					break;
				case KthuraKind::Obstacle:
					#ifndef BLOCKMAP_IGNOREOBSTACLES
					TX = (int)floor((double)(X / GW));
					TY = (int)floor((double)((Y - 1) / GH));
					//BlockMap[TX, TY] = false;
					_BlockMap[AX + (AY * (BoundX + 1))] = false;
					if (KthuraDraw::DrawDriver == NULL) Kthura::Throw("Draw Driver is null!");
					if (KthuraDraw::DrawDriver->HasTexture(O.get()))
						iw = KthuraDraw::DrawDriver->ObjectWidth(O.get());
					else
						iw = 0;
					tiw = ceil((double)iw / GW) - 1;

					for (AX = TX - (tiw); AX <= TX + (tiw); AX++) {
						if (AX >= 0 && AX <= BoundX && TY <= BoundY && TY >= 0) _BlockMap[AX + (AY * (BoundX + 1))] = false; //BlockMap[AX, TY] = true;
					}
#endif
					break;
				case KthuraKind::Pic:
#ifndef  BLOCKMAP_IGNOREPIC
					TX = (int)floor((double)X / GW);
					TY = (int)floor((double)Y / GH);
					//BlockMap[TX, TY] = false;
					_BlockMap[AX + (AY * (BoundX + 1))] = false;
					if (KthuraDraw::DrawDriver->HasTexture(O.get())) {
						iw = KthuraDraw::DrawDriver->ObjectWidth(O.get()); //ImageWidth(o.textureimage)
						tiw = ceil((double)iw / GW);
						ih = KthuraDraw::DrawDriver->ObjectHeight(O.get()); //ImageHeight(o.textureimage)
						tih = ceil((double)ih / GH);
						for (AX = TX; AX < TX + (tiw); AX++) for (AY = TY; AY < TY + tih; AY++) {
							if (AX >= 0 && AX <= BoundX && AY <= BoundY && AY >= 0) _BlockMap[AX + (AY * (BoundX + 1))] = false;// BlockMap[AX, AY] = true;
						}
					}
#endif
					break;
				}
			}
			
		}


	}

	void KthuraLayer::RemapID() {
		ID_Map.clear();
		for (auto& Obj : Objects) {
			ID_Map[Obj->ID()] = Obj.get();
		}
	}

	void KthuraLayer::TotalRemap() {
		parent->_ignorecase_labels = Upper(parent->Options.Value("IgnoreCase", "Labels")) == "YES";
		parent->_ignorecase_tags = Upper(parent->Options.Value("IgnoreCase", "Tags")) == "YES";
		RemapID();
		RemapDominance();
		RemapTags();
		RemapLabels();
		BuildBlockMap();
	}

	void KthuraLayer::NewObject(std::string Kind) {
		Objects.push_back(KthuraObject::Create(Kind, this));
		if (Kthura::AutoMap) TotalRemap();
	}

	KthuraObject* KthuraLayer::RNewObject(std::string Kind) {
		NewObject(Kind);
		TotalRemap();
		return Objects[Objects.size() - 1].get();
	}

	void KthuraLayer::Spawn(std::string spottag, std::string ActorTag) {
		//Actors.push_back(KthuraActor::Spawn(this, spottag));
		Objects.push_back(KthuraObject::Import(new KthuraActor(this, spottag)));
		Objects[Objects.size() - 1]->Tag(ActorTag);
		//Objects[Objects.size() - 1].autokill = true;                        
		if (Kthura::AutoMap) TotalRemap();
		RemapID();
	}

	void KthuraLayer::Spawn(KthuraObject* spot, std::string ActorTag) {
		//cout << "Spawning on: " << ActorTag << "\n";
		//Actors.push_back(KthuraActor::Spawn(this, spot));
		//Actors[Actors.size()-1].Tag(ActorTag);
		Objects.push_back(KthuraObject::Import(new KthuraActor(this, spot)));
		Objects[Objects.size() - 1]->Tag(ActorTag);
		//Objects[Objects.size() - 1].autokill = true;
		if (Kthura::AutoMap) TotalRemap();
		RemapID();
	}

	void KthuraLayer::Spawn(std::string ActorTag, int x, int y, std::string wind, unsigned char R, unsigned char G, unsigned char B, unsigned char alpha, int Dominance) {
		//Actors.push_back(KthuraActor::Spawn(this, x, y, wind, R, G, B, alpha, Dominance));
		//Actors[Actors.size() - 1].Tag(ActorTag);
		Objects.push_back(KthuraObject::Import(new KthuraActor(this, x, y, wind, R, G, B, alpha, Dominance)));
		Objects[Objects.size() - 1]->Tag(ActorTag);
		//Objects[Objects.size() - 1].autokill = true;
		if (Kthura::AutoMap) TotalRemap();
		RemapID();
	}

	void KthuraLayer::Kill(KthuraObject* O) {
		int idx = -1;
		if (!O->CheckParent(this)) { Kthura::Throw("Trying to kill an object that is not part of the requested layer!"); return; }
		//if (O->EKind() == KthuraKind::Actor) {
			//for (int i = 0; i < Actors.size(); ++i) if (&Actors[i] == O) idx = i;
		//} else {
		for (int i = 0; i < Objects.size(); ++i) if (Objects[i].get() == O) idx = i;
		//}
		if (idx == -1) { Kthura::Throw("Object to kill not found!"); return; }
		auto EK = O->EKind();
		O->Kill();
		O = NULL;
		_TagMap.clear();
		_LabelMap.clear();
		ID_Map.clear();
		//if (EK == KthuraKind::Actor) {
//            Actors.erase(Actors.begin() + idx);
  //      } else {
		Objects.erase(Objects.begin() + idx);
		//    }
		if (Kthura::AutoMap) TotalRemap();
	}

	void KthuraLayer::Kill(int ID) {
		if (ID_Map.count(ID)) Kill(ID_Map[ID]);
	}

	void KthuraLayer::Kill(std::string Tag) {
		if (_TagMap.count(Tag)) Kill(_TagMap[Tag]);
	}

	void KthuraLayer::KillAllObjects() {
		for (auto O : Objects) O->Kill();
		_TagMap.clear();
		ID_Map.clear();
		_LabelMap.clear();
		_BlockMap.clear();
		Objects.clear();
		//Actors.clear();
		TotalRemap(); // Just an extra safety pre-caution.
	}

	void KthuraLayer::HideByLabel(std::string label) {
		if (!_LabelMap.count(label)) return;
		auto& lst = _LabelMap[label];
		for (auto O : lst) {
			O->Visible(false);
		}
	}

	void KthuraLayer::ShowByLabel(std::string label) {
		if (!_LabelMap.count(label)) return;
		auto& lst = _LabelMap[label];
#ifdef HideAndShowDebug
		cout << "ShowByLabel:> " << label << "; This should make me show " << lst.size() << " objects! Here goes!\n";
#endif
		for (auto O : lst) {
			O->Visible(true);
		}
	}

	void KthuraLayer::HideButLabel(std::string label) {
		//for (auto& O : Objects) O.Visible(false);
		for (int i = 0; i < Objects.size(); i++) {
			auto O = Objects[i];
			O->Visible(false);
		}
		ShowByLabel(label);
	}

	void KthuraLayer::ShowButLabel(std::string label) {
		//for (auto& O : Objects) O.Visible(true);
		for (int i = 0; i < Objects.size(); i++) {
			auto O = Objects[i];
			O->Visible(true);
		}
		HideByLabel(label);
	}

	void KthuraLayer::RemoveActors() {
		std::vector<KthuraObject*> Dodenlijst;
		for (auto& O : Objects) {
			if (O->EKind() == KthuraKind::Actor) {
				Dodenlijst.push_back(O.get());
			}
		}
		for (auto& Slachtoffer : Dodenlijst) Kill(Slachtoffer);
	}

	std::string KthuraLayer::LabelMapDump() {
		std::string ret = "";
		for (auto tl : _LabelMap) {
			if (ret.size()) ret += "\n";
			ret += "Label: " + tl.first + "; Entries: " + to_string(tl.second.size());
		}
		return ret;
	}

	int KthuraLayer::BlockMapWidth() { return BM_W; }
	int KthuraLayer::BlockMapHeight() { return BM_H; }

	bool KthuraLayer::UsedTex(std::string Tex) {
		Tex = Upper(Tex);
		for (auto o : Objects)
			if (Upper(o->Texture()) == Tex) return true;
		return false;
	}

	void KthuraLayer::Relabel(int x, int y, int w, int h, std::string label, bool overwrite) {
		bool
			anydone{ false },
			automap{ Kthura::AutoMap };
		Kthura::AutoMap = false; // Performance!
		for (auto o : Objects) {
			bool InArea{ false };
			switch (o->EKind()) {
			case KthuraKind::Zone:
			case KthuraKind::StretchedArea:
			case KthuraKind::Rect:
			case KthuraKind::TiledArea:
				InArea = InArea || (o->X()+o->W() >= x && o->Y()+o->H() >= y && o->X() <= x + w && o->Y()+o->H() <= y + h);
				InArea = InArea || (o->X() >= x || o->Y() >= y) && (o->X() <= x + w || o->Y() <= y + h);
				// fallthrough
			case KthuraKind::Actor:
			case KthuraKind::Obstacle:
			case KthuraKind::Pivot:
			case KthuraKind::Exit:
			case KthuraKind::Custom:
			case KthuraKind::CustomItem:
				InArea = InArea || (o->X() >= x && o->Y() >= y && o->X() <= x + w && o->Y() <= y + h);
				break;
			default:
				Kthura::Panic("Relabel: Unknown kind code for object #" + std::to_string(o->ID()) + " (" + std::to_string((int)o->EKind()));
				return;
			}
			if (InArea) {
				anydone = true;
				if (overwrite)
					o->Labels( label);
				else {
					auto l{ o->Labels() };
					if (l.size()) l += ",";
					l += label;
					o->Labels(l);
				}
			}
			if (automap && anydone) RemapLabels();
			Kthura::AutoMap = automap;
		}
	}

	



	


	bool Kthura::AutoMap = true;
	bool Kthura::AllowUnknown = true;
	void (*Kthura::Panic) (std::string err) = NULL;
	std::map<std::string, std::vector<char>>* Kthura::UnknownData() { return &Unknown; }
	KthuraLayer* Kthura::Layer(std::string lay) {
		lay = Upper(lay);
		if (!Layers.count(lay)) { Throw("No layer called \"" + lay + "\" found!"); return NULL; }
		return Layers[lay].get();
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
		Layers[lay] = std::make_shared<KthuraLayer>();
		Layers[lay]->SetParent(this,lay);
		Layers[lay]->TotalRemap();
	}

	void Kthura::RenameLayer(std::string vieux, std::string nouveau) {
		vieux = Upper(vieux);
		nouveau = Upper(nouveau);
		if (vieux == nouveau) return;
		if (Layers.count(nouveau)) {
			Throw("New layer '" + nouveau + "' already exists");
			return;
		}
		Layers[nouveau] = Layers[vieux];
		Layers.erase(vieux);
	}
	void Kthura::KillLayer(std::string lay) {
		if (!Layers.count(TrickyUnits::Upper(lay))) return;
		Layers[TrickyUnits::Upper(lay)]->KillAllObjects();
		Layers.erase(TrickyUnits::Upper(lay));
	}

	void Kthura::KillAllLayers() {
		for (auto& l : Layers) {
			l.second->KillAllObjects();
		}
		Layers.clear();
	}

	void Kthura::KillMap() {
		KillAllLayers();
		MetaData.clear();
	}

	bool Kthura::AutoAttachJCRForTex{ false };
	bool Kthura::StrictLoad = true;
	void Kthura::Load(jcr6::JT_Dir& sourcedir, std::string Prefix) {        
#ifdef DEFAULT_CASESENSENSITIVE_LOAD
		Options.Value("IgnoreCase", "Tags", "NO");
		Options.Value("IgnoreCase", "Labels", "NO");
#endif
		if (AutoAttachJCRForTex) this->TexDir = &sourcedir;
		if (!sourcedir.EntryExists(Prefix + "Objects")) { Throw(Prefix + "Objects has not been found in JCR6 resource!"); return; }
		if (!sourcedir.EntryExists(Prefix + "Data")) { Throw(Prefix + "Data has not been found in JCR6 resource!"); return; }
		if (sourcedir.EntryExists(Prefix + "Options")) {
			auto optionstr{ sourcedir.String(Prefix + "Options") };
			Options.Parse(optionstr);
		}
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
		// Data
		auto m = sourcedir.StringMap(Prefix + "Data");
		for (auto K : m)
			MetaData[K.first] = K.second;
		// Objects
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
				else if (l == "AUTORETAG")
					_autoretag = true;
				else if (readlayers) {
					this->NewLayer(Upper(l)); // Better?
					//Layers[Upper(l)].GridX += 0; // Just forces to create the layer... Had to do something!
					//Layers[Upper(l)].SetParent(this, Upper(l));
#ifdef Kthura_LoadChat
					cout << "Created layer: " << l << " >> " << Upper(l) << endl;
#endif
				} else if (l == "NEW") {
					//obj = new KthuraRegObject("?", ret.Layers[curlayername]);
					Layers[Upper(curlayername)]->NewObject("??");
					obj = Layers[Upper(curlayername)]->LastObject();
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
						curlayername = Upper(value);
						if (!Layers.count(curlayername)) Throw("Try to load data to a non-existent layer: \"" + curlayername + "\"");
						curlayer = Layers[value].get();
						curlayer->SetParent(this,curlayername);
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
						obj->insertx ( stoi(Trim(s[0])));// *(-1);
						obj->inserty ( stoi(Trim(s[1])));// *(-1);
					}//break;
					kthload_case("ROTATION") {
						kthload_assert(obj, "ROTATION: No object");
						obj->RotationDegrees(stoi(value));
					}//break;
					kthload_case("ROTATION_RAD") {
						kthload_assert(obj, "ROTATION: No object");
						obj->RotationRadians(stod(value));
						// For Kthura Degrees and not Radians will always remain the standard, though!
					}//break;
					kthload_case("SIZE") {
						kthload_assert(obj, "SIZE: No Object");
						s = Split(value, 'x');
						kthload_assert(s.size() == 2, "SIZE syntax error!");
						obj->W(stoi(Trim(s[0])));
						obj->H(stoi(Trim(s[1])));
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
						obj->Texture ( value);
					}//    break;
					kthload_case("CURRENTFRAME") {
						kthload_assert(obj, "CURRENT FRAME: No object");
						obj->AnimFrame ( stoi(value));
						//break;
					}
					kthload_case("FRAMESPEED") {
						kthload_assert(obj, "FRAME SPEED: No object");
						obj->AnimSpeed ( stoi(value));
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
						obj->Visible ( stoi(value) == 1);
					}//break;
					kthload_case("COLOR") {
						kthload_assert(obj, "COLOR: No Object");
						s = Split(value, ',');
						kthload_assert(s.size() == 3, "COLOR syntax error!");
						obj->R ( stoi(Trim(s[0])));
						obj->G ( stoi(Trim(s[1])));
						obj->B ( stoi(Trim(s[2])));
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
						obj->ScaleX ( stoi(Trim(s[0])));
						obj->ScaleY ( stoi(Trim(s[1])));
					} // break;
					kthload_case("BLEND") {
						auto v = stoi(value);
						//if (stoi(value) != 0) {
							//Throw("Alternate Blends are only supported in the BlitzMax version of Kthura!"); return;
							// Although when rading some documentation, it may be possible to support this in C++
						//}
						obj->Blend(v);
					} // break;
					else {
						if (prefixed(key, "DATA.")) {
							auto _Key = Trim(key.substr(5));
							kthload_assert(obj, key+": No object");
							//cout << "Set Data '" << _Key << "' => '" << Trim(value) << endl;
							obj->MetaData(_Key, Trim(value));
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
		for (auto ilay : Layers) { ilay.second->TotalRemap(); }
		// Unknwon if allowed
		if (AllowUnknown) {
			Unknown.clear();
			for (auto ku : sourcedir.Entries()) {
				auto UP = Upper(Prefix);
				if (prefixed(ku.first, UP) && ku.first != UP + "OBJECTS" && ku.first != UP + "SETTINGS" && ku.first != UP + "DATA") {
					Unknown[right(ku.second.Entry(), ku.second.Entry().size() - UP.size())] = sourcedir.Characters(ku.first);
				}
			}
		}
		//return ret;
	}
	

	void Kthura::Load(std::string JCR, std::string Prefix) {
		auto J = jcr6::Dir(JCR);
		if (JCR_Error != "" && JCR_Error!="Ok") { Throw("JCR6 file \"" + JCR + "\" could notbe loaded.\nJCR6 reported: " + JCR_Error); return; }
		Load(J, Prefix);
	}

	void Kthura::Remap() {
		_ignorecase_labels = Upper(Options.Value("IgnoreCase", "Labels")) == "YES";
		_ignorecase_tags = Upper(Options.Value("IgnoreCase", "Tags")) == "YES";
		for (auto ti : Layers) {
			Layer(ti.first)->TotalRemap();
		}
	}

	void Kthura::RemoveActors() {
		for (auto& Layer : Layers) Layer.second->RemoveActors();
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

	bool Kthura::UsedTex(std::string Tex) {
		for (auto L : Layers)
			if (L.second->UsedTex(Tex)) return true;
		return false;
	}

	Kthura::Kthura() {
		_id = countup++;
		Options.Value("IgnoreCase", "Tags", "YES");
		Options.Value("IgnoreCase", "Labels", "YES");
	}

	Kthura::~Kthura() {
		KillMap();
	}

	int Kthura::countup = 0;

	KthuraPathFinder* Kthura::PathFinder = NULL;

	

	KthuraPoint::KthuraPoint() {    }

	KthuraPoint::KthuraPoint(KthuraLayer* parent) { this->parent = parent; }

	void KthuraPoint::GX(int v) {
		_grix = v;
		_exax = floor((v * parent->GridX) + (parent->GridX / 2));
	}

	void KthuraPoint::GY(int v) {
		if (!parent) { Kthura::Throw("Kthura point without parent!"); return; }
		_griy = v;
		_exay = floor((v * parent->GridY) + (parent->GridY));
	}

	void KthuraPoint::XX(int v) {
		if (!parent) { Kthura::Throw("Kthura point without parent!"); return; }
		_exax = v;
		_grix = floor(v / parent->GridX);
	}

	void KthuraPoint::XY(int v) {
		_exay = v;
		_griy = floor(v / parent->GridY);
	}

	int KthuraPoint::GX() {
		return _grix;
	}

	int KthuraPoint::GY() {
		return _griy;
	}
	int KthuraPoint::XX() {
		return _exax;
	}

	int KthuraPoint::XY() {
		return _exay;
	}

	void KthuraActor::Walk2Move() {
		if (Walking) {
			MoveX = FoundPath[PathIndex].XX();
			MoveY = FoundPath[PathIndex].XY()-1;
			Moving = true;
		}
	}

	void KthuraActor::InMotion(bool value) { _InMotion = value; }

	bool KthuraActor::InMotion() {
		if (WalkingIsInMotion)
			return Walking || Moving;
		else
			return _InMotion;
	}

	int KthuraActor::PathLength() { return FoundPath.size(); }

	int KthuraActor::CWalkX() {
		if (Walking) return FoundPath[PathIndex].GX();
		return 0;
	}
	int KthuraActor::CWalkY() {
		if (Walking) return FoundPath[PathIndex].GY();
		return 0;
	}

	int KthuraActor::ID() {
		return _id;
	}

	void KthuraActor::WalkTo(int to_x, int to_y, bool real) {
		//cout << "KthuraActor." << GetParent()->GetCreationName() <<"::WalkTo(" << to_x << "," << to_y << "," << real << ")!\n";
		//cout << "Start point: (" << O.X() << "," << O.Y() << ")\n";
		auto gridx = GetParent()->GridX;
		auto gridy = GetParent()->GridY;
		int tox = to_x, toy = to_y;
		int fromx = O.X(), fromy = O.Y();
		if (real) {
			tox = to_x / gridx;
			toy = to_y / gridy;
			fromx = O.X() / gridx;
			fromy = O.Y() / gridy;
		}
		//cout << "Calc (if applicable): >> (" << fromx << "," << fromy << ") -> (" << tox << "," << toy << ")    grid: " << gridx << "x" << gridy << "\n";
		// Old C# code but not usable in this C++ version: FoundPath = Dijkstra.QuickPath(Parent.PureBlockRev, Parent.BlockMapWidth, Parent.BlockMapHeight, fromx, fromy, tox, toy);
		//FoundPath = Kthura::PathFinder->FindPath(parentobj, tox, toy);
		FoundPath = Kthura::PathFinder->FindPath(fromx, fromy, GetParent(), tox, toy);
		if (Kthura::PathFinder->Success) { //if (FoundPath.Success) {
			PathIndex = 0;
			Walking = true;
			WalkingToX = to_x; //FoundPath.Nodes[0].x;
			WalkingToY = to_y; //FoundPath.Nodes[1].y;
			MoveX = O.X();
			MoveY = O.Y();
			Walk2Move();
		} else {
			Walking = false;
			//FoundPath = null;
		}
	}

	void KthuraActor::WalkTo(KthuraObject* obj) { WalkTo(obj->X(), obj->Y()); }
	void KthuraActor::WalkTo(std::string Tag) { WalkTo(GetParent()->TagMap(Tag)); }

	void KthuraActor::MoveTo(int x, int y) {
		MoveX = x;
		MoveY = y;
		Moving = true;
	}

	void KthuraActor::MoveTo(KthuraObject* obj) { MoveTo(obj->X(), obj->Y()); }
	void KthuraActor::MoveTo(std::string Tag) { MoveTo(GetParent()->TagMap(Tag)); }

	KthuraActor::KthuraActor (KthuraLayer* parent, string spot) {
		auto ret = this;
		//KthuraActor ret(parent);
		//{ KthuraActor created(parent); parent->Objects.push_back(created); ret = &created; }
		auto obj = parent->TagMap(spot);
		ret->parent = parent;        
		ret->O.PSync(this);
		ret->_id = parent->nextID();
		ret->O.X(obj->X());
		ret->O.Y(obj->Y());
		ret->O.Dominance ( obj->Dominance());
		ret->O.Alpha255(255);
		ret->O.R = 255;
		ret->O.G = 255;
		ret->O.B = 255;
		ret->O.Visible = true;
		ret->O.Impassible ( false);
		ret->O.Labels(obj->Labels());
		if (obj->MetaDataCount("Wind")) ret->Wind = obj->MetaData("Wind"); else ret->Wind = "NORTH";
		//return ret;        
	}

	KthuraActor::KthuraActor(KthuraLayer* parent, KthuraObject* obj) {
		//KthuraActor* ret;
		//KthuraActor created(parent); /*parent->Objects.push_back(created);*/ ret = &created; 
		auto ret = this;
		ret->parent = parent;
		ret->O.PSync(this);
		ret->_id = parent->nextID();
		ret->O.X(obj->X());
		ret->O.Y(obj->Y());
		ret->O.Dominance(obj->Dominance());
		ret->O.Alpha255(255);
		ret->O.R = 255;
		ret->O.G = 255;
		ret->O.B = 255;
		ret->O.Visible = true;
		ret->O.Impassible(false);
		ret->O.Labels(obj->Labels());
		if (obj->MetaDataCount("Wind")) ret->Wind = obj->MetaData("Wind"); else ret->Wind = "NORTH";
		//return created;
	}

	KthuraActor::KthuraActor(KthuraLayer* parent, int x, int y, std::string wind, unsigned char R, unsigned char G, unsigned char B, unsigned char alpha, int Dominance) {
		auto ret = this;
		//KthuraActor* ret;
		//KthuraActor created(parent); /*parent->Objects.push_back(created);*/ ret = &created;
		//var ret = new KthuraActor(parent);
		ret->parent = parent;
		ret->O.PSync(this);
		ret->_id = parent->nextID();
		ret->O.X(x);
		ret->O.Y(y);
		ret->Wind = wind;
		ret->O.R = R;
		ret->O.G = G;
		ret->O.B = B;
		ret->O.Alpha255(alpha);
		ret->O.Dominance(Dominance);
		ret->O.Visible = true;
		ret->O.Impassible(false);
		//return *ret;
	}

	void KthuraActor::UpdateMoves() {
		if (Moving || Walking) {
			if (MoveX < O.X()) { O.Xm(MoveSkip); if (O.X() < MoveX) O.X(MoveX); if (AutoWind) Wind = "WEST"; }
			if (MoveX > O.X()) { O.Xp(MoveSkip); if (O.X() > MoveX) O.X(MoveX); if (AutoWind) Wind = "EAST"; }
			if (MoveY < O.Y()) { O.Ym(MoveSkip); if (O.Y() < MoveY) O.Y(MoveY); if (AutoWind) Wind = "NORTH"; }
			if (MoveY > O.Y()) { O.Yp(MoveSkip); if (O.Y() > MoveY) O.Y(MoveY); if (AutoWind) Wind = "SOUTH"; }
			if (MoveX == O.X() && MoveY == O.Y()) {
				if (!Walking)
					Moving = false;
				else {
					PathIndex++;
					if (PathIndex >= PathLength()) {
						Walking = false;
						Moving = false;
					} else {
						Walk2Move();
					}
				}
			}
		} else {
			MoveX = O.X();
			MoveY = O.Y(); 
		}
		if ((WalkingIsInMotion && Walking) || InMotion()) {
			FrameSpeedCount++;
			if (FrameSpeedCount >= FrameSpeed) {
				FrameSpeedCount = 0;
				O.AnimFrame++;
			}
		} else if (WalkingIsInMotion && (!Walking) && (NotInMotionThen0)) O.AnimFrame = 0;
	}

	std::string KthuraActor::Kind() { return "Actor"; }
	KthuraKind KthuraActor::EKind() { return KthuraKind::Actor; }


	KthuraActor::KthuraActor(KthuraLayer* argparent){
		
		parent = argparent;
		_Kind = "Actor";        
	}

	KthuraLayer* KthuraActor::GetParent() {
		return parent;
	}

	KthuraObject* KthuraActor::GetObject() { return parentobj; }

	void KthuraActor::SetObject(KthuraObject* o) { parentobj = o; }

	

		/*
	KthuraObject::~KthuraObject() {
		cout << "Destructor KthuraObject Called: " << Kind() << "; " << ID() << "\n";
		if (!autokill) return;
		if (O) delete O;
		if (A) delete A;
		autokill = false;
	}
		*/

	void KthuraObject::Kill() {
		// cout << "Object kill! " << Kind() << "; " << Tag() << "; " << ID() << ";\n";
		if (O) delete O;
		if (A) delete A;
		O = NULL;
		A = NULL;
	}

	KthuraObject::KthuraObject(std::string aKind, KthuraLayer* prnt) {
		if (aKind == "Actor") { A = new KthuraActor(prnt); }
		else { O = new KthuraRegObject(prnt,aKind); }
	}

	KthuraObject::KthuraObject(KthuraActor* act) {
		A = act;
	}

	KthuraObject::KthuraObject(KthuraRegObject* obj) {
		O = obj;
	}

	KthuraLayer* KthuraObject::GetParent() {
		if (A) return A->GetParent(); else return O->GetParent();        
	}

	int KthuraObject::ID() { if (A) return A->ID(); return _id; }
	std::string KthuraObject::MetaData(std::string key) { kthobjret(MetaData[Upper(key)]); }
	int KthuraObject::MetaDataCount(std::string key) { kthobjret(MetaData.count(Upper(key))); }

	std::vector<std::string> KthuraObject::MetaDataFields() {
		vector<std::string> ret;
		if (A) {
			for (auto it : A->O.MetaData) ret.push_back(it.first);
		} else {
			for (auto it : O->MetaData) ret.push_back(it.first);
		}
		return ret;
	}

	void KthuraObject::MetaData(std::string key, std::string value) {
		if (A) A->O.MetaData[key] = value; else O->MetaData[Upper(key)]=value;
	}

	std::string KthuraObject::Texture() { kthobjret(Texture); }
	bool KthuraObject::Visible() { 
		kthobjret(Visible); 
	}
	bool KthuraObject::XVisible(int cx, int cy) {
		if (Kthura::AutoVisible.active) {
			switch (EKind()) {
			case KthuraKind::TiledArea:
			case KthuraKind::StretchedArea:
			case KthuraKind::Zone:
				if (
					-cx + X() + W() < Kthura::AutoVisible.bx ||
					-cx + X() > Kthura::AutoVisible.ex ||
					-cy + Y() + H() < Kthura::AutoVisible.by ||
					-cy + Y() > Kthura::AutoVisible.ey)
					return false;
				break;
			case KthuraKind::Actor:
			case KthuraKind::Obstacle:
				if (
					// No division on W(). This is a safety precaution to have a bit of a margin when it comes to scaling
					-cx + X() + W() < Kthura::AutoVisible.bx ||
					-cx + X() - W() > Kthura::AutoVisible.ex ||
					-cy + Y() < Kthura::AutoVisible.by ||
					-cy + Y() - H() > Kthura::AutoVisible.ey)
					return false;
				break;
			}
		}
		return Visible();
	}
	int KthuraObject::W() { kthobjret(w); }
	int KthuraObject::H() { kthobjret(h); }
	int KthuraObject::insertx() { kthobjret(insertx); }
	int KthuraObject::inserty() { kthobjret(inserty); }
	int KthuraObject::R() { kthobjret(R);}
	int KthuraObject::G() { kthobjret(G); }
	int KthuraObject::B() { kthobjret(B); }
	int KthuraObject::ScaleX() { kthobjret(ScaleX); }
	int KthuraObject::ScaleY() { kthobjret(ScaleY); }
	int KthuraObject::AnimSpeed() { kthobjret(AnimSpeed); }
	int KthuraObject::AnimFrame() { kthobjret(AnimFrame); }
	int KthuraObject::Alpha255() { kthobjretf(Alpha255); }
	int KthuraObject::Alpha1000() { kthobjretf(Alpha1000); }
	float KthuraObject::TrueScaleX() { kthobjret(ScaleX / (float)1000); }
	float KthuraObject::TrueScaleY() { kthobjret(ScaleY / (float)1000); }
	int KthuraObject::Blend() { kthobjret(Blend); }
	std::string KthuraObject::Kind() { if (A) return "Actor"; return O->Kind(); }
	KthuraKind KthuraObject::EKind() { if (A) return KthuraKind::Actor; return O->EKind(); }
	int KthuraObject::X() { kthobjretf(X); }
	int KthuraObject::Y() { kthobjretf(Y); }
	std::string KthuraObject::Tag() { kthobjretf(Tag); }
	int KthuraObject::Dominance() { kthobjretf(Dominance); }
	std::string KthuraObject::Labels() { kthobjretf(Labels); }
	bool KthuraObject::ForcePassible() { kthobjretf(ForcePassible); }
	bool KthuraObject::Impassible() { kthobjretf(Impassible); }
	int KthuraObject::RotationDegrees() { kthobjretf(RotationDegrees); }
	double KthuraObject::RotationRadians() { kthobjretf(RotationRadians); }
	void KthuraObject::Texture(std::string value) {  std::string OTex{ Texture() }; kthobjdef(Texture); if (Upper(OTex) != Upper(value)) {AnimFrame(0); } }
	void KthuraObject::Visible(bool value) { kthobjdef(Visible); }
	void KthuraObject::W(int value) { kthobjdef(w); }
	void KthuraObject::H(int value) { kthobjdef(h); }
	void KthuraObject::insertx(int value) { kthobjdef(insertx); }
	void KthuraObject::inserty(int value) { kthobjdef(inserty); }
	void KthuraObject::ScaleX(int value) { kthobjdef(ScaleX); }
	void KthuraObject::ScaleY(int value) { kthobjdef(ScaleY); }
	void KthuraObject::AnimSpeed(int value) { kthobjdef(AnimSpeed); }
	void KthuraObject::AnimFrame(int value) { kthobjdef(AnimFrame); }
	void KthuraObject::X(int value) { if (A) A->O.X(value); else O->X(value); }
	void KthuraObject::Y(int value) { if (A) A->O.Y(value); else O->Y(value); }
	void KthuraObject::Tag(std::string value) { kthobjset(Tag); }
	void KthuraObject::TagNoRemap(std::string value) { kthobjset(TagNoRemap);  }
	void KthuraObject::Dominance(int value) { kthobjset(Dominance); }
	void KthuraObject::Labels(std::string value) { kthobjset(Labels); }
	void KthuraObject::ForcePassible(bool value) { kthobjset(ForcePassible); }
	void KthuraObject::Impassible(bool value) { kthobjset(Impassible); }
	void KthuraObject::RotationRadians(double value) { kthobjset(RotationRadians); }
	void KthuraObject::Alpha255(int value) { kthobjset(Alpha255); }
	void KthuraObject::Alpha1000(int value) { kthobjset(Alpha1000); }
	void KthuraObject::R(int value) { kthobjdef(R); }
	void KthuraObject::G(int value) { kthobjdef(G); }
	void KthuraObject::B(int value) { kthobjdef(B); }
	void KthuraObject::Blend(int value) { kthobjdef(Blend); }
	void KthuraObject::RotationDegrees(int value) { kthobjset(RotationDegrees); }
	
	KthuraAutoVisibleRect Kthura::AutoVisible{ 0, 0, 0, 0, false };

}