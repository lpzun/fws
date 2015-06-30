/*
 * fws.hh
 *
 *  Created on: Jun 30, 2015
 *      Author: lpzun
 */

#ifndef FWS_HH_
#define FWS_HH_

#include "state.hh"
#include "functions.hh"
#include "refs.hh"

namespace sura {

class FWS {
public:
	FWS();
	FWS(const Thread_State& init_ts);
	virtual ~FWS();

	void standard_FWS(const adjacency_list& TTD, const size_p& n, const size_p& s);

private:
	Thread_State init_ts;
	vector<vector<bool>> extract_reachable_TS(const set<Global_State>& R);
	void print_reachable_TS(const vector<vector<bool>>& R);
	void print_unreachable_TS(const vector<vector<bool>>& R);
	void statistic(const vector<vector<bool>>& R);

	Locals update_counter(const Locals &Z, const Local_State &dec, const Local_State &inc, const bool &is_spawn);
};

class Util {
public:
	Util();
	virtual ~Util();

	static vector<string> split(const string &s, char delim, vector<string> &elems);
	static vector<string> split(const string &s, const char& delim);
	static Thread_State create_thread_state_from_str(const string& s_ts, const char& delim = '|');
	static Global_State create_global_state_from_str(const string& s_gs, const char& delim = '|');
	static void print_adj_list(const map<Thread_State, list<Thread_State> >& adjacency_list, ostream& out = cout);
	static bool is_spawn_transition(const Thread_State& src, const Thread_State& dst);
};

} /* namespace sura */

#endif /* FWS_HH_ */
