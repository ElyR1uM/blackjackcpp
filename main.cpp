#include "engine.hpp"
#include <ncurses.h>
#include <vector>

namespace {
WindowConfig config;
}

void setup() {
  initscr();
  clear();
  noecho();
  raw();
  start_color();
  curs_set(0);
  refresh();

  int rows{};
  int columns{};
  getmaxyx(stdscr, rows, columns);

  config.BoardwindowPositionY = rows / 2 - BOARD_HEIGHT / 2;
  config.BoardwindowPositionX = columns / 2 - BOARD_WIDTH / 2;
  config.verticallyCentered = config.BoardwindowPositionY + (BOARD_HEIGHT / 2);
}

void drawText(int y, int width, const std::string_view& text) {
  mvaddstr(
    y,
    config.BoardwindowPositionX + (width - text.length()) / 2,
    text.data()
  );
  refresh();
}

void setupDrawUI() {
  drawText(
    config.BoardwindowPositionY - 1,
    BOARD_WIDTH,
    UI::StaticText::dealer
  );
  drawText(
    config.BoardwindowPositionY + BOARD_HEIGHT,
    BOARD_WIDTH,
    UI::StaticText::player
  );
  drawText(
    config.BoardwindowPositionY + BOARD_HEIGHT - 2,
    BOARD_WIDTH,
    UI::EventMessages::yourTurn
  );
  mvaddstr(
    config.BoardwindowPositionY + BOARD_HEIGHT + 1,
    config.BoardwindowPositionX,
    UI::controlPrompts::hit.data()
  );
  mvaddstr(
    config.BoardwindowPositionY + BOARD_HEIGHT + 1,
    config.BoardwindowPositionX + BOARD_WIDTH - UI::controlPrompts::stand.length(),
    UI::controlPrompts::stand.data()
  );
  refresh();
}

void drawWindowUI(WINDOW *window, const std::vector<int>& hand, bool isDealer, bool hide = false) {
  int y{isDealer ? 0 : BOARD_HEIGHT - 1};
  std::string_view score{UI::displayScore(hand, hide)};
  mvwaddstr(
    window,
    y,
    (BOARD_WIDTH - UI::StaticText::score.length()) / 2 - 1, // The '-1' centers the static score string with the dynamic score.
    score.data()
  );
  wrefresh(window);
}

void displayHand(const std::vector<int>& hand, bool isDealer, bool hideFirstCard) {
  // Draw the virtual cards on the board when I have the willpower to implement this
}

bool evaluateTurn(WINDOW *window, std::vector<int>& hand) {
  while (true) {
    switch (getch()) {
      case 'H':
      [[fallthrough]];
      case 'h':
      hand.push_back(drawCard());
      break;
      case 'S':
      [[fallthrough]];
      case 's':
      return false;
      break;
      default:
      break;
    }

    drawWindowUI(window, hand, false);

    if (calculateScore(hand) > 21) {
      return true;
    }
  }
}

void gameOverEvents(const std::vector<int>& playerHand, const std::vector<int>& dealerHand, bool playerBusted, WINDOW *window) {
  if (playerBusted) {
    drawWindowUI(window, dealerHand, true);
    drawText(config.verticallyCentered - 1, BOARD_WIDTH, UI::EventMessages::bust);
    drawText(config.verticallyCentered, BOARD_WIDTH, UI::EventMessages::lose);
  }
  if (calculateScore(dealerHand) > 21) {
    drawText(config.verticallyCentered - 1, BOARD_WIDTH, UI::EventMessages::dealerBust);
    drawText(config.verticallyCentered, BOARD_WIDTH, UI::EventMessages::win);
  } else if (calculateScore(playerHand) > calculateScore(dealerHand) && !playerBusted) {
    drawText(config.verticallyCentered, BOARD_WIDTH, UI::EventMessages::win);
  } else if (calculateScore(playerHand) == calculateScore(dealerHand) && !playerBusted) {
    drawText(config.verticallyCentered, BOARD_WIDTH, UI::EventMessages::push);
  } else {
    drawText(config.verticallyCentered, BOARD_WIDTH, UI::EventMessages::lose);
  }
}

bool replayPrompt() {
  // Play again? Prompt
  drawText(
    config.verticallyCentered + 1,
    BOARD_WIDTH,
    UI::EventMessages::rematchPrompt
  );
  mvaddstr(
    config.verticallyCentered + 2,
    config.BoardwindowPositionX + 3,
    UI::controlPrompts::yes.data()
  );
  mvaddstr(
    config.verticallyCentered + 2,
    config.BoardwindowPositionX + BOARD_WIDTH - UI::controlPrompts::no.length() - 3,
    UI::controlPrompts::no.data()
  );

  switch(getch()) {
    case 'Y':
    [[fallthrough]];
    case 'y':
    return true;
    break;
    case 'N':
    [[fallthrough]];
    case 'n':
    return false;
    break;
    default:
    return false;
    break;
  }
}

int main() {
  setup();

  bool playAgain{true};
  // Game Loop
  while (playAgain) {

    WINDOW *boardWin = newwin(BOARD_HEIGHT, BOARD_WIDTH, config.BoardwindowPositionY , config.BoardwindowPositionX);
    box(boardWin, 0, 0);
    wrefresh(boardWin);

    setupDrawUI();

    std::vector<int> playerHand;
    std::vector<int> dealerHand;

    // Initial Draw
    playerHand.push_back(drawCard());
    playerHand.push_back(drawCard());

    dealerHand.push_back(drawCard());
    dealerHand.push_back(drawCard());

    drawWindowUI(boardWin, playerHand, false);
    drawWindowUI(boardWin, dealerHand, true, true);

    bool playerBusted{evaluateTurn(boardWin, playerHand)};

    // Dealer's turn
    if (!playerBusted) {
      if (calculateScore(dealerHand) >= 17) {
        drawText(config.BoardwindowPositionY + 1, BOARD_WIDTH, UI::EventMessages::dealerStands);
      }
      while (calculateScore(dealerHand) < 17) {
        dealerHand.push_back(drawCard());
      }
      drawWindowUI(boardWin, dealerHand, true);
    }

    gameOverEvents(playerHand, dealerHand, playerBusted, boardWin);

    playAgain = replayPrompt();

    delwin(boardWin);

  }
  endwin();
  return 0;
}
