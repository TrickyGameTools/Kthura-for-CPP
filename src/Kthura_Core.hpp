// Lic:
// src/Kthura_Core.hpp
// Kthura - Core (header)
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
//#ifndef KTHURA_LOADED_CORE
//#define KTHURA_LOADED_CORE
#pragma once

#include <string>
#include <map>
#include <vector>

// Requires Tricky's Units!
#include <QuickString.hpp>

// JCR6 is needed for loading. Kthura files are just JCR6 files after all
#include <jcr6_core.hpp>

namespace NSKthura{
	
	enum class KthuraKind { Unknown, Zone, TiledArea, StretchedArea, Obstacle, Pic, Actor, CustomItem, Pivot, Exit };

	class Kthura;
	class KthuraLayer;
	class KthuraObject;
	class KthuraActor;
	
	typedef void (*KthuraAnimReset)(KthuraObject* O);
	
	class KthuraObject {
	private:
		KthuraLayer* parent;
		//int cnt = 0;
		int _id = 0;
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
		int _alpha1000 = 0;
		int _alpha255 = 0;
	public:
		KthuraLayer* GetParent();
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
		void Kind(std::string k,bool force=false); // Only works when kind is not yet defined! Otherwise this request will be ignored!
		KthuraKind EKind();
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
		bool Impassible();
		void ForcePassible(bool fp);
		bool ForcePassible();
		void RotationRadians(double value);
		double RotationRadians();
		void RotationDegrees(int value);
		int RotationDegrees(); 
		void Alpha255(int value);
		int Alpha255();
		int ID();
		void Animate(KthuraAnimReset RESET=NULL);
		bool IsInZone(std::string Tag);

		/// <summary>
		/// Sets the alpha in a 0-1000 scale. Please note that this was a leftover from the very original BlitzMax version of Kthura. That version is discontinued and all systems I used later use the official 0-255 scale. Therefore the use of the 1-1000 scale has been deprecated, and I strongly recommend to use Alpha255 in stead!
		/// </summary>
		/// <param name="value"></param>
		void Alpha1000(int value);			
		/// <summary>
		/// Deprecated! Use Alpha255 in stead.
		/// </summary>
		int Alpha1000();
		static 	KthuraObject Create(std::string Kind,KthuraLayer* p);
		bool CheckParent(KthuraLayer* p);
	};

	class KthuraActor :public KthuraObject {

	};
	
	class KthuraLayer{
	private:
		Kthura* parent=NULL;
		std::map<std::string, KthuraObject*> _TagMap;
		std::map<std::string, std::vector<KthuraObject*>> _LabelMap;		
		std::vector<bool> _BlockMap;
		std::map<int, KthuraObject*> ID_Map;
		int BM_W = 0;
		int BM_H = 0;
		int idinc = 0;
	public:
		Kthura* GetParent();
		void SetParent(Kthura* prnt);
		int nextID();
		int GridX{ 32 };
		int GridY{ 32 };
		std::vector<KthuraObject> Objects;
		std::map<int, KthuraObject*> GetIDMap();

		KthuraObject* TagMap(std::string Tag);
		std::map<std::string, KthuraObject*> _DomMap; // not to be documented!
		std::vector<KthuraObject*>* LabelMap(std::string label);
		KthuraObject* LastObject();
		
		bool HasTag(std::string Tag);
		bool Blocked(int x, int y);
		bool BlockedPix(int x, int y);
		void RemapDominance();
		void RemapTags();
		void RemapLabels();
		void BuildBlockMap();
		void RemapID();
		void TotalRemap();
		void NewObject(std::string Kind);
		void Kill(KthuraObject* O);
		void Kill(int ID);
		void Kill(std::string Tag);
		void KillAllObjects();
	};
	
	class Kthura {
	private:
		static int countup;
		int _id;
	public:
		///<summary>NEVER, and I repeat NEVER address the Layers map directly unless you know what you are doing!</summary>
		std::map<std::string, KthuraLayer> Layers;
		std::map<std::string, std::string> MetaData;
		jcr6::JT_Dir* TexDir=NULL;
		KthuraLayer* Layer(std::string lay);
		void NewLayer(std::string lay, bool force = false);
		void KillLayer(std::string lay);
		void KillAllLayers();
		void KillMap();
		void Load(jcr6::JT_Dir& JCR, std::string Prefix = "");
		void Load(std::string JCR, std::string Prefix = "");
		static void Throw(std::string err);
		/// <summary>
		/// Should Kthura have an error, this function will be executed *if* it's been defined. If it's not defined it will merely put a message on the console. 
		/// </summary>
		static void (*Panic)(std::string msg);
		static bool AutoMap;
		/// <summary>
		/// When set to false, instructions in mapfiles not understood will be ignored. Other wise errors will be reported based on the panic setting!
		/// </summary>
		static bool StrictLoad;
		int ID();
		Kthura();
		~Kthura();
	};
	
}

//#endif