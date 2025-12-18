#include "engine.hpp"
#include <ncurses.h>
#include <iostream>

void setup() {
  clear();
  noecho();
  raw();
  start_color();
  curs_set(0);
  refresh();
}

void drawText(WINDOW *window, int y, int initialX, int width, const std::string_view& text, int offset = 0) {
  // Centers the text around half of the string length by default, an offset can be passed to shift the text horizontally.
  int x = initialX + (width - text.length()) / 2;
  mvwaddstr(
    window,
    y,
    x - offset,
    text.data()
  );
  refresh();
  wrefresh(window);
}

void setupDrawUI(int rows, int columns, int boardWindowPositionY, int boardWindowPositionX) {
  drawText(
    stdscr,
    boardWindowPositionY - 1,
    boardWindowPositionX,
    BOARD_WIDTH,
    UI::StaticText::dealer
  );
  drawText(
    stdscr,
    boardWindowPositionY + BOARD_HEIGHT,
    boardWindowPositionX,
    BOARD_WIDTH,
    UI::StaticText::player
  );
  mvaddstr(
    boardWindowPositionY + BOARD_HEIGHT + 1,
    boardWindowPositionX,
    UI::controlPrompts::hit.data()
  );
  mvaddstr(
    boardWindowPositionY + BOARD_HEIGHT + 1,
    boardWindowPositionX + BOARD_WIDTH - UI::controlPrompts::stand.length(),
    UI::controlPrompts::stand.data()
  );
  refresh();
}

void drawWindowUI(WINDOW *window, int boardWindowPositionY, int boardWindowPositionX, const std::vector<int>& hand, bool isDealer, bool hide = false) {
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

int main() {
  bool playAgain{true};
 
  initscr();

  int rows{};
  int columns{};
  getmaxyx(stdscr, rows, columns);

  int boardWindowPositionY{rows / 2 - BOARD_HEIGHT / 2};
  int boardWindowPositionX{columns / 2 - BOARD_WIDTH / 2};

  // Game Loop
  while (playAgain) {
    setup();

    WINDOW *boardWin = newwin(BOARD_HEIGHT, BOARD_WIDTH, boardWindowPositionY , boardWindowPositionX);
    box(boardWin, 0, 0);
    wrefresh(boardWin);

    setupDrawUI(rows, columns, boardWindowPositionY, boardWindowPositionX);

    std::vector<int> playerHand;
    std::vector<int> dealerHand;

    // Initial Draw
    playerHand.push_back(drawCard());
    playerHand.push_back(drawCard());

    dealerHand.push_back(drawCard());
    dealerHand.push_back(drawCard());

    drawWindowUI(boardWin, boardWindowPositionY, boardWindowPositionX, playerHand, false);
    drawWindowUI(boardWin, boardWindowPositionY, boardWindowPositionX, dealerHand, true, true);

    bool playerBusted{false};
    bool playerStand{false};

    drawText(stdscr, boardWindowPositionY + BOARD_HEIGHT - 2, boardWindowPositionX, BOARD_WIDTH, UI::EventMessages::yourTurn);
 
    while (!playerBusted && !playerStand) {
      switch (getch()) {
        case 'H':
        [[fallthrough]];
        case 'h':
        playerHand.push_back(drawCard());
        break;
        case 'S':
        [[fallthrough]];
        case 's':
        playerStand = true;
        break;
        default:
        break;
      }

      drawWindowUI(boardWin, boardWindowPositionY, boardWindowPositionX, playerHand, false);

      if (calculateScore(playerHand) > 21) {
        playerBusted = true;
      }
    }

    // Dealer's turn
    if (!playerBusted) {
      if (calculateScore(dealerHand) >= 17) {
        drawText(stdscr, boardWindowPositionY + 1, boardWindowPositionX, BOARD_WIDTH, UI::EventMessages::dealerStands);
      }
      while (calculateScore(dealerHand) < 17) {
        dealerHand.push_back(drawCard());
      }
      drawWindowUI(boardWin, boardWindowPositionY, boardWindowPositionX, dealerHand, true);
    }
    // Game Over Events
    else {
      drawWindowUI(boardWin, boardWindowPositionY, boardWindowPositionX, dealerHand, true);
      drawText(stdscr, boardWindowPositionY + (BOARD_HEIGHT / 2) - 1, boardWindowPositionX, BOARD_WIDTH, UI::EventMessages::bust);
      drawText(stdscr, boardWindowPositionY + (BOARD_HEIGHT / 2), boardWindowPositionX, BOARD_WIDTH, UI::EventMessages::lose);
    }
    if (calculateScore(dealerHand) > 21) {
      drawText(stdscr, boardWindowPositionY + (BOARD_HEIGHT / 2) - 1, boardWindowPositionX, BOARD_WIDTH, UI::EventMessages::dealerBust);
      drawText(stdscr, boardWindowPositionY + (BOARD_HEIGHT / 2), boardWindowPositionX, BOARD_WIDTH, UI::EventMessages::win);
    } else if (calculateScore(playerHand) > calculateScore(dealerHand) && !playerBusted) {
      drawText(stdscr, boardWindowPositionY + (BOARD_HEIGHT / 2), boardWindowPositionX, BOARD_WIDTH, UI::EventMessages::win);
    } else if (calculateScore(playerHand) == calculateScore(dealerHand) && !playerBusted) {
      drawText(stdscr, boardWindowPositionY + (BOARD_HEIGHT / 2), boardWindowPositionX, BOARD_WIDTH, UI::EventMessages::push);
    } else {
      drawText(stdscr, boardWindowPositionY + (BOARD_HEIGHT / 2), boardWindowPositionX, BOARD_WIDTH, UI::EventMessages::lose);
    }

    // Play again? Prompt
    drawText(
      stdscr,
      boardWindowPositionY + (BOARD_HEIGHT / 2) + 1,
      boardWindowPositionX,
      BOARD_WIDTH,
      UI::EventMessages::rematchPrompt
    );
    mvaddstr(
      boardWindowPositionY + (BOARD_HEIGHT / 2) + 2,
      boardWindowPositionX + 3,
      UI::controlPrompts::yes.data()
    );
    mvaddstr(
      boardWindowPositionY + (BOARD_HEIGHT / 2) + 2,
      boardWindowPositionX + BOARD_WIDTH - UI::controlPrompts::no.length() - 3,
      UI::controlPrompts::no.data()
    );

    switch(getch()) {
      case 'Y':
      [[fallthrough]];
      case 'y':
      break;
      case 'N':
      [[fallthrough]];
      case 'n':
      playAgain = false;
      break;
      default:
      playAgain = false;
      break;
    }

    delwin(boardWin);

  }

  endwin();

  return 0;
}
