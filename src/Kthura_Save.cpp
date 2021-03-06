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
