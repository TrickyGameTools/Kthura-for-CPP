// Lic:
// src/Kthura_Core.hpp
// Kthura - Core (header)
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
//#ifndef KTHURA_LOADED_CORE
//#define KTHURA_LOADED_CORE
#pragma once

#include <string>
#include <map>
#include <vector>

namespace NSKthura{
	
	class Kthura;
	class KthuraLayer;
	class KthuraObject;
	
	class KthuraObject {
	private:
		KthuraLayer* parent;
		int cnt = 0;
		int _x = 0, _y = 0;
		int AnimFrameSkip = 0;
		int _Dominance = 20;
		std::string _Labels = "";
		std::string _Tag = "";
		bool _impassible = false;
		bool _forcepassible = false;
		std::string _Kind;
		double _rotrad = 0;
		int _rotdeg = 0;
	public:
		std::map<std::string, std::string> MetaData;
		std::string Texture = "";
		bool Visible = true;
		int w = 0, h = 0;
		int insertx = 0, inserty = 0;
		int R = 255, G = 255, B = 255;
		int ScaleX = 1000, ScaleY = 1000;
		int AnimSpeed = 0;
		int AnimFrame = 0;
		float TrueScaleX();
		float TrueScaleY();
		std::string Kind();
		void X(int newx);
		int X();
		void Y(int newy);
		int Y();
		void Tag(std::string newtag);
		std::string Tag();
		void Dominance(int newdom);
		int Dominance();
		void Labels(std::string nl);
		std::string Labels();
		void Impassible(bool imp);
		bool Imapassible();
		void RotationRadians(double value);
		double RotationRadians();
		void RotationDegrees(int value);
		int RotationDegrees(); 
		static 	KthuraObject Create(std::string Kind,KthuraLayer* p);
	};
	
	class KthuraLayer{
	private:
		Kthura* parent;
		std::map<std::string, KthuraObject*> _TagMap;
		std::map<std::string, std::vector<KthuraObject*>> _LayerMap;
		std::vector<bool> _BlockMap;
		int BM_W = 0;
		int BM_H = 0;
	public:
		int GridX{ 32 };
		int GridY{ 32 };
		std::vector<KthuraObject> Objects;

		KthuraObject* TagMap(std::string Tag);
		std::map<std::string, KthuraObject*> _DomMap; // not to be documented!
		std::vector<KthuraObject*>* LayerMap(std::string label);
		
		bool Blocked(int x, int y);
		bool BlockedPix(int x, int y);
		void RemapDominance();
		void RemapTags();
		void RemapLabels();
		void BuildBlockMap();
		void TotalRemap();
		void NewObject(std::string Kind);
	};
	
	class Kthura {
	private:
	public:
		std::map<std::string, KthuraLayer> Layers;
		std::map<std::string, std::string> MetaData;
		static void Throw(std::string err);
		/// <summary>
		/// Should Kthura have an error, this function will be executed *if* it's been defined. If it's not defined it will merely put a message on the console. 
		/// </summary>
		static void (*Panic)(std::string msg);
		static bool AutoMap;
	};
	
}

//#endif