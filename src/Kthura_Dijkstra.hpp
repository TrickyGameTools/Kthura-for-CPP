#pragma once
#include <Kthura.hpp>
#include <Dijkstra.hpp>

namespace NSKthura {

	using namespace TrickyUnits;

	class Kthura_Dijkstra :public KthuraPathFinder {
		std::vector<KthuraPoint> FindPath(KthuraActor* A, int x, int y);
	};

}