#include "evaluator.h"

#include <algorithm>

namespace Poker {
	// CachedEvaluator

	void CachedEvaluator::process_flush(Props& props)
	{
		props.is_flush = board_cache->suit_count + props.cache->suits[board_cache->max_suit] >= 5;
	}

	void CachedEvaluator::process_straight(Props& props)
	{
		props.hand_ranks[0] = props.cache->hand.primary.rank;
		props.hand_ranks[1] = props.cache->hand.secondary.rank;

		props.straight_rank = board_cache->straights[props.hand_ranks[0] - 2][props.hand_ranks[1] - 2];
		props.is_straight = props.straight_rank != 1;
	}

	void CachedEvaluator::process_matches()
	{
		hero.match_counts = { 0 };
		vill.match_counts = { 0 };

		char count;
		char index;
		for (auto& entry : board_cache->ranks) {
			index = entry.first;

			count = hero.cache->ranks[index] + entry.second - 1;
			hero.matches[count][hero.match_counts[count]++] = index;

			count = vill.cache->ranks[index] + entry.second - 1;
			vill.matches[count][vill.match_counts[count]++] = index;
		}
	}

	void CachedEvaluator::init_royal_flush(Props& props)
	{
		if (!props.is_flush || !props.is_straight) {
			props.is_royal = false;
			return;
		}

		props.cards = {
			props.cache->hand.primary, props.cache->hand.secondary,
			board_cache->board[0], board_cache->board[1], board_cache->board[2],
			board_cache->board[3], board_cache->board[4]
		};
		std::sort(props.cards.rbegin(), props.cards.rend());

		if (props.straight_rank != 14) {
			props.is_royal = false;
			return;
		}

		int counter = 0;
		for (auto& card : props.cards) {
			if (card.rank < 10) {
				props.is_royal = false;
				break;
			}
			if (card.suit == board_cache->max_suit) {
				++counter;
			}
		}

		props.is_royal = counter == 5;
	}

