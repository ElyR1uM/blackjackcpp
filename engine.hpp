#ifndef ENGINE_H
#define ENGINE_H

#include <string_view>
#include <vector>

struct Card {
  const std::string_view name;
  const int cardValue;
};

const Card cards[] = {
  {"Ace", 11}, {"Two", 2},  {"Three", 3}, {"Four", 4},
  {"Five", 5}, {"Six", 6},  {"Seven", 7}, {"Eight", 8},
  {"Nine", 9}, {"Ten", 10}, {"Jack", 10}, {"Queen", 10},
  {"King", 10}
};

namespace UI {
struct StaticText {
  static constexpr std::string_view dealer{"Dealer"};
  static constexpr std::string_view player{"Player"};
  static constexpr std::string_view score{"Score:  "};
};

struct EventMessages {
  static constexpr std::string_view bust{"BUST!"};
  static constexpr std::string_view win{"You win!"};
  static constexpr std::string_view lose{"You lose!"};
  static constexpr std::string_view push{"Push!"};
  static constexpr std::string_view dealerStands{"Dealer stands"};
  static constexpr std::string_view dealerBust{"Dealer busted!"};
  static constexpr std::string_view yourTurn{"Your turn..."};
  static constexpr std::string_view rematchPrompt{"Want to play again?"};
};

struct controlPrompts {
  static constexpr std::string_view hit{"1. [H]it"};
  static constexpr std::string_view stand{"2. [S]tand"};
  static constexpr std::string_view yes{"1. [Y]es"};
  static constexpr std::string_view no{"2. [N]o"};
};

std::string displayScore(const std::vector<int>& hand, bool hidden);
}

#define BOARD_WIDTH 48
#define BOARD_HEIGHT 32

int drawCard();
int calculateScore(const std::vector<int>& hand);

#endif
