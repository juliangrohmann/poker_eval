#pragma once

#include <string>
#include <vector>
#include <memory>


namespace Poker {
	// CardSuit

	enum class CardSuit : char {
		CLUB = 0,
		DIAMOND = 1,
		HEART = 2,
		SPADE = 3,
		PLACEHOLDER = 4
	};

	char suit_to_repr(CardSuit rank);
	CardSuit repr_to_suit(char repr);

	// CardRank

	enum class CardRank : char {
		PLACEHOLDER = 1,
		C_2 = 2,
		C_3 = 3,
		C_4 = 4,
		C_5 = 5,
		C_6 = 6,
		C_7 = 7,
		C_8 = 8,
		C_9 = 9,
		C_T = 10,
		C_J = 11,
		C_Q = 12,
		C_K = 13,
		C_A = 14,
	};

	inline CardRank operator+(CardRank rank, int num) { return static_cast<CardRank>(static_cast<char>(rank) + num); }
	inline CardRank operator-(CardRank rank, int num) { return static_cast<CardRank>(static_cast<char>(rank) - num); }
	inline CardRank& operator+=(CardRank& rank, int num) { rank = rank + num; return rank; }
	inline CardRank& operator-=(CardRank& rank, int num) { rank = rank - num; return rank; }
	inline CardRank& operator++(CardRank& rank) { return rank += 1; }
	inline CardRank& operator--(CardRank& rank) { return rank -= 1; }

	inline bool operator<(CardRank hero, CardRank vill) { return static_cast<int>(hero) < static_cast<int>(vill); }
	inline bool operator>(CardRank hero, CardRank vill) { return vill < hero; }
	inline bool operator==(CardRank hero, CardRank vill) { return !(hero < vill) && !(hero > vill); }
	inline bool operator!=(CardRank hero, CardRank vill) { return !(hero == vill); }
	inline bool operator>=(CardRank hero, CardRank vill) { return hero > vill || hero == vill; }
	inline bool operator<=(CardRank hero, CardRank vill) { return hero < vill || hero == vill; }

	char rank_to_repr(CardRank rank);
	CardRank repr_to_rank(char repr);


	// Card

	class Card {
	public:
		Card() : rank{ CardRank::PLACEHOLDER }, suit{ CardSuit::PLACEHOLDER } {}
		Card(CardRank rank, CardSuit suit) : rank{ rank }, suit{ suit } {}
		Card(std::string card_str) : rank{ repr_to_rank(card_str[0]) }, suit{ repr_to_suit(card_str[1]) } {}

		CardRank get_rank() const { return rank; }
		CardSuit get_suit() const { return suit; }
		std::string repr() const { return std::string{ rank_to_repr(rank), suit_to_repr(suit) }; }

		const Card& operator++() { ++rank; }
		const Card& operator--() { --rank; }

	private:
		CardRank rank;
		CardSuit suit;
	};

	inline bool operator<(Card hero, Card vill) { return hero.get_rank() < vill.get_rank(); }
	inline bool operator>(Card hero, Card vill) { return vill < hero; }
	inline bool operator==(Card hero, Card vill) { return !(hero < vill) && !(hero > vill); }
	inline bool operator!=(Card hero, Card vill) { return !(hero == vill); }
	inline bool operator>=(Card hero, Card vill) { return hero > vill || hero == vill; }
	inline bool operator<=(Card hero, Card vill) { return hero < vill || hero == vill; }


	// PokerHand

	class PokerHand {
	public:
		PokerHand() {}
		PokerHand(Card card1, Card card2) : primary{ card1 }, secondary(card2) {}
		PokerHand(CardRank rank1, CardSuit suit1, CardRank rank2, CardSuit suit2)
			: primary{ Card(rank1, suit1) }, secondary{ Card(rank2, suit2) } {}
		PokerHand(std::string hand_str) : PokerHand(Card(hand_str.substr(0, 2)), Card(hand_str.substr(2, 2))) {}

		const Card& get_primary() const { return primary; }
		const Card& get_secondary() const { return secondary; }
		bool is_suited() const { return primary.get_suit() == secondary.get_suit(); }
		bool is_pp() const { return primary.get_rank() == secondary.get_rank(); }
		std::string repr() const { return primary.repr() + secondary.repr(); }

	private:
		Card primary;
		Card secondary;
	};


	// Street

	enum class Street {
		PREFLOP = 0, FLOP = 3, TURN = 4, RIVER = 5
	};


	// Board

	class Board {
	public:
		Board() { cards.reserve(5); }
		Board(const std::vector<Card>& cards) : cards{ cards } {}
		Board(const std::vector<Card>&& cards) : cards{ cards } {}
		Board(std::string board_str);

		void add_card(const Card& card) { cards.push_back(card); }
		void pop_card() { cards.pop_back(); }
		Street street() const { return static_cast<Street>(cards.size()); }
		int count(CardRank rank) const;
		int count(CardSuit rank) const;
		std::string repr() const;

		const Card& operator[](int i) const { return cards[i]; }
		Card& operator[](int i) { return cards[i]; }

		std::vector<Card>::iterator begin() { return cards.begin(); }
		std::vector<Card>::iterator end() { return cards.end(); }
		std::vector<Card>::const_iterator begin() const { return cards.cbegin(); }
		std::vector<Card>::const_iterator end() const { return cards.cend(); }
		std::vector<Card>::reverse_iterator rbegin() { return cards.rbegin(); }
		std::vector<Card>::reverse_iterator rend() { return cards.rend(); }
		std::vector<Card>::const_reverse_iterator rbegin() const { return cards.rbegin(); }
		std::vector<Card>::const_reverse_iterator rend() const { return cards.rend(); }

	private:
		std::vector<Card> cards;
	};
}
