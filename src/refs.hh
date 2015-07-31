/*
 * refs.hh
 *
 *  Created on: Jun 30, 2015
 *      Author: lpzun
 */

#ifndef REFS_HH_
#define REFS_HH_

#include "state.hh"

namespace sura {
class Refs {
public:
	Refs(){

	}
	~Refs(){

	}

	static bool OPT_PRT_ALL;
	static bool OPT_PRT_REACH_TS;
	static bool OPT_PRT_STATISTIC;
	static bool OPT_PRT_UNREACH_TS;
	static adjacency_list spawntra_TTD;
};

} /* namespace sura */

#endif /* REFS_HH_ */
