// Lic:
// src/Kthura_Save.hpp
// Kthura Save (header)
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