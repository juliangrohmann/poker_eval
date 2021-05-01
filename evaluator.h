#pragma once

#include "poker_game.h"

#include <array>
#include <map>
#include <utility>

namespace Poker {
	enum class Winner {
		HERO, VILL, SPLIT
	};

	class RuntimeEvaluator {
	public:
		RuntimeEvaluator(const PokerHand& hero_hand, const PokerHand& vill_hand, const Board& board)
			: hero{ hero_hand }, vill{ vill_hand }, board{ board } {}

		Winner evaluate();

	private:
		struct Props {
			Props(const PokerHand& hand) : hand{ hand } {}

			const PokerHand& hand = PokerHand(Card(), Card());
			std::array<Card, 7> cards;
			std::array<int, 13> ranks;
			std::array<int, 4> suits;
			std::array<CardRank, 3> pairs = { CardRank::PLACEHOLDER };
			std::array<CardRank, 2> trips = { CardRank::PLACEHOLDER };
			std::array<CardRank, 5> kickers = { CardRank::PLACEHOLDER };
			CardRank quads = CardRank::PLACEHOLDER;
			CardSuit flush_suit = CardSuit::PLACEHOLDER;
			CardRank straight_rank = CardRank::PLACEHOLDER;
			bool is_flush = false;
			bool is_straight = false;
			bool is_royal = false;
			bool is_strf = false;
			bool is_quads = false;
			bool is_full_house = false;
			int trips_count = 0;
			int pair_count = 0;
		};

		void process_cards();
		void process_flush();
		void process_straight(Props& props);
		void process_ranks();
		void process_matches(Props& props);

		void init_royal_flush(Props& props);
		void init_straight_flush(Props& props);
		void init_quads(Props& props);
		void init_full_house(Props& props);
		void init_flush(Props& props);
		void init_trips(Props& props);
		void init_two_pair(Props& props);
		void init_pair(Props& props);

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

		const Board& board;
		Props hero;
		Props vill;

		CardRank curr_rank = CardRank::PLACEHOLDER;
		CardRank next_rank = CardRank::PLACEHOLDER;
		Winner result = Winner::HERO;
	};
}