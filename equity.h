#pragma once

#include "poker_game.h"
#include "evaluator.h"

#include <memory>
#include <array>

namespace Poker {
	class EquitySolver {
	public:
		//virtual double enumerate(const PokerHand& hero, const PokerRange& vill, const Board& board = Board()) = 0;
		//virtual double enumerate(PokerRange& hero, PokerRange& vill, const Board& board = Board()) = 0;

	protected:
		EquitySolver() = default;
		~EquitySolver() = default;

		void add_result(Winner winner);
		double calc_equity() { return equity / count; }
		void reset() { equity = 0; count = 0; }

		const Board& board() { return m_board; }
		void add_card(const Card& card) { m_board.add_card(card); }
		void pop_card() { m_board.pop_card(); }

		const PokerHand& hero() { return m_hero; }
		const PokerHand& vill() { return m_vill; }
		void set_hero(const PokerHand& hand) { m_hero = hand; }
		void set_vill(const PokerHand& hand) { m_vill = hand; }

	private:
		PokerHand m_hero;
		PokerHand m_vill;
		Board m_board;
		double equity = 0;
		int count = 0;
	};

	class RuntimeEquitySolver : public EquitySolver {
	public:
		RuntimeEquitySolver() : evaluator{ RuntimeEvaluator(hero(), vill(), board()) } {}

		double enumerate(const PokerHand& hero, const PokerHand& vill, const Board& board = Board());

	private:
		void enumerate_r(int start_index);

		RuntimeEvaluator evaluator;
	};
}