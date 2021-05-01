#include "poker_game.h"

#include <stdexcept>
#include <list>
#include <iostream>
#include <algorithm>
#include <functional>

using std::cout;
using std::endl;

namespace Poker {

	// CardSuit

	CardSuit repr_to_suit(char repr)
	{
		// TODO: Add stacktrace to exception

		switch (repr) {
		case 'c': return CardSuit::CLUB;
		case 'd': return CardSuit::DIAMOND;
		case 'h': return CardSuit::HEART;
		case 's': return CardSuit::SPADE;
		default: return CardSuit::PLACEHOLDER;
		}
	}

	char suit_to_repr(CardSuit suit)
	{
		// TODO: Add stacktrace to exception

		switch (suit) {
		case CardSuit::CLUB: return 'c';
		case CardSuit::DIAMOND: return 'd';
		case CardSuit::HEART: return 'h';
		case CardSuit::SPADE: return 's';
		default: return 'x';
		}
	}


	// CardRank

	CardRank repr_to_rank(char repr)
	{
		// TODO: Add stacktrace to exception

		switch (repr) {
		case '2': return CardRank::C_2;
		case '3': return CardRank::C_3;
		case '4': return CardRank::C_4;
		case '5': return CardRank::C_5;
		case '6': return CardRank::C_6;
		case '7': return CardRank::C_7;
		case '8': return CardRank::C_8;
		case '9': return CardRank::C_9;
		case 'T': return CardRank::C_T;
		case 'J': return CardRank::C_J;
		case 'Q': return CardRank::C_Q;
		case 'K': return CardRank::C_K;
		case 'A': return CardRank::C_A;
		default: return CardRank::PLACEHOLDER;
		}
	}

	char rank_to_repr(CardRank rank)
	{
		// TODO: Add stacktrace to exception

		switch (rank) {
		case CardRank::C_2: return '2';
		case CardRank::C_3: return '3';
		case CardRank::C_4: return '4';
		case CardRank::C_5: return '5';
		case CardRank::C_6: return '6';
		case CardRank::C_7: return '7';
		case CardRank::C_8: return '8';
		case CardRank::C_9: return '9';
		case CardRank::C_T: return 'T';
		case CardRank::C_J: return 'J';
		case CardRank::C_Q: return 'Q';
		case CardRank::C_K: return 'K';
		case CardRank::C_A: return 'A';
		default: return 'X';
		}
	}


	// Board

	Board::Board(std::string board_str)
	{
		cards.reserve(5);

		for (unsigned int i = 0; i < board_str.size(); i += 2) {
			cards.push_back(Card(board_str.substr(i, 2)));
		}
	}

	int Board::count(CardRank rank) const
	{
		int count = 0;
		for (const Card& c : cards) {
			if (c.get_rank() == rank) {
				count++;
			}
		}

		return count;
	}

	int Board::count(CardSuit suit) const
	{
		int count = 0;
		for (const Card& c : cards) {
			if (c.get_suit() == suit) {
				count++;
			}
		}

		return count;
	}

	std::string Board::repr() const
	{
		std::string board_str = "";
		for (const Card& c : cards) {
			board_str += c.repr();
		}
		return board_str;
	}
}
