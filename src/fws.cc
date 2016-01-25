/*
 * fws.cc
 *
 *  Created on: Jun 30, 2015
 *      Author: lpzun
 */

#include "fws.hh"

namespace sura {

FWS::FWS() {
	// TODO Auto-generated constructor stub
}

FWS::FWS(const Thread_State& init_ts, const adjacency_list& TTD) :
		init_ts(init_ts), TTD(TTD) {

}

FWS::~FWS() {
	// TODO Auto-generated destructor stub
}

void FWS::cutoff_detection() {
	size_p cutoff = 1;
	uint oreach = 0, nreach = 0;
	while (cutoff < 10) { ///TODO we set this as 10, this is no good
		auto R = this->standard_FWS(cutoff, cutoff);
		auto mark_R = this->extract_reachable_TS(R);
		nreach = this->statistic(mark_R);
		cout << "Under Setting: " << cutoff << " threads at initial state, "
				<< cutoff << " spawn transitions" << endl;
		if (Refs::OPT_PRT_REACH_TS)
			this->print_reachable_TS(mark_R); // print out all reachable thread states
		if (Refs::OPT_PRT_UNREACH_TS)
			this->print_unreachable_TS(mark_R); // print out all unreachable thread states

		if (nreach == oreach)
			break;
		else
			oreach = nreach;
		++cutoff;
	}
	if (Refs::INPUT_IS_TTS)
		this->standard_FWS(cutoff + 1, cutoff - 1);
	cout << "cutoff is " << (cutoff + cutoff) << endl;
}

/**
 * @brief standard forward search: to compute the set of reachable thread state which n
 * 			threads in initial state and at most s threads created dynamically
 * @param TTD: TTD represented in a adjacency list
 * @param n  : number of threads at the initial states
 * @param s  : maximum number of spawn transition could be fired
 */
set<Global_State> FWS::standard_FWS(const size_p& n, const size_p& s) {
	auto spw = s; // local copy of maximum number of spawn transition could be fired
	queue<Global_State, deque<Global_State>> W; /// worklist
	W.emplace(init_ts, n); /// start from the initial state with n threads
	set<Global_State> R; /// reachable global states
	while (!W.empty()) {
		Global_State tau = W.front();
		W.pop();
		const ushort &shared = tau.share;
		for (auto il = tau.locals.begin(); il != tau.locals.end(); il++) {
			Thread_State src(shared, il->first);
			auto ifind = TTD.find(src);
			if (ifind != TTD.end()) {
				for (auto idst = ifind->second.begin();
						idst != ifind->second.end(); idst++) {
					bool is_spawn = Util::is_spawn_transition(src, *idst); // if (src, dst) is a spawn transition
					if (is_spawn) {
						if (spw > 0) {
							spw--;
						} else { // if the we already fire s spawn transitions, then
							continue; // we can't spawn again and have to skip src +> dst;
						}
					}
					Global_State _tau(idst->share,
							this->update_counter(tau.locals, src.local,
									idst->local, is_spawn)); // successor of tau
					if (R.emplace(_tau).second) { // if _tau is haven't been reached before
						W.emplace(_tau);
					}
				}
			}
		}
	}
	return R;
}

/**
 * @brief update counters in local part
 * @param Z  : current locals
 * @param dec: decremental local
 * @param inc: incremental local
 * @param is_spawn:
 * @return local part after updating
 */
Locals FWS::update_counter(const Locals &Z, const Local_State &dec,
		const Local_State &inc, const bool &is_spawn) {
	auto _Z = Z;   /// local copy of Z

	if (!is_spawn) {
		if (dec == inc) /// if dec == inc
			return Z;

		auto idec = _Z.find(dec);
		if (idec != _Z.end()) {
			idec->second--;
			if (idec->second == 0)
				_Z.erase(idec);
		} else {
			throw CONTROL::Error(
					"update_counter: missed local state" + std::to_string(dec));
		}
	}

	auto iinc = _Z.find(inc);
	if (iinc != _Z.end()) {
		iinc->second++;
	} else {
		_Z.emplace(inc, 1);
	}

	return _Z;
}

/**
 * @brief extract all reachable thread states from all reachable global states
 * @param R: the set if reachable lobal states
 * @return S x L bit matrix:
 * 			R[s][l] = true : thread state (s, l) is reachable
 * 			R[s][l] = false: thread state (s, l) is unreachable
 */
vector<vector<bool>> FWS::extract_reachable_TS(const set<Global_State>& R) {
	vector<vector<bool>> reached(Thread_State::S,
			vector<bool>(Thread_State::L, false));
	reached[0][0] = true;
	for (auto itau = R.begin(); itau != R.end(); ++itau) {
		const auto &share = itau->share;
		for (auto iloc = itau->locals.begin(); iloc != itau->locals.end();
				++iloc) {
			const auto &local = iloc->first;
			if (!reached[share][local])
				reached[share][local] = true;
		}
	}
	return reached;
}

/**
 * @brief print out all reachable thread states
 * @param R: S x L bit matrix:
 * 			R[s][l] = true : thread state (s, l) is reachable
 * 			R[s][l] = false: thread state (s, l) is unreachable
 */
void FWS::print_reachable_TS(const vector<vector<bool>>& R) {
	cout << "Reachable Thread States:\n";
	for (auto s = 0; s < Thread_State::S; ++s) {
		for (auto l = 0; l < Thread_State::L; ++l) {
			if (R[s][l])
				cout << s << "|" << l << "\n";
		}
	}
	cout << endl;
}

/**
 * @brief print out all unreachable thread states
 * @param R: S x L bit matrix:
 * 			R[s][l] = true : thread state (s, l) is reachable
 * 			R[s][l] = false: thread state (s, l) is unreachable
 */
void FWS::print_unreachable_TS(const vector<vector<bool>>& R) {
	cout << "Unreachable Thread States:\n";
	for (auto s = 0; s < Thread_State::S; ++s) {
		for (auto l = 0; l < Thread_State::L; ++l) {
			if (!R[s][l])
				cout << s << "|" << l << "\n";
		}
	}
	cout << endl;
}

/**
 * @brief statistics
 * @param R: S x L bit matrix:
 * 			R[s][l] = true : thread state (s, l) is reachable
 * 			R[s][l] = false: thread state (s, l) is unreachable
 */
uint FWS::statistic(const vector<vector<bool>>& R) {
	uint reach = 0;
	for (auto s = 0; s < Thread_State::S; ++s) {
		for (auto l = 0; l < Thread_State::L; ++l) {
			if (R[s][l])
				reach++;
		}
	}
	cout << "current # of   reachable Thread States: " << reach << "\n";
	cout << "current # of unreachable Thread States: "
			<< (Thread_State::S * Thread_State::L - reach) << "\n";
	cout << endl;
	return reach;
}

/////////////////////// utilities class ////////////////////////////////

Util::Util() {
	// TODO Auto-generated constructor stub

}

Util::~Util() {
	// TODO Auto-generated destructor stub
}

/**
 * @brief determine if (src, dst) correpsonds to a spawn transition
 * @param src
 * @param dst
 * @param spawn_trans
 * @return bool
 * 			true : src +> dst
 * 			false: otherwise
 */
bool Util::is_spawn_transition(const Thread_State& src,
		const Thread_State& dst) {
	auto ifind = Refs::spawntra_TTD.find(src);
	if (ifind == Refs::spawntra_TTD.end()) {
		return false;
	} else {
		auto idst = std::find(ifind->second.begin(), ifind->second.end(), dst);
		if (idst == ifind->second.end())
			return false;
		else
			return true;
	}
}

/**
 *
 * @param s
 * @param delim
 * @param elems
 * @return
 */
vector<string> Util::split(const string &s, char delim, vector<string> &elems) {
	std::stringstream ss(s);
	string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

/**
 * @brief
 * @param s
 * @param delim
 * @return
 */
vector<string> Util::split(const string &s, const char& delim) {
	vector<string> elems;
	split(s, delim, elems);
	return elems;
}

/**
 *
 * @param s_ts
 * @param delim
 * @return
 */
Thread_State Util::create_thread_state_from_str(const string& s_ts,
		const char& delim) {
	auto vts = split(s_ts, delim);
	if (vts.size() != 2)
		throw CONTROL::Error("The format of thread state is wrong.");
	const string &share = vts[0];
	const string &local = vts[1];
	return Thread_State(std::stoi(share), std::stoi(local));
}

/**
 * @brief print all of the transitions in the thread-state transition diagram
 * @param adjacency_list
 * @param out
 */
void Util::print_adj_list(const adjacency_list& adj_list, ostream& out) {
	out << Thread_State::S << " " << Thread_State::L << endl;
	for (auto pair = adj_list.begin(), end = adj_list.end(); pair != end;
			++pair) {
		__SAFE_ASSERT__ (! pair->second.empty());
		for (auto isucc = pair->second.begin(), end = pair->second.end();
				isucc != end; ++isucc) {
			out << pair->first
					<< (is_spawn_transition(pair->first, *isucc) ?
							" +> " : " -> ") << (*isucc) << endl;
		}
	}
}

} /* namespace sura */
