#include "equity.h"

#include <algorithm>
#include <iostream>
#include <memory>

namespace Poker {
	// utility

	bool strict_equal(const Card& card1, const Card& card2)
	{
		return card1 == card2 && card1.get_suit() == card2.get_suit();
	}

	bool strict_equal(const SlimCard& card1, const SlimCard& card2)
	{
		return card1.rank == card2.rank && card1.suit == card2.suit;
	}

	bool is_taken_board(const Card& card, const Board& board)
	{
		for (const Card& board_card : board) {
			if (strict_equal(card, board_card)) return true;
		}

		return false;
	}

	bool is_taken_board(const SlimCard& card, const SlimBoard& board)
	{
		for (char i = 0; i < board.size(); ++i) {
			if (strict_equal(card, board[i])) return true;
		}

		return false;
	}

	bool is_taken(const Card& card, const PokerHand& hero, const PokerHand& vill, const Board& board)
	{
		return	strict_equal(card, hero.get_primary()) || strict_equal(card, hero.get_secondary()) ||
			strict_equal(card, vill.get_primary()) || strict_equal(card, vill.get_secondary()) ||
			is_taken_board(card, board);
	}

	bool is_taken(const SlimCard& card, const SlimHand& hero, const SlimHand& vill, const SlimBoard& board)
	{
		return	strict_equal(card, hero.primary) || strict_equal(card, hero.secondary) ||
			strict_equal(card, vill.primary) || strict_equal(card, vill.secondary) ||
			is_taken_board(card, board);
	}


	int card_to_index(const Card& card) {
		return (static_cast<int>(card.get_rank()) - 2) * 4 + static_cast<int>(card.get_suit());
	}

	Card index_to_card(int index) {
		return Card(static_cast<CardRank>((index / 4) + 2), static_cast<CardSuit>(index % 4));
	}

	char slim_card_to_index(const SlimCard& card) {
		return (card.rank - 2) * 4 + card.suit;
	}

	SlimCard index_to_slim_card(char index) {
		return SlimCard{ (index / 4) + 2 , index % 4 };
	}

	bool is_valid(const Board& board, const std::array<int, 52>& hand_ranks)
	{
		return !(hand_ranks[card_to_index(board[0])]
			+ hand_ranks[card_to_index(board[1])]
			+ hand_ranks[card_to_index(board[2])]
			+ hand_ranks[card_to_index(board[3])]
			+ hand_ranks[card_to_index(board[4])]);
	}

	bool is_valid(const std::array<SlimCard, 5> board, const std::array<char, 52>& hand_ranks)
	{
		return !(hand_ranks[slim_card_to_index(board[0])]
			+ hand_ranks[slim_card_to_index(board[1])]
			+ hand_ranks[slim_card_to_index(board[2])]
			+ hand_ranks[slim_card_to_index(board[3])]
			+ hand_ranks[slim_card_to_index(board[4])]);
	}

	void set_hand_ranks(const SlimHand& hero, const SlimHand& vill, std::array<char, 52>& ranks) {
		ranks = { 0 };
		++ranks[slim_card_to_index(hero.primary)];
		++ranks[slim_card_to_index(hero.secondary)];
		++ranks[slim_card_to_index(vill.primary)];
		++ranks[slim_card_to_index(vill.secondary)];
	}


	// EquitySolver

	void EquitySolver::add_result(Winner winner) {
		switch (winner) {
		case Winner::HERO: equity += 1.0; break;
		case Winner::SPLIT: equity += 0.5; break;
		}
		++count;
	}


	// CachedEquitySolver

	CachedEquitySolver::CachedEquitySolver(bool test) : evaluator{ CachedEvaluator(hero_cache, vill_cache, board_cache) }, all_boards{ test ? 0 : 2'598'960 }, all_hands{ 1326 }
	{
		if (!test) {
			cache_boards_r(0);
		}

		cache_hands();
	}


	// CachedEquitySolver, board_cache

	char is_straight(const std::array<SlimCard, 5>& board, char primary, char secondary)
	{
		std::array<char, 7> ranks = {
			primary, secondary,
			board[0].rank, board[1].rank, board[2].rank, board[3].rank, board[4].rank };
		std::sort(ranks.rbegin(), ranks.rend());

		int counter = 0;
		char curr_rank;
		char top_rank = 1;

		char i = 0;
		for (; i < 3; ++i) {
			curr_rank = ranks[i];

			if (curr_rank == ranks[i + 1]) {
				continue;
			}

			if (ranks[i + 1] == curr_rank - 1) {
				++counter;
				if (counter == 1) top_rank = curr_rank;
			}
			else {
				counter = 0;
			}
		}

		if (counter == 0 &&
			(ranks.back() != 2 || ranks.front() != 14))
		{
			return 1;
		}

		for (; i < 6; ++i) {
			curr_rank = ranks[i];

			if (curr_rank == ranks[i + 1]) {
				continue;
			}

			if (ranks[i + 1] == curr_rank - 1) {
				++counter;
				if (counter == 4) return top_rank;
			}
			else {
				return 1;
			}
		}

		if (counter == 3 &&
			ranks.back() == 2 &&
			ranks.front() == 14)
		{
			return 5;
		}

		return 1;
	}

