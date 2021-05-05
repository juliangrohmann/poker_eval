#pragma once

#include "poker_game.h"

#include <array>
#include <map>
#include <utility>

namespace Poker {

	struct SlimCard {
		char rank;
		char suit;
	};

	inline bool operator<(const SlimCard& hero, const SlimCard& vill) { return hero.rank < vill.rank; }
	inline bool operator>(const SlimCard& hero, const SlimCard& vill) { return vill < hero; }
	inline bool operator==(const SlimCard& hero, const SlimCard& vill) { return !(hero < vill) && !(hero > vill); }
	inline bool operator!=(const SlimCard& hero, const SlimCard& vill) { return !(hero == vill); }
	inline bool operator>=(const SlimCard& hero, const SlimCard& vill) { return hero > vill || hero == vill; }
	inline bool operator<=(const SlimCard& hero, const SlimCard& vill) { return hero < vill || hero == vill; }

	struct SlimHand {
		SlimCard primary;
		SlimCard secondary;
	};

	struct BoardCache {
		std::array<SlimCard, 5> board;
		std::array<std::array<char, 13>, 13> straights;
		std::vector<std::pair<char, char>> ranks;
		std::array<char, 4> suits = { 0 };
		char max_suit = 0;
		char suit_count = 0;
	};

	struct HandCache {
		SlimHand hand;
		std::array<char, 15> ranks = { 0 };
		std::array<char, 4> suits = { 0 };
	};

	class CachedEvaluator {
	public:
		CachedEvaluator(HandCache*& hero_cache, HandCache*& vill_cache, BoardCache*& board_cache)
			: hero{ hero_cache }, vill{ vill_cache }, board_cache{ board_cache } {}

		int evaluate();

	private:
		struct Props {
			Props(HandCache*& cache) : cache{ cache } {}

			HandCache*& cache;
			std::array<SlimCard, 7> cards;
			std::array<int, 15> ranks;
			std::array<int, 4> suits;
			std::array<std::array<char, 5>, 4> matches;
			std::array<int, 4> match_counts;
			std::array<char, 5> kickers = { 1 };
			std::array<char, 3> hand_ranks = { 1 };
			char straight_rank = 1;
			bool is_flush = false;
			bool is_straight = false;
			bool is_royal = false;
			bool is_strf = false;
			bool is_full_house = false;
			int kicker_count = 0;
		};

		void process_flush(Props& props);
		void process_straight(Props& props);
		void process_matches();

		void init_royal_flush(Props& props);
		void init_straight_flush(Props& props);
		void init_quads();
		void init_flush();
		void init_trips();
		void init_two_pair();
		void init_pair();
		void eval_two_kickers();

		bool check_royal();
		bool check_strf();
		bool check_quads();
		bool check_full_house();
		bool check_flush();
		bool check_straight();
		bool check_trips();
		bool check_two_pair();
		bool check_pair();
		void check_high_card();

		BoardCache*& board_cache;
		Props hero;
		Props vill;

		std::array<char, 2> min_ranks = { 1 };
		int result = 0;
	};
}