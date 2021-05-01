#include "evaluator.h"

#include <algorithm>
#include <iostream>

namespace Poker {
	// evaluate

	void RuntimeEvaluator::process_cards()
	{
		for (int i = 0; i < 5; ++i) {
			hero.cards[i] = board[i];
			vill.cards[i] = board[i];
		}
		hero.cards[5] = hero.hand.get_primary();
		hero.cards[6] = hero.hand.get_secondary();
		vill.cards[5] = vill.hand.get_primary();
		vill.cards[6] = vill.hand.get_secondary();

		std::sort(hero.cards.rbegin(), hero.cards.rend());
		std::sort(vill.cards.rbegin(), vill.cards.rend());
	}

	void RuntimeEvaluator::process_flush()
	{
		for (int i = 0; i < 4; ++i) {
			hero.suits[i] = 0;
			vill.suits[i] = 0;
		}

		for (const Card& card : board) {
			++hero.suits[static_cast<int>(card.get_suit())];
		}

		vill.suits = hero.suits;

		++hero.suits[static_cast<int>(hero.hand.get_primary().get_suit())];
		++hero.suits[static_cast<int>(hero.hand.get_secondary().get_suit())];
		++vill.suits[static_cast<int>(vill.hand.get_primary().get_suit())];
		++vill.suits[static_cast<int>(vill.hand.get_secondary().get_suit())];

		hero.is_flush = false;
		vill.is_flush = false;
		for (int i = 0; i < 4; ++i) {
			if (hero.suits[i] >= 5) {
				hero.flush_suit = static_cast<CardSuit>(i);
				hero.is_flush = true;
			}
			if (vill.suits[i] >= 5) {
				vill.flush_suit = static_cast<CardSuit>(i);
				vill.is_flush = true;
			}
		}
	}

	void RuntimeEvaluator::process_straight(Props& props)
	{
		int counter = 0;
		int i = 0;
		for (; i < 3; ++i) {
			curr_rank = props.cards[i].get_rank();
			next_rank = props.cards[i + 1].get_rank();

			if (curr_rank == next_rank) {
				continue;
			}

			if (next_rank == curr_rank - 1) {
				++counter;
				if (counter == 1) props.straight_rank = curr_rank;
			}
			else {
				counter = 0;
			}
		}

		if (counter == 0 &&
			(props.cards.back().get_rank() != CardRank::C_2 || props.cards.front().get_rank() != CardRank::C_A))
		{
			props.is_straight = false;
			return;
		}

		for (; i < 6; ++i) {
			curr_rank = props.cards[i].get_rank();
			next_rank = props.cards[i + 1].get_rank();

			if (curr_rank == next_rank) {
				continue;
			}

			if (next_rank == curr_rank - 1) {
				++counter;
				if (counter == 4) {
					props.is_straight = true;
					return;
				}
			}
			else {
				props.is_straight = false;
				return;
			}
		}

		if (counter == 3 &&
			props.cards.back().get_rank() == CardRank::C_2 &&
			props.cards.front().get_rank() == CardRank::C_A)
		{
			props.straight_rank = CardRank::C_5;
			props.is_straight = true;
		}
		else {
			props.is_straight = false;
		}
	}

	void RuntimeEvaluator::process_ranks()
	{
		for (int i = 0; i < 13; ++i) {
			hero.ranks[i] = 0;
			vill.ranks[i] = 0;
		}

		for (const Card& card : board) {
			++hero.ranks[static_cast<int>(card.get_rank())];
		}

		vill.ranks = hero.ranks;

		++hero.ranks[static_cast<int>(hero.hand.get_primary().get_rank())];
		++hero.ranks[static_cast<int>(hero.hand.get_secondary().get_rank())];
		++vill.ranks[static_cast<int>(vill.hand.get_primary().get_rank())];
		++vill.ranks[static_cast<int>(vill.hand.get_secondary().get_rank())];
	}

	void RuntimeEvaluator::process_matches(Props& props)
	{
		props.pair_count = 0;
		props.trips_count = 0;
		props.is_quads = false;

		int count;
		for (int i = 12; i >= 0; --i) {
			count = props.ranks[i];
			if (count == 2) props.pairs[props.pair_count++] = static_cast<CardRank>(i);
			else if (count == 3) props.trips[props.trips_count++] = static_cast<CardRank>(i);
			else if (count == 4) {
				props.quads = static_cast<CardRank>(i);
				props.is_quads = true;
			}
		}
	}

	void RuntimeEvaluator::init_royal_flush(Props& props)
	{
		if (!props.is_flush || !props.is_straight || props.straight_rank != CardRank::C_A) {
			props.is_royal = false;
			return;
		}

		int counter = 0;
		for (auto& card : props.cards) {
			if (card.get_rank() < CardRank::C_T) {
				props.is_royal = false;
				break;
			}
			if (card.get_suit() == props.flush_suit) {
				++counter;
				if (counter == 5) {
					props.is_royal = true;
					break;
				}
			}
		}
	}

	void RuntimeEvaluator::init_straight_flush(Props& props)
	{
		if (!props.is_flush || !props.is_straight) {
			props.is_strf = false;
			return;
		}

		int counter = 0;
		for (int i = 0; i < 7; ++i) {
			curr_rank = props.cards[i].get_rank();
			if (curr_rank > props.straight_rank) {
				continue;
			}
			if (props.cards[i].get_suit() != props.flush_suit) {
				continue;
			}

			if (curr_rank != props.straight_rank - counter) {
				if (i > 2) {
					props.is_strf = false;
					return;
				}
				counter = 1;
				props.straight_rank = curr_rank;
			}
			else {
				++counter;
				if (counter == 5) break;
			}
		}

		if (counter == 4 && props.straight_rank == CardRank::C_5) {
			for (auto& card : props.cards) {
				if (card.get_rank() < CardRank::C_A) {
					props.is_strf = false;
					return;
				}
				if (card.get_rank() == CardRank::C_A && card.get_suit() == props.flush_suit) {
					props.is_strf = true;
					return;
				}
			}
		}

		props.is_strf = counter == 5;
	}

	void RuntimeEvaluator::init_quads(Props& props)
	{
		for (auto& card : props.cards) {
			curr_rank = card.get_rank();
			if (curr_rank != props.quads) {
				props.kickers[0] = card.get_rank();
				break;
			}
		}
	}

	void RuntimeEvaluator::init_full_house(Props& props)
	{
		if (props.trips_count >= 1) {
			if (props.pair_count >= 1) {
				props.is_full_house = true;
				return;
			}
			else if (props.trips_count == 2) {
				props.is_full_house = true;
				props.pairs[0] = props.trips[1];
				return;
			}
		}

		props.is_full_house = false;
	}

	void RuntimeEvaluator::init_flush(Props& props)
	{
		int counter = 0;
		for (auto& card : props.cards) {
			if (card.get_suit() == props.flush_suit) {
				props.kickers[counter] = card.get_rank();
				if (counter == 4) break;
				++counter;
			}
		}
	}

	void RuntimeEvaluator::init_trips(Props& props)
	{
		int counter = 0;
		for (auto& card : props.cards) {
			if (card.get_rank() != props.trips[0]) {
				props.kickers[counter] = card.get_rank();
				if (counter == 1) break;
				++counter;
			}
		}
	}

	void RuntimeEvaluator::init_two_pair(Props& props)
	{
		for (auto& card : props.cards) {
			curr_rank = card.get_rank();
			if (curr_rank != props.pairs[0] && curr_rank != props.pairs[1]) {
				props.kickers[0] = curr_rank;
				break;
			}
		}
	}

	void RuntimeEvaluator::init_pair(Props& props)
	{
		int counter = 0;
		for (auto& card : props.cards) {
			if (card.get_rank() != props.pairs[0]) {
				props.kickers[counter] = card.get_rank();
				if (counter == 2) break;
				++counter;
			}
		}
	}

	bool RuntimeEvaluator::check_royal()
	{
		init_royal_flush(hero);
		init_royal_flush(vill);

		if (hero.is_royal) {
			result = vill.is_royal ? Winner::SPLIT : Winner::HERO;
			return true;
		}
		else if (vill.is_royal) {
			result = Winner::VILL;
			return true;
		}

		return false;
	}

	bool RuntimeEvaluator::check_strf()
	{
		init_straight_flush(hero);
		init_straight_flush(vill);

		if (hero.is_strf) {
			if (!vill.is_strf || hero.straight_rank > vill.straight_rank) {
				result = Winner::HERO;
			}
			else {
				result = hero.straight_rank == vill.straight_rank ? Winner::SPLIT : Winner::VILL;
			}

			return true;
		}
		else if (vill.is_strf) {
			result = Winner::VILL;
			return true;
		}

		return false;
	}

	bool RuntimeEvaluator::check_quads()
	{
		if (hero.is_quads) {
			if (!vill.is_quads) {
				result = Winner::HERO;
			}
			else {
				init_quads(hero);
				init_quads(vill);

				if (hero.quads > vill.quads) result = Winner::HERO;
				else if (hero.quads < vill.quads) result = Winner::VILL;
				else if (hero.kickers[0] > vill.kickers[0]) result = Winner::HERO;
				else if (hero.kickers[0] < vill.kickers[0]) result = Winner::VILL;
				else result = Winner::SPLIT;
			}

			return true;
		}
		else if (vill.is_quads) {
			result = Winner::VILL;
			return true;
		}

		return false;
	}

	bool RuntimeEvaluator::check_full_house()
	{
		init_full_house(hero);
		init_full_house(vill);

		if (hero.is_full_house) {
			if (!vill.is_full_house) {
				result = Winner::HERO;
			}
			else {
				if (hero.trips[0] > vill.trips[0]) result = Winner::HERO;
				else if (hero.trips[0] < vill.trips[0]) result = Winner::VILL;
				else if (hero.pairs[0] > vill.pairs[0]) result = Winner::HERO;
				else if (hero.pairs[0] < vill.pairs[0]) result = Winner::VILL;
				else result = Winner::SPLIT;
			}

			return true;
		}
		else if (vill.is_full_house) {
			result = Winner::VILL;
			return true;
		}

		return false;
	}

	bool RuntimeEvaluator::check_flush()
	{
		if (hero.is_flush) {
			if (!vill.is_flush) {
				result = Winner::HERO;
			}
			else {
				init_flush(hero);
				init_flush(vill);

				for (int i = 0; i < 5; ++i) {
					if (hero.kickers[i] > vill.kickers[i]) {
						result = Winner::HERO;
						return true;
					}
					else if (hero.kickers[i] < vill.kickers[i]) {
						result = Winner::VILL;
						return true;
					}
				}

				result = Winner::SPLIT;
			}

			return true;
		}
		else if (vill.is_flush) {
			result = Winner::VILL;
			return true;
		}

		return false;
	}

	bool RuntimeEvaluator::check_straight()
	{
		if (hero.is_straight) {
			if (!vill.is_straight || hero.straight_rank > vill.straight_rank) {
				result = Winner::HERO;
			}
			else {
				result = hero.straight_rank == vill.straight_rank ? Winner::SPLIT : Winner::VILL;
			}

			return true;
		}
		else if (vill.is_straight) {
			result = Winner::VILL;
			return true;
		}

		return false;
	}

	bool RuntimeEvaluator::check_trips()
	{
		if (hero.trips_count >= 1) {
			if (vill.trips_count == 0) {
				result = Winner::HERO;
			}
			else {
				init_trips(hero);
				init_trips(vill);

				if (hero.trips[0] > vill.trips[0]) result = Winner::HERO;
				else if (hero.trips[0] < vill.trips[0]) result = Winner::VILL;
				else if (hero.kickers[0] > vill.kickers[0]) result = Winner::HERO;
				else if (hero.kickers[0] < vill.kickers[0]) result = Winner::VILL;
				else if (hero.kickers[1] > vill.kickers[1]) result = Winner::HERO;
				else if (hero.kickers[1] < vill.kickers[1]) result = Winner::VILL;
				else result = Winner::SPLIT;
			}

			return true;
		}
		else if (vill.trips_count >= 1) {
			result = Winner::VILL;
			return true;
		}

		return false;
	}

	bool RuntimeEvaluator::check_two_pair()
	{
		if (hero.pair_count >= 2) {
			if (vill.pair_count < 2) {
				result = Winner::HERO;
			}
			else {
				init_two_pair(hero);
				init_two_pair(vill);

				if (hero.pairs[0] > vill.pairs[0]) result = Winner::HERO;
				else if (hero.pairs[0] < vill.pairs[0]) result = Winner::VILL;
				else if (hero.pairs[1] > vill.pairs[1]) result = Winner::HERO;
				else if (hero.pairs[1] < vill.pairs[1]) result = Winner::VILL;
				else if (hero.kickers[0] > vill.kickers[0]) result = Winner::HERO;
				else if (hero.kickers[0] < vill.kickers[0]) result = Winner::VILL;
				else result = Winner::SPLIT;
			}

			return true;
		}
		else if (vill.pair_count >= 2) {
			result = Winner::VILL;
			return true;
		}

		return false;
	}

	bool RuntimeEvaluator::check_pair()
	{
		if (hero.pair_count == 1) {
			if (vill.pair_count == 0) {
				result = Winner::HERO;
			}
			else {
				init_pair(hero);
				init_pair(vill);

				if (hero.pairs[0] > vill.pairs[0]) result = Winner::HERO;
				else if (hero.pairs[0] < vill.pairs[0]) result = Winner::VILL;
				else if (hero.kickers[0] > vill.kickers[0]) result = Winner::HERO;
				else if (hero.kickers[0] < vill.kickers[0]) result = Winner::VILL;
				else if (hero.kickers[1] > vill.kickers[1]) result = Winner::HERO;
				else if (hero.kickers[1] < vill.kickers[1]) result = Winner::VILL;
				else if (hero.kickers[2] > vill.kickers[2]) result = Winner::HERO;
				else if (hero.kickers[2] < vill.kickers[2]) result = Winner::VILL;
				else result = Winner::SPLIT;
			}

			return true;
		}
		else if (vill.pair_count == 1) {
			result = Winner::VILL;
			return true;
		}

		return false;
	}

	void RuntimeEvaluator::check_high_card()
	{
		for (int i = 0; i < 5; ++i) {
			if (hero.cards[i] > vill.cards[i]) {
				result = Winner::HERO;
				return;
			}
			else if (hero.cards[i] < vill.cards[i]) {
				result = Winner::VILL;
				return;
			}
		}
		result = Winner::SPLIT;
	}

	Winner RuntimeEvaluator::evaluate()
	{
		process_cards();
		process_flush();
		process_straight(hero);
		process_straight(vill);

		if (check_royal()) return result;
		if (check_strf()) return result;

		process_ranks();
		process_matches(hero);
		process_matches(vill);

		if (check_quads()) return result;
		if (check_full_house()) return result;
		if (check_flush()) return result;
		if (check_straight()) return result;
		if (check_trips()) return result;
		if (check_two_pair()) return result;
		if (check_pair()) return result;
		check_high_card();

		return result;
	}
}