	void cache_board_straights(BoardCache& cache)
	{
		for (char c_1 = 14; c_1 >= 2; --c_1) {
			for (char c_2 = c_1; c_2 >= 2; --c_2) {
				cache.straights[c_1 - 2][c_2 - 2] = is_straight(cache.board, c_1, c_2);
			}
		}
	}

	void cache_board_ranks(BoardCache& cache, std::array<char, 15>& temp_ranks)
	{
		temp_ranks = { 0 };
		for (auto& card : cache.board) {
			++temp_ranks[static_cast<int>(card.rank)];
		}

		cache.ranks.reserve(5);
		for (char i = 14; i >= 2; --i) {
			if (temp_ranks[i] > 0) {
				cache.ranks.emplace_back(i, temp_ranks[i]);
			}
		}
		cache.ranks.shrink_to_fit();
	}

	void cache_board_suits(BoardCache& cache)
	{
		for (auto& card : cache.board) {
			++cache.suits[static_cast<int>(card.suit)];
		}

		for (char i = 0; i < 4; ++i) {
			if (cache.suits[i] >= 3) {
				cache.max_suit = i;
				cache.suit_count = cache.suits[i];
				break;
			}
		}
	}

	void fill_board_cache(BoardCache& cache, std::array<char, 15>& temp_ranks)
	{
		cache_board_straights(cache);
		cache_board_ranks(cache, temp_ranks);
		cache_board_suits(cache);
		std::sort(cache.board.rbegin(), cache.board.rend());
	}

	void CachedEquitySolver::cache_boards_r(int start_index)
	{
		if (board.size() == 5) {
			all_boards[index].board = { board[0], board[1], board[2], board[3], board[4] };
			fill_board_cache(all_boards[index], temp_ranks);
			++index;
		}
		else {
			SlimCard card;
			for (char i = start_index; i < 52; i++) {
				card = { (i / 4) + 2, i % 4 };
				if (is_taken_board(card, board)) {
					continue;
				}

				board.add_card(card);
				cache_boards_r(i + 1);
				board.pop_card();
			}
		}
	}


	// CachedEquitySolver, hand_cache

	int arith_series(int n) {
		return n * (n + 1) / 2;
	}

	int hand_to_index(const PokerHand& hand)
	{
		return arith_series(card_to_index(hand.get_primary()) - 1) + card_to_index(hand.get_secondary());

	}
	int hand_to_index(const SlimHand& hand)
	{
		return arith_series(slim_card_to_index(hand.primary) - 1) + slim_card_to_index(hand.secondary);
	}


	void cache_hand_ranks(HandCache& cache)
	{
		++cache.ranks[cache.hand.primary.rank];
		++cache.ranks[cache.hand.secondary.rank];
	}

	void cache_hand_suits(HandCache& cache)
	{
		++cache.suits[cache.hand.primary.suit];
		++cache.suits[cache.hand.secondary.suit];
	}

	void fill_hand_cache(HandCache& cache)
	{
		cache_hand_ranks(cache);
		cache_hand_suits(cache);
	}

	void CachedEquitySolver::cache_hands()
	{
		SlimHand hand;
		int index;
		for (char c_1 = 1; c_1 < 52; ++c_1) {
			for (char c_2 = 0; c_2 < c_1; ++c_2) {
				hand = SlimHand{ index_to_slim_card(c_1), index_to_slim_card(c_2) };
				index = hand_to_index(hand);
				all_hands[index].hand = hand;
				fill_hand_cache(all_hands[index]);
			}
		}
	}


	// CachedEquitySolver, enumerate

	Winner to_winner(int result) {
		if (result > 0) return Winner::HERO;
		else if (result < 0) return Winner::VILL;
		else return Winner::SPLIT;
	}

	double CachedEquitySolver::enumerate(const PokerHand& hero, const PokerHand& vill, const Board& board)
	{
		Board runtime_board;
		reset();
		hero_cache = &all_hands[hand_to_index(hero)];
		vill_cache = &all_hands[hand_to_index(vill)];
		set_hand_ranks(hero_cache->hand, vill_cache->hand, hand_ranks);

		BoardCache* end = &all_boards.back() + 1;
		for (board_cache = &all_boards[0]; board_cache != end; ++board_cache) {
			if (!is_valid(board_cache->board, hand_ranks)) continue;

			Winner cached = to_winner(evaluator.evaluate());
			add_result(cached);
		}

		return calc_equity();
	}
}
