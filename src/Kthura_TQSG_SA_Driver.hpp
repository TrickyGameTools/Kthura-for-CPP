#pragma once
#include "Kthura_Core.hpp"
#include "Kthura_Draw.hpp"

namespace NSKthura {


	class Kthura_Draw_TQSG_SA_Driver;
	class Kthura_Draw_TQSG_SA_Driver :public KthuraDraw {
	public:
		static void Init(unsigned int w, unsigned h);
		void DrawTiledArea(KthuraObject* obj, int ix = 0, int iy = 0, int scrollx = 0, int scrolly = 0);
		void DrawRect(KthuraObject* obj, int ix = 0, int iy = 0, int scrollx = 0, int scrolly = 0);
		void DrawObstacle(KthuraObject* obj, int ix = 0, int iy = 0, int scrollx = 0, int scrolly = 0);
		void DrawActor(KthuraObject* obj, int ix = 0, int iy = 0, int scrollx = 0, int scrolly = 0);
		void DrawPic(KthuraObject* obj, int ix = 0, int iy = 0, int scrollx = 0, int scrolly = 0);
		void DrawStretchedArea(KthuraObject* obj, int ix = 0, int iy = 0, int scrollx = 0, int scrolly = 0);
		int ObjectWidth(KthuraObject* obj);
		int ObjectHeight(KthuraObject* obj);
		int ObjectWidth(std::shared_ptr<KthuraObject> obj);
		int ObjectHeight(std::shared_ptr<KthuraObject> obj);
		bool HasTexture(KthuraObject* obj);
	};

}
