//============================================================================
// Name        : fws.cpp
// Author      : Peizun Liu
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "state.hh"
#include "input.hh"
#include "fws.hh"

using namespace sura;

/**
 * @brief main function: the entry of this program
 * @param argc
 * @param argv
 * @return int
 * 					 0: successful
 */
int main(const int argc, const char * const * const argv) {
	try {
		Input ins;
		//"X" is the default filename, means there are no input file
		ins.add_argument("-f", "input file (if given, don't specify s,l,e)", "",
				"X");
		ins.add_argument("-n", "number of threads at the initial state", "",
				"1");
		ins.add_argument("-s", "maximum times of spawn transitions fired", "",
				"0");
		ins.add_argument("-i", "the initial thread state", "", "0|0");

		ins.add_switch("--tts", "TTS or BP");
		ins.add_switch("--cutoff", "cutoff detection");
		ins.add_switch("--adj-list", "whether to print the adjacency list");
		ins.add_switch("--cmd-line", "whether to print the command line");
		ins.add_switch("--reach-ts",
				"whether to print all reachable thread states");
		ins.add_switch("--unreach-ts",
				"whether to print all unreachable thread states");
		ins.add_switch("--statistic",
				"whether to print the statistic information");
		ins.add_switch("--all", "whether to print all of the above");

		try {
			ins.get_command_line(argc, argv);
		} catch (Input::Help) {
			return 0;
		}

		Refs::OPT_PRT_ALL = ins.arg2bool("--all");
		if (ins.arg2bool("--cmd-line") || Refs::OPT_PRT_ALL) {
			ins.print_command_line(0);
		}

		string filename = ins.arg_value("-f");
		//FILE_NAME_PREFIX = filename.substr(0, filename.find_last_of("."));
		const string s_inital = ins.arg_value("-i");
		const size_p n = atol(ins.arg_value("-n").c_str());
		const size_p s = atol(ins.arg_value("-s").c_str());

		const bool is_cutoff = ins.arg2bool("--cutoff");

		Refs::INPUT_IS_TTS = ins.arg2bool("--tts");
		Refs::OPT_PRT_REACH_TS = ins.arg2bool("--reach-ts");
		Refs::OPT_PRT_STATISTIC = ins.arg2bool("--statistic");
		Refs::OPT_PRT_UNREACH_TS = ins.arg2bool("--unreach-ts");

		Thread_State init_ts = Util::create_thread_state_from_str(s_inital);
		adjacency_list original_TTD;
		if (filename == "X") {
			throw CONTROL::Error("no input file");
		} else {
			if (!Refs::INPUT_IS_TTS) {
				cout<<"I am here...\n";
				filename = filename.substr(0, filename.find_last_of("."));
				filename += ".tts";
				cout << filename << endl;
			}
			ifstream org_in(filename.c_str());
			CONTROL::remove_comments(org_in, "/tmp/tmp.ttd.no_comment", "#");
			org_in.close();
			ifstream new_in("/tmp/tmp.ttd.no_comment");
			new_in >> Thread_State::S >> Thread_State::L;
			ushort s1, l1, s2, l2;
			string sep;
			while (new_in >> s1 >> l1 >> sep >> s2 >> l2) {
				if (sep == "->" || sep == "+>") {
					if (sep == "+>")
						Refs::spawntra_TTD[Thread_State(s1, l1)].emplace_back(
								s2, l2);
					original_TTD[Thread_State(s1, l1)].emplace_back(s2, l2);
				} else {
					throw CONTROL::Error("illegal transition");
				}
			}
			new_in.close();
		}

		if (ins.arg2bool("--adj-list") || Refs::OPT_PRT_ALL) {
			cout << "Adjacency list:" << endl;
			Util::print_adj_list(original_TTD);
		}

		FWS fws(init_ts, original_TTD);
		if (is_cutoff)
			fws.cutoff_detection();
		else
			fws.standard_FWS(n, s);
		return 0;
	}

	catch (const CONTROL::Error& error) {
		error.print_exit();
	} catch (...) {
		CONTROL::Error("main: unknown exception occurred").print_exit();
	}
}
