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

	bool is_taken_board(const Card& card, const Board& board)
	{
		for (const Card& board_card : board) {
			if (strict_equal(card, board_card)) return true;
		}

		return false;
	}

	bool is_taken(const Card& card, const PokerHand& hero, const PokerHand& vill, const Board& board)
	{
		return	strict_equal(card, hero.get_primary()) || strict_equal(card, hero.get_secondary()) ||
			strict_equal(card, vill.get_primary()) || strict_equal(card, vill.get_secondary()) ||
			is_taken_board(card, board);
	}


	// EquitySolver

	void EquitySolver::add_result(Winner winner) {
		switch (winner) {
		case Winner::HERO: equity += 1.0; break;
		case Winner::SPLIT: equity += 0.5; break;
		}
		++count;
	}

	// RuntimeEquitySolver

	void RuntimeEquitySolver::enumerate_r(int start_index)
	{
		if (board().street() == Street::RIVER) {
			add_result(evaluator.evaluate());
		}
		else {
			Card card;
			for (int i = start_index; i < 52; i++) {
				card = Card(static_cast<CardRank>(i / 4), static_cast<CardSuit>(i % 4));
				if (is_taken(card, hero(), vill(), board())) {
					continue;
				}
				add_card(card);
				enumerate_r(i + 1);
				pop_card();
			}
		}
	}

	double RuntimeEquitySolver::enumerate(const PokerHand& hero, const PokerHand& vill, const Board& board)
	{
		reset();
		set_hero(hero);
		set_vill(vill);

		enumerate_r(0);
		return calc_equity();
	}
}
