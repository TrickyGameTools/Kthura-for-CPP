// Lic:
// src/Kthura_Save.cpp
// Kthura Save
// version: 22.01.06
// Copyright (C) 2021, 2022 Jeroen P. Broks
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
#include "Kthura_Save.hpp"

#define SAVE_DEBUG



// This will just make the Macros be there if needed, but result to empty code when not needed! Cool, huh?
#ifdef SAVE_DEBUG
#define SDC(a) std::cout << "\x1b[33mKTHURA SAVE DEBUG!\x1b[0m " << a << "\n"
#else
#define SDC(a)
#endif

#define ts(a) std::to_string(a)

using namespace TrickyUnits;
using namespace jcr6;

namespace NSKthura {
	void KthuraSave(Kthura* Map, std::string MainFile, std::string Prefix, std::string Storage) {
		auto UseStorage = Storage;
		if (UseStorage == "") UseStorage = Trim(Map->Options.Value("Save", "Storage"));
		if (UseStorage == "") UseStorage = "Store";
		SDC( "Save with method \"" << UseStorage << "\" to: "<<MainFile);
		JT_Create SaveRes{ MainFile,UseStorage };
		KthuraSave(Map, &SaveRes, Prefix, Storage);
		SaveRes.Close();
		
	}

	void KthuraSave(std::shared_ptr<Kthura> Map, std::string MainFile, std::string Prefix, std::string Storage) { KthuraSave(Map.get(), MainFile, Prefix, Storage); }


	void KthuraSave(std::shared_ptr<Kthura> Map, jcr6::JT_Create* Resource, std::string Prefix, std::string Storage) { KthuraSave(Map.get(), Resource, Prefix, Storage); }


	void KthuraSave(Kthura* Map, jcr6::JT_Create* Resource, std::string Prefix, std::string Storage) {
		auto UseStorage = Storage;
		if (UseStorage == "") UseStorage = Trim(Map->Options.Value("Save", "Storage"));
		if (UseStorage == "") UseStorage = "Store";
		// Save the unknown first (this is a kind of security measure. This way true Kthura data will always overwrite unknown data. Kthura users uing their own data should be aware of that)
		if (Kthura::AllowUnknown) {
			for (auto ku : (*Map->UnknownData())) Resource->AddCharacters(Prefix+ku.first, ku.second, UseStorage);
		}
		
		SDC("Save with method \"" << UseStorage << "\" to resource: " << (unsigned long long)Resource);

		Resource->AddString(Prefix + "Options", Map->Options.UnParse(), UseStorage);
		Resource->AddStringMap(Prefix + "Data", Map->MetaData, UseStorage);
		std::string Output{ "-- Kthura (save library build: "+std::string(__TIMESTAMP__)+")\n\n" };
		// Layer summary
		Output += "LAYERS\n";
		for (auto l : Map->Layers) Output += "\t" + l.first + "\n";
		Output += "__END\n\n\n";
		// Layer actual data
		for (auto l : Map->Layers) {
			auto lay = l.second.get();
			Output += "-- Layer: " + l.first + "\nLAYER = " + l.first + "\n";
			Output += "BLOCKMAPGRID = " + ts(lay->GridX) + "x" + ts(lay->GridY) + "\n\n\n";
			for (auto o : lay->Objects) {
				Output += "NEW\n";
				Output += "\tKIND = " + o->Kind()+"\n";
				Output += "\tCOORD = " + ts(o->X()) + ", " + ts(o->Y())+"\n";
				Output += "\tINSERT = " + ts(o->insertx()) + ", " + ts(o->inserty()) + "\n";
				Output += "\tROTATION = " + ts(o->RotationDegrees()) + "\n";
				Output += "\tSIZE = " + ts(o->W()) + "x" + ts(o->H()) + "\n";
				Output += "\tTAG = " + o->Tag() + "\n";
				Output += "\tLABELS = " + o->Labels() + "\n";
				Output += "\tDOMINANCE = " + ts(o->Dominance()) + "\n";
				Output += "\tTEXTURE = " + o->Texture() + "\n";
				Output += "\tCURRENTFRAME = " + ts(o->AnimFrame()) + "\n";
				Output += "\tFRAMESPEED = " + ts(o->AnimSpeed()) + "\n";
				//ALPHA = 1.0000000000000000 // Will no longer be used!
				Output += "\tALPHA255 = " + ts(o->Alpha255()) + "\n";
				Output += "\tVISIBLE = " + ts(o->Visible()) + "\n";
				Output += "\tCOLOR = " + ts(o->R()) + ", " + ts(o->G()) + ", " + ts(o->B()) + "\n";
				Output += "\tIMPASSIBLE = " + ts(o->Impassible()) + "\n";
				Output += "\tFORCEPASSIBLE = " + ts(o->ForcePassible()) + "\n";
				Output += "\tSCALE = " + ts(o->ScaleX()) + ", " + ts(o->ScaleY()) + "\n";
				Output += "\tBLEND = " + ts(o->Blend()) + "\n";
				for (auto idata : o->MetaDataFields()) {
					Output += "\tDATA." + idata + " = " + o->MetaData(idata) + "\n";
				}
				Output += "\n\n";
			}
		}
		Resource->AddString(Prefix + "Objects", Output, UseStorage);
	}
}