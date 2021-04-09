#pragma once

#include <jcr6_core.hpp>
#include "Kthura_Core.hpp"

namespace NSKthura {

	/// <summary>
	/// Saves a Kthura map. Please note that this function (and entire library) is only meant for Kthura editor tools.
	/// </summary>
	/// <param name="Map">Pointer to the map</param>
	/// <param name="MainFile">The file to save to. Kthura files are based on JCR6.</param>
	/// <param name="Prefix">Prefix to be used inside the JCR file</param>
	/// <param name="Storage">If not set it will follow the map's options settings or use "Store"</param>
	void KthuraSave(Kthura* Map, std::string MainFile, std::string Prefix = "", std::string Storage = "");

	/// <summary>
	/// Saves a Kthura map. Please note that this function (and entire library) is only meant for Kthura editor tools.
	/// </summary>
	/// <param name="Map">Shared pointer to the map</param>
	/// <param name="MainFile">The file to save to. Kthura files are based on JCR6.</param>
	/// <param name="Prefix">Prefix to be used inside the JCR file</param>
	/// <param name="Storage">If not set it will follow the map's options settings or use "Store"</param>
	void KthuraSave(std::shared_ptr<Kthura> Map, std::string MainFile, std::string Prefix = "", std::string Storage = "");

	/// <summary>
	/// Saves a Kthura map. Please note that this function (and entire library) is only meant for Kthura editor tools.
	/// </summary>
	/// <param name="Map">Pointer to the map</param>
	/// <param name="Resource">JCR6 creation resource to add the map to</param>
	/// <param name="Prefix">Prefix to be used inside the JCR file</param>
	/// <param name="Storage">If not set it will follow the map's options settings or use "Store"</param>
	void KthuraSave(Kthura* Map, jcr6::JT_Create* Resource, std::string Prefix = "", std::string Storage = "");
	// ^^ This is the function were all the "magic" really happens ^^ //


	/// <summary>
	/// Saves a Kthura map. Please note that this function (and entire library) is only meant for Kthura editor tools.
	/// </summary>
	/// <param name="Map">Pointer to the map</param>
	/// <param name="Resource">JCR6 creation resource to add the map to</param>
	/// <param name="Prefix">Prefix to be used inside the JCR file</param>
	/// <param name="Storage">If not set it will follow the map's options settings or use "Store"</param>
	void KthuraSave(std::shared_ptr<Kthura> Map, jcr6::JT_Create* Resource, std::string Prefix = "", std::string Storage = "");

}
