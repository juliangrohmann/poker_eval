#include "equity.h"
#include <iostream>

int main() {
	Poker::CachedEquitySolver solver(false);

	for (int i = 0; i < 100; ++i) {
		std::cout << "Equity: " << solver.enumerate(Poker::PokerHand("Ac5c"), Poker::PokerHand("Td8h")) << std::endl;
	}

	return 0;
}