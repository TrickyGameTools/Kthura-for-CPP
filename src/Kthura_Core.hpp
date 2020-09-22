// Lic:
// src/Kthura_Core.hpp
// Kthura - Core (header)
// version: 20.09.21
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
//#include <dim2.hpp> // Didn't work the way it should... Needs more study!

// JCR6 is needed for loading. Kthura files are just JCR6 files after all
#include <jcr6_core.hpp>

namespace NSKthura{
	
	enum class KthuraKind { Unknown, Zone, TiledArea, StretchedArea, Obstacle, Pic, Actor, CustomItem, Pivot, Exit };

	class Kthura;
	class KthuraLayer;
	class KthuraRegObject;
	class KthuraObject;
	class KthuraActor;
	class KthuraPathFinder;
	
	typedef void (*KthuraAnimReset)(KthuraObject* O);

	class KthuraPoint {
	private:
		KthuraLayer* parent;
		int _exax = 0;
		int _exay = 0;
		int _grix = 0;
		int _griy = 0;
	public:
		void GX(int v);
		void GY(int v);
		void XX(int v);
		void XY(int v);
		int GX();
		int GY();
		int XX();
		int XY();
	};

	class KthuraPathFinder {
	public:
		virtual std::vector<KthuraPoint> FindPath(KthuraActor* A, int x, int y) = 0;
		bool Success = false; // Should contain 'true' if the last path finding attempt was succesful.
	};
	
	class KthuraRegObject {
	//private:
	protected:
		//int cnt = 0;
		int _x = 0, _y = 0;
		int AnimFrameSkip = 0;
		int _Dominance = 20;
		std::string _Labels = "";
		std::string _Tag = "";
		bool _impassible = false;
		bool _forcepassible = false;
		double _rotrad = 0;
		int _rotdeg = 0;
		int _alpha1000 = 0;
		int _alpha255 = 0;
		KthuraLayer* parent;
		std::string _Kind;
		int _id = 0;
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
		void Xp(int add);
		void Yp(int add);
		void Xm(int sub);
		void Ym(int syb);
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
		//void Animate(KthuraAnimReset RESET=NULL);
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
		//static 	KthuraObject Create(std::string Kind,KthuraLayer* p);
		bool CheckParent(KthuraLayer* p);
		KthuraRegObject();
		KthuraRegObject(KthuraLayer* p); // Should only be used by derrived classes
		KthuraRegObject(KthuraLayer* p, std::string setKind);
	};

	class KthuraActor :public KthuraRegObject {
	private:
		bool _InMotion=false;
		void Walk2Move();
	public:
		//public object DriverTextureObject = null; // To be defined by the graphics driver for its own needs
		std::string ChosenPic = "";

		void InMotion(bool value);
		bool InMotion();
		bool NotInMotionThen0 = true;
		bool Walking = false;
		bool WalkingIsInMotion = true;
		bool Moving = false;
		bool MoveIgnoreBlock = false;
		bool AutoWind = true;
		int UnMoveTimer = 4;
		int MoveX = 0, MoveY = 0;
		int MoveSkip = 4;
		int FrameSpeed = 4;
		int FrameSpeedCount = 0;
		int WalkSpot = 0;
		std::string Wind{ "NORTH" };
		int WalkingToX = 0, WalkingToY = 0;
		//Path FoundPath = null;
		std::vector<KthuraPoint> FoundPath;
		int PathIndex = 0;
		int PathLength();
		int Cycle = -1;
		int CWalkX();
		int CWalkY();
		void WalkTo(int to_x, int to_y, bool real = true);
		void WalkTo(KthuraObject* obj);
		void WalkTo(std::string Tag);
		void MoveTo(int x, int y);
		void MoveTo(KthuraObject* obj);
		void MoveTo(std::string Tag);
		static KthuraActor Spawn(KthuraLayer* parent, std::string spot);
		static KthuraActor Spawn(KthuraLayer* parent, KthuraObject* obj);
		static KthuraActor Spawn(KthuraLayer* parent, int x, int y, std::string wind = "NORTH", unsigned char R = 255, unsigned char G = 255, unsigned char B = 255, unsigned char alpha = 255, int Dominance = 20);
		void UpdateMoves();
		std::string Kind();
		KthuraKind EKind();
		KthuraActor(KthuraLayer* parent);
		
	};

