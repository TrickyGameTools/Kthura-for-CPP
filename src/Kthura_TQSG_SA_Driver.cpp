#include "Kthura_TQSG_SA_Driver.hpp"
#include <TQSG.hpp>
#include <QuickString.hpp>
#include <algorithm>

#define Kthura_SA_DEBUG


#ifdef Kthura_SA_DEBUG
#define KSADChat(arg) std::cout << "DEBUG KTHURA SA> " << arg << "\n"
#else
#define KSADChat(arg)
#endif

using namespace TrickyUnits;

namespace NSKthura {

	static TQSG_ASScreen KthuraScreen = nullptr;
	typedef std::map<std::string, TQSG_AutoImage> TD_TexMap;
	std::map<std::string, TD_TexMap> TexMap;

	
	static TQSG_AutoImage GetTex(KthuraObject* o) {
		static int ID = -1;
		static auto lay = o->GetParent();
		static auto map = lay->GetParent();
		static auto tex = Trim(o->Texture());
		if (ID != map->ID()) TexMap.clear();
		if (tex == "") return nullptr;
		if (TexMap[o->Kind()].count(o->Texture())) return TexMap[o->Kind()][tex];
		KSADChat("Loading texture for " << o->Kind() << ": " << o->Texture());
		if (!(map->TexDir->EntryExists(tex) || map->TexDir->DirectoryExists(tex))) {
			Kthura::Throw("Texture \"" + tex + "\" not found!");
			return nullptr;
		}
		TexMap[o->Kind()][tex] = TQSG_LoadAutoImage(map->TexDir, tex);
		switch (o->EKind()) {
		case KthuraKind::Obstacle:
		case KthuraKind::Actor:
			TexMap[o->Kind()][tex]->HotBottomCenter();
			break;
		}
		return TexMap[o->Kind()][tex];
	}

	static void AS_AnimReset(KthuraObject* O) {
		Kthura::Throw("AnimReset not yet defined for this driver");
	}

	static inline void QuickDrawSettings(KthuraObject* obj,bool plus=false) {
		TQSG_SetBlend(TQSG_Blend::ALPHA);
		TQSG_SetAlpha(obj->Alpha255());
		TQSG_Color(obj->R(), obj->G(), obj->B());
		if (plus) {
			TQSG_Rotate(obj->RotationDegrees());
			SetScale((double)obj->ScaleX() / 1000, (double)obj->ScaleY() / 1000);
		}
	}

	void Kthura_Draw_TQSG_SA_Driver::Init(unsigned int w, unsigned h) {
		TQSG_CreateAS(w, h);
		if (KthuraDraw::DrawDriver) delete KthuraDraw::DrawDriver;
		Kthura_Draw_TQSG_SA_Driver* Me = new Kthura_Draw_TQSG_SA_Driver();
		KthuraDraw::DrawDriver = Me;
		Me->AnimReset = AS_AnimReset;
	}

	void Kthura_Draw_TQSG_SA_Driver::DrawTiledArea(KthuraObject* obj, int ix, int iy, int scrollx, int scrolly) {
		auto Tex = GetTex(obj);
		QuickDrawSettings(obj);
		KthuraScreen->Tile(Tex,obj->X() + ix - scrollx, obj->Y() + iy - scrolly, obj->W(), obj->H(), -obj->insertx(), -obj->inserty(), obj->AnimFrame());
	}

	void Kthura_Draw_TQSG_SA_Driver::DrawRect(KthuraObject* obj, int ix, int iy, int scrollx, int scrolly) {
		QuickDrawSettings(obj);
		KthuraScreen->Rect(obj->X() + ix - scrollx, obj->Y() + iy - scrolly, obj->W(), obj->H());
	}

	void Kthura_Draw_TQSG_SA_Driver::DrawObstacle(KthuraObject* obj, int ix, int iy, int scrollx, int scrolly) {
		auto Tex = GetTex(obj);
		QuickDrawSettings(obj,true);
		KthuraScreen->Draw(Tex, obj->X() + ix - scrollx, obj->Y() + iy - scrolly);
	}

