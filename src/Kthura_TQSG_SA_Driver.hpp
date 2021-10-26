// Lic:
// src/Kthura_TQSG_SA_Driver.hpp
// SA Driver (header)
// version: 21.10.11
// Copyright (C) 2021 Jeroen P. Broks
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

		static int Width();
		static int Height();
		static bool Used();
	};

}