	class KthuraObject {
	private:
		KthuraRegObject* O=NULL;
		KthuraActor* A=NULL;
		int AnimFrameSkip;
		int _id;
	public:
		~KthuraObject();
		KthuraObject(std::string aKind, KthuraLayer* prnt);
		KthuraLayer* GetParent();
		int ID();
		std::string MetaData(std::string key);
		int MetaDataCount(std::string key);
		std::string Texture();
		bool Visible();
		int W();
		int H();
		int insertx();
		int inserty();
		int R();
		int G();
		int B();
		int ScaleX();
		int ScaleY();
		int AnimSpeed();
		int AnimFrame();
		int Alpha255();
		int Alpha1000();
		float TrueScaleX();
		float TrueScaleY();
		std::string Kind();
		KthuraKind EKind();
		int X();
		int Y();
		std::string Tag();
		int Dominance();
		std::string Labels();
		bool ForcePassible();
		bool Impassible();
		int RotationDegrees();
		double RotationRadians();
		void MetaData(std::string key, std::string value);
		void Texture(std::string value);
		void Visible(bool value);
		void W(int value);
		void H(int value);
		void insertx(int value);
		void inserty(int value);
		void ScaleX(int value);
		void ScaleY(int value);
		void AnimSpeed(int value);
		void AnimFrame(int value);
		void X(int value);
		void Y(int value);
		void Tag(std::string value);
		void Dominance(int value);
		void Labels(std::string value);
		void ForcePassible(bool value);
		void Impassible(bool value);
		void RotationDegrees(int value);
		void RotationRadians(double value);
		void Alpha255(int value);
		void Alpha1000(int value);
		void R(int value);
		void G(int value);
		void B(int value);
		

		void Animate(KthuraAnimReset RESET = NULL);
		void Xp(int value);
		void Yp(int value);
		void Xm(int value);
		void Ym(int value);
		void Kind(std::string k, bool force = false);
		bool IsInZone(std::string zone);
		bool CheckParent(KthuraLayer* p);

		static KthuraObject Create(std::string Kind, KthuraLayer* p);
	};
	
	class KthuraLayer{
	private:
		Kthura* parent=NULL;
		std::map<std::string, KthuraObject*> _TagMap;
		std::map<std::string, std::vector<KthuraObject*>> _LabelMap;		
		std::vector<bool> _BlockMap;
		//dim2<bool> _Blockmap(1, 1);
		std::map<int, KthuraObject*> ID_Map;
		int BlockMapBoundW{ 0 }, BlockMapBoundH{ 0 };
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
		//std::vector<KthuraActor> Actors;
		int CountObjects();
		KthuraObject* ObjFIdx(int index);
		std::map<int, KthuraObject*> GetIDMap();

		KthuraObject* TagMap(std::string Tag);
		std::string TagList();
		std::map<std::string, KthuraObject*> _DomMap; // not to be documented!
		std::vector<KthuraObject*>* LabelMap(std::string label);
		KthuraObject* LastObject();

		std::string BlockMapStringDump(); // For debug purposes only!
		
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
		void Spawn(std::string spottag, std::string ActorTag);
		void Spawn(KthuraObject* spot, std::string ActorTag);
		void Spawn(std::string ActorTag, int x, int y, std::string wind = "NORTH", unsigned char R = 255, unsigned char G = 255, unsigned char B = 255, unsigned char alpha = 255, int Dominance = 20);
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
		void Remap();
		static void Throw(std::string err);
		/// <summary>
		/// Should Kthura have an error, this function will be executed *if* it's been defined. If it's not defined it will merely put a message on the console. 
		/// </summary>
		static void (*Panic)(std::string msg);
		static KthuraPathFinder* PathFinder;
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