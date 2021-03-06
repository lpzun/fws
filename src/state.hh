/*
 * state.hh
 *
 *  Created on: Jun 21, 2015
 *      Author: lpzun
 */

#ifndef STATE_HH_
#define STATE_HH_

#include "heads.hh"
#include "algs.hh"

namespace sura {

/// define local state
typedef unsigned short Local_State;
/// define the size of local states
typedef unsigned short size_l;

/// define shared state
typedef unsigned short Shared_State;
/// define size of shared states
typedef unsigned short size_s;

/// define the counter of thread state
typedef unsigned short size_p;

/// define the thread state id
typedef unsigned int thread_state_id;

/// class thread state
class Thread_State {
public:
	static size_s S; /// the size of shared state
	static size_l L; /// the size of local  state

	Shared_State share;
	Local_State local;

	inline Thread_State();
	inline Thread_State(const Thread_State& t);
	inline Thread_State(const Shared_State& share, const Local_State& local);
	virtual ~Thread_State() {
	}

	ostream& to_stream(ostream& out = cout) const;
};

typedef map<Thread_State, deque<Thread_State>> adjacency_list;

/**
 * @brief default constructor
 */
inline Thread_State::Thread_State() :
		share(0), local(0) {
}

/**
 * @brief constructor with thread state
 * @param t
 */
inline Thread_State::Thread_State(const Thread_State& t) :
		share(t.share), local(t.local) {

}

/**
 * @brief constructor with shared state and local state
 * @param share: shared state
 * @param local: local  state
 */
inline Thread_State::Thread_State(const Shared_State& share,
		const Local_State& local) :
		share(share), local(local) {
}

/**
 * @brief print thread state
 * @param out
 * @return ostream
 */
inline ostream& Thread_State::to_stream(ostream& out) const {
	out << "(" << share << "|" << local << ")";
	return out;
}

/**
 * @brief overloading operator <<: print thread state
 * @param out
 * @param ts
 * @return ostream
 */
inline ostream& operator<<(ostream& out, const Thread_State& t) {
	return t.to_stream(out);
}

/**
 * @brief overloading operator ==
 * @param t1
 * @param t2
 * @return bool
 * 		   true : if t1 == t2
 * 		   false: otherwise
 */
inline bool operator==(const Thread_State& t1, const Thread_State& t2) {
	return t1.share == t2.share && t1.local == t2.local;
}

/**
 * @brief overloading operator !=
 * @param t1
 * @param t2
 * @return bool
 * 		   true : if t1 == t2
 * 		   false: otherwise
 */
inline bool operator!=(const Thread_State& t1, const Thread_State& t2) {
	return !(t1 == t2);
}

/**
 * @brief overloading operator <
 * @param t1
 * @param t2
 * @return bool
 * 		   true : if t1 < t2
 * 		   false: otherwise
 */
inline bool operator<(const Thread_State& t1, const Thread_State& t2) {
	if (t1.share == t2.share)
		return t1.local < t2.local;
	else
		return t1.share < t2.share;
}

/**
 * @brief overloading operator >
 * @param t1
 * @param t2
 * @return bool
 * 		   true : if t1 > t2
 * 		   false: otherwise
 */
inline bool operator>(const Thread_State& t1, const Thread_State& t2) {
	return t2 < t1;
}

/// class global state
typedef map<Local_State, size_p> Locals;

class Global_State {
public:
	Shared_State share;
	Locals locals;

	inline Global_State();
	inline Global_State(const Thread_State& t);
	inline Global_State(const Thread_State& t, const size_p &n);
	inline Global_State(const Shared_State& share, const Locals& locals);
	virtual ~Global_State() {
	}

	ostream& to_stream(ostream& out = cout, const string& sep = "|") const;
};

/**
 * @brief default constructor: initialize
 *        share  = 0
 *        locals = empty map
 */
inline Global_State::Global_State() :
		share(0), locals(Locals()) {
}

/**
 * @brief constructor with a thread state
 * @param t
 */
inline Global_State::Global_State(const Thread_State& t) :
		share(t.share) {
	locals[t.local] = 1;
}

/**
 * @brief constructor with thread state t and n
 * @param t
 * @param n
 */
inline Global_State::Global_State(const Thread_State& t, const size_p &n) :
		share(t.share) {
	locals[t.local] = n;
}

/**
 * @brief constructor with a shared state and local states
 * @param share : shared state
 * @param locals: local states represented in counter abstraction form
 */
inline Global_State::Global_State(const Shared_State& share,
		const Locals& locals) :
		share(share), locals(locals) {
}

/**
 * @brief call by <<
 * @param out
 * @param sep
 * @return ostream
 */
inline ostream& Global_State::to_stream(ostream& out, const string& sep) const {
	out << "<" << this->share << "|";
	for (auto iloc = this->locals.begin(); iloc != this->locals.end(); ++iloc) {
		out << "(" << iloc->first << "," << iloc->second << ")";
	}
	out << ">";
	return out;
}

/**
 * @brief overloading operator <<
 * @param out
 * @param g
 * @return
 */
inline ostream& operator<<(ostream& out, const Global_State& s) {
	return s.to_stream(out);
}

/**
 * @brief overloading operator <
 * @param s1
 * @param s2
 * @return bool
 * 		   true : if s1 < s2
 * 		   false: otherwise
 */
inline bool operator<(const Global_State& s1, const Global_State& s2) {
	if (s1.share == s2.share) {
		return COMPARE::compare_map(s1.locals, s2.locals) == -1;
	} else {
		return s1.share < s2.share;
	}
}

/**
 * @brief overloading operator >
 * @param s1
 * @param s2
 * @return bool
 * 		   true : if s1 > s2
 * 		   false: otherwise
 */
inline bool operator>(const Global_State& s1, const Global_State& s2) {
	return s2 < s1;
}

/**
 * @brief overloading operator ==
 * @param s1
 * @param s2
 * @return bool
 * 		   true : if s1 == s2
 * 		   false: otherwise
 */
inline bool operator==(const Global_State& s1, const Global_State& s2) {
	if (s1.share == s2.share) {
		if (s1.locals.size() == s2.locals.size()) {
			auto is1 = s1.locals.begin(), is2 = s2.locals.begin();
			while (is1 != s1.locals.end()) {
				if ((is1->first != is2->first) || (is1->second != is2->second))
					return false;
				is1++, is2++;
			}
			return true;
		}
	}
	return false;
}

/**
 * @brief overloading operator !=
 * @param s1
 * @param s2
 * @return bool
 * 		   true : if s1 != s2
 * 		   false: otherwise
 */
inline bool operator!=(const Global_State& s1, const Global_State& s2) {
	return !(s1 == s2);
}

} /* namespace SURA */

#endif /* STATE_HH_ */
