#pragma once

#include "evaluator.h"

#include <memory>
#include <array>

namespace Poker {
	enum class Winner {
		HERO, VILL, SPLIT
	};

	struct SlimBoard {
	public:
		SlimBoard() : m_size{ 0 } {}
		SlimBoard(SlimCard c_1, SlimCard c_2, SlimCard c_3, SlimCard c_4, SlimCard c_5) : board{ c_1, c_2, c_3, c_4, c_5 } {}

		SlimCard& operator[](char i) { return board[i]; }
		const SlimCard& operator[](char i) const { return board[i]; }
		char size() const { return m_size; }
		void add_card(SlimCard card) { board[m_size++] = card; }
		void pop_card() { --m_size; }

		std::array<SlimCard, 5>::const_iterator begin() const { return board.cbegin(); }
		std::array<SlimCard, 5>::const_iterator end() const { return board.cend(); }

	private:
		std::array<SlimCard, 5> board;
		char m_size;
	};

	class EquitySolver {
	public:
		//virtual double enumerate(const PokerHand& hero, const PokerRange& vill, const Board& board = Board()) = 0;
		//virtual double enumerate(PokerRange& hero, PokerRange& vill, const Board& board = Board()) = 0;

	protected:
		EquitySolver() = default;
		~EquitySolver() = default;

		void add_result(Winner winner);
		void add_result(int winner);
		double calc_equity() { return equity / count; }
		void reset() { equity = 0; count = 0; }

	private:
		double equity = 0;
		int count = 0;
	};

	class CachedEquitySolver : public EquitySolver {
	public:
		CachedEquitySolver(bool test);
		Winner test(const PokerHand& hero, const PokerHand& vill, const Board& board);
		double enumerate(const PokerHand& hero, const PokerHand& vill, const Board& board = Board());

	private:
		void cache_boards_r(int start_index);
		void cache_hands();

		CachedEvaluator evaluator;
		std::vector<BoardCache> all_boards;
		std::vector<HandCache> all_hands;
		std::array<char, 52> hand_ranks;

		BoardCache* board_cache = nullptr;
		HandCache* hero_cache = nullptr;
		HandCache* vill_cache = nullptr;

		SlimBoard board;
		std::array<char, 15> temp_ranks;
		int index = 0;
	};
}