#include "engine.hpp"
#include <random>
#include <sstream>
#include <iomanip>

// RNG seeding
namespace {
std::mt19937 mt{};
bool seeded{false};
void initRandom() {
  if (seeded)
    return;
  mt.seed(std::random_device{}());
  seeded = true;
}
}

namespace UI {
std::string displayScore(const std::vector<int>& hand, bool hidden) {
  int score{::calculateScore(hand)};
  std::ostringstream oss;
  oss << StaticText::score;

  if (hidden) {
    oss << "??";
  } else {
    oss << std::setw(2) << std::setfill('0') << score;
  }
  return oss.str();
}
}

int drawCard() {
  initRandom();
  std::uniform_int_distribution<> drawnCard{0, 12};
  int index{drawnCard(mt) % 13};
  return cards[index].cardValue;
}

int calculateScore(const std::vector<int>& hand) {
  int score{};
  int aceCount{};

  for (int card : hand) {
    score += card;
    if (card == 11)
      aceCount++;
  }
 
  // If 2 Aces are drawn
  while (score > 21 && aceCount > 0) {
    score -= 10;
    aceCount--;
  }
  return score;
}
