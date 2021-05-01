#include "equity.h"
#include <iostream>

int main() {
	Poker::RuntimeEquitySolver solver{};
	std::cout << "Equity: " << solver.enumerate(Poker::PokerHand("Ac5c"), Poker::PokerHand("Td8h")) << std::endl;

	return 0;
}