	void Kthura_Draw_TQSG_SA_Driver::DrawActor(KthuraObject* obj, int ix, int iy, int scrollx, int scrolly) {
		auto Tex = GetTex(obj);
		QuickDrawSettings(obj, true);
		if (Tex) {
			//cout << obj->Walking() << obj->Moving() << "???\n";
			obj->UpdateMoves();
			QuickDrawSettings(obj);
			if (obj->AnimFrame() >= Tex->Frames()) obj->AnimFrame(0);
			KthuraScreen->XDraw(Tex,obj->X() + ix - scrollx, obj->Y() + iy - scrolly, obj->AnimFrame());
			SetScale(1, 1);
			TQSG_RotateRAD(0);
			TQSG_SetAlpha(255);
		} else {
			//CrashOnNoTex ? .Invoke($"Actor-texture '{obj.Texture}' did somehow not load?");
			Kthura::Throw("Actor-texture '" + obj->Texture() + "' did somehow not load?");
		}
	}

	void Kthura_Draw_TQSG_SA_Driver::DrawPic(KthuraObject* obj, int ix, int iy, int scrollx, int scrolly) {
		auto Tex = GetTex(obj);
		QuickDrawSettings(obj, true);
		KthuraScreen->Draw(Tex, obj->X() + ix - scrollx, obj->Y() + iy - scrolly);
	}

	void Kthura_Draw_TQSG_SA_Driver::DrawStretchedArea(KthuraObject* obj, int ix, int iy, int scrollx, int scrolly) {
		auto Tex = GetTex(obj);
		QuickDrawSettings(obj);
		KthuraScreen->Stretch(Tex, obj->X() + ix - scrollx, obj->Y() + iy - scrolly, obj->W(), obj->H(), obj->AnimFrame());
	}

	int Kthura_Draw_TQSG_SA_Driver::ObjectWidth(KthuraObject* obj) {
		switch (obj->EKind()) {
		case KthuraKind::TiledArea:
		case KthuraKind::StretchedArea:
		case KthuraKind::Zone:
			return obj->W();
		case KthuraKind::Obstacle:
#ifdef Kthura_TEST_OB
			return 50;
#else
			//Kthura::Throw("I cannot yet see the width of an obstacle");
			return GetTex(obj)->W();
#endif
		case KthuraKind::Pivot:
		case KthuraKind::CustomItem:
			return 0;
		case KthuraKind::Actor:
			//Kthura::Throw("I cannot yet see the width of an actor");
			return GetTex(obj)->W();
		case KthuraKind::Pic:
			return GetTex(obj)->W();
		default:
			Kthura::Throw("Unknown object kind " + obj->Kind());
		}
		return 0;
		return 0;
	}

	int Kthura_Draw_TQSG_SA_Driver::ObjectHeight(KthuraObject* obj) {
		switch (obj->EKind()) {
		case KthuraKind::TiledArea:
		case KthuraKind::StretchedArea:
		case KthuraKind::Zone:
			return obj->H();
		case KthuraKind::Obstacle:
#ifdef Kthura_TEST_OB
			return 50;
#else
			//Kthura::Throw("I cannot yet see the width of an obstacle");
			return GetTex(obj)->H();

#endif
		case KthuraKind::Pivot:
		case KthuraKind::CustomItem:
			return 0;
		case KthuraKind::Actor:
		case KthuraKind::Pic:
			//Kthura::Throw("I cannot yet see the width of an actor");
			return GetTex(obj)->H();
		default:
			Kthura::Throw("Unknown object kind " + obj->Kind());
		}		
		return 0;
	}

	int Kthura_Draw_TQSG_SA_Driver::ObjectWidth(std::shared_ptr<KthuraObject> obj) {
		return ObjectWidth(obj.get());
	}

	int Kthura_Draw_TQSG_SA_Driver::ObjectHeight(std::shared_ptr<KthuraObject> obj) {
		return ObjectHeight(obj.get());
	}

	bool Kthura_Draw_TQSG_SA_Driver::HasTexture(KthuraObject* obj) {
		// TODO: Make this happen!
		return false;
	}

	void DrawRect(KthuraObject* obj, int ix = 0, int iy = 0, int scrollx = 0, int scrolly = 0) {
		QuickDrawSettings(obj);
		KthuraScreen->Rect(obj->X() + ix - scrollx, obj->Y() + iy - scrolly, obj->W(), obj->H());
	}

}