	void CachedEvaluator::init_straight_flush(Props& props)
	{
		if (!props.is_flush || !props.is_straight) {
			props.is_strf = false;
			return;
		}

		char curr_rank;
		int counter = 0;
		for (int i = 0; i < 7; ++i) {
			curr_rank = props.cards[i].rank;
			if (curr_rank > props.straight_rank) {
				continue;
			}
			if (props.cards[i].suit != board_cache->max_suit) {
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

		if (counter == 4 && props.straight_rank == 5) {
			for (auto& card : props.cards) {
				if (card.rank < 14) {
					props.is_strf = false;
					return;
				}
				if (card.rank == 14 && card.suit == board_cache->max_suit) {
					props.is_strf = true;
					return;
				}
			}
		}

		props.is_strf = counter == 5;
	}

	void CachedEvaluator::init_quads()
	{
		for (auto& card : board_cache->board) {
			if (card.rank != hero.matches[3][0]) {
				min_ranks[0] = card.rank + 1;
				break;
			}
		}
	}

	void CachedEvaluator::init_flush()
	{
		int index = 5 - board_cache->suit_count;
		min_ranks[0] = 1;

		for (int i = 4; index < 1; --i) {
			if (board_cache->board[i].suit == board_cache->max_suit) {
				min_ranks[index] = board_cache->board[i].rank;
				break;
			}
		}
	}

	void CachedEvaluator::init_trips()
	{
		int counter = -hero.cache->ranks[hero.matches[2][0]];

		for (int i = 4; counter < 2; --i) {
			if (board_cache->board[i].rank != hero.matches[2][0]) {
				if (counter >= 0) {
					min_ranks[counter] = board_cache->board[i].rank;
				}
				++counter;
			}
		}
	}

	void CachedEvaluator::init_two_pair()
	{
		int i = 0;
		for (; board_cache->board[i].rank == hero.matches[1][0] || board_cache->board[i].rank == hero.matches[1][1]; ++i) {}
		min_ranks[0] = board_cache->board[i].rank;
	}

	void CachedEvaluator::init_pair()
	{
		//int hand_count = -hero.ranks[static_cast<int>(hero.matches[1][0])];

		//if (hand_count == -2) {
		//	return;
		//}

		//min_ranks[0] = CardRank::PLACEHOLDER;
		//min_ranks[1] = CardRank::PLACEHOLDER;

		//for (int i = 4; hand_count < 2; --i) {
		//	if (board_cache->board[i].get_rank() != hero.matches[1][0]) {
		//		if (hand_count >= 0) {
		//			min_ranks[hand_count] = board_cache->board[i].get_rank();
		//		}
		//		++hand_count;
		//	}
		//}

		min_ranks[0] = hero.matches[0][hero.match_counts[0] - 1];
		min_ranks[1] = hero.matches[0][hero.match_counts[0] - 2];
	}

	bool CachedEvaluator::check_royal()
	{
		init_royal_flush(hero);
		init_royal_flush(vill);

		if (!hero.is_royal && !vill.is_royal) {
			return false;
		}

		result = hero.is_royal - vill.is_royal;

		return true;
	}

	bool CachedEvaluator::check_strf()
	{
		init_straight_flush(hero);
		init_straight_flush(vill);

		if (!hero.is_strf && !vill.is_strf) {
			return false;
		}

		result = hero.is_strf * hero.straight_rank - vill.is_strf * vill.straight_rank;

		return true;
	}

	bool CachedEvaluator::check_quads()
	{
		if (!hero.match_counts[3] && !vill.match_counts[3]) {
			return false;
		}

		result = hero.match_counts[3] * hero.matches[3][0] - vill.match_counts[3] * vill.matches[3][0];

		if (result == 0) {
			init_quads();

			result = (hero.hand_ranks[0] / min_ranks[0]) * static_cast<int>(hero.hand_ranks[0])
				- (vill.hand_ranks[0] / min_ranks[0]) * static_cast<int>(vill.hand_ranks[0]);
		}

		return true;
	}

	bool CachedEvaluator::check_full_house()
	{
		hero.is_full_house = (hero.match_counts[2] == 1 && hero.match_counts[1] >= 1) || hero.match_counts[2] == 2;
		vill.is_full_house = (vill.match_counts[2] == 1 && vill.match_counts[1] >= 1) || vill.match_counts[2] == 2;

		if (!hero.is_full_house && !vill.is_full_house) {
			return false;
		}

		result = hero.is_full_house * hero.matches[2][0] - vill.is_full_house * vill.matches[2][0];
		if (result != 0) return true;

		if (hero.match_counts[2] == 2) hero.matches[1][0] = hero.matches[2][1];
		if (vill.match_counts[2] == 2) vill.matches[1][0] = vill.matches[2][1];
		result = hero.matches[1][0] - vill.matches[1][0];

		return true;
	}

	bool CachedEvaluator::check_flush()
	{
		if (!hero.is_flush && !vill.is_flush) {
			return false;
		}

		result = hero.is_flush - vill.is_flush;
		if (result != 0) return true;

		init_flush();

		int index_h = (hero.cache->hand.primary.suit != board_cache->max_suit) << (hero.cache->hand.secondary.suit != board_cache->max_suit);
		int index_v = (vill.cache->hand.primary.suit != board_cache->max_suit) << (vill.cache->hand.secondary.suit != board_cache->max_suit);

		result = (vill.hand_ranks[index_v] / min_ranks[0]) * static_cast<int>(vill.hand_ranks[index_v]) * (index_v - 2)
			- (hero.hand_ranks[index_h] / min_ranks[0]) * static_cast<int>(hero.hand_ranks[index_h]) * (index_h - 2);

		return true;
	}

	bool CachedEvaluator::check_straight()
	{
		if (!hero.is_straight && !vill.is_straight) {
			return false;
		}

		result = hero.is_straight * hero.straight_rank - vill.is_straight * vill.straight_rank;

		return true;
	}

	bool CachedEvaluator::check_trips()
	{
		if (!hero.match_counts[2] & !vill.match_counts[2]) {
			return false;
		}

		result = hero.match_counts[2] * hero.matches[2][0] - vill.match_counts[2] * vill.matches[2][0];

		if (result == 0) {
			init_trips();

			int index = 0;
			do {
				result = (hero.hand_ranks[index] / min_ranks[index]) * static_cast<int>(hero.hand_ranks[index])
					   - (vill.hand_ranks[index] / min_ranks[index]) * static_cast<int>(vill.hand_ranks[index]);
				++index;
			} while (result == 0 && index < 2);
		}

		return true;
	}

	bool CachedEvaluator::check_two_pair()
	{
		if (hero.match_counts[1] < 2 && vill.match_counts[1] < 2) {
			return false;
		}

		result = (hero.match_counts[1] / 2) * hero.matches[1][0] - (vill.match_counts[1] / 2) * vill.matches[1][0];
		if (result != 0) return true;

		result = (hero.match_counts[1] / 2) * hero.matches[1][1] - (vill.match_counts[1] / 2) * vill.matches[1][1];
		if (result != 0) return true;

		init_two_pair();

		int index_h = hero.hand_ranks[0] == hero.matches[1][0] || hero.hand_ranks[0] == hero.matches[1][1];
		int index_v = vill.hand_ranks[0] == vill.matches[1][0] || vill.hand_ranks[0] == vill.matches[1][1];
		result = (hero.hand_ranks[index_h] / min_ranks[0]) * static_cast<int>(hero.hand_ranks[index_h])
			- (vill.hand_ranks[index_v] / min_ranks[0]) * static_cast<int>(vill.hand_ranks[index_v]);

		return true;
	}

	bool CachedEvaluator::check_pair()
	{
		if (!hero.match_counts[1] & !vill.match_counts[1]) {
			return false;
		}

		result = hero.match_counts[1] * hero.matches[1][0] - vill.match_counts[1] * vill.matches[1][0];

		if (result == 0) {
			init_pair();

			int index = 0;
			do {
				result = (hero.hand_ranks[index] / min_ranks[index]) * static_cast<int>(hero.hand_ranks[index])
					- (vill.hand_ranks[index] / min_ranks[index]) * static_cast<int>(vill.hand_ranks[index]);
				++index;
			} while (result == 0 && index < 2);
		}

		return true;
	}

	void CachedEvaluator::check_high_card()
	{
		char min_rank_1 = board_cache->board[4].rank;
		char min_rank_2 = board_cache->board[3].rank;

		int index = 0;
		do {
			result = (hero.hand_ranks[index] / min_ranks[index]) * static_cast<int>(hero.hand_ranks[index])
				- (vill.hand_ranks[index] / min_ranks[index]) * static_cast<int>(vill.hand_ranks[index]);
			++index;
		} while (result == 0 && index < 2);
	}

	int CachedEvaluator::evaluate()
	{
		process_flush(hero);
		process_flush(vill);

		process_straight(hero);
		process_straight(vill);

		if (check_royal()) return result;
		if (check_strf()) return result;

		process_matches();

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
