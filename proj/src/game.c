#include "game.h"
#include "pixmap.h"

#include "video.h"
#include <lcom/lab5.h>
#include <lcom/lcf.h>
#include <machine/int86.h>
#include <stdint.h>
#include <stdio.h>

extern vbe_mode_info_t modeinfo;
card cardArray[16];
// Game Name
xpm_image_t imgGameName;
uint8_t *spriteGameName;

extern xpm_image_t imgDeck;
extern uint8_t *spriteDeck;

extern xpm_image_t imgCardBack;
extern uint8_t *spriteCardBack;

xpm_image_t imgMouse;
uint8_t *spriteMouse;

xpm_image_t imgHeart;
uint8_t *spriteHeart;

xpm_image_t imgVictory;
uint8_t *spriteVictory;

xpm_image_t imgGameOver;
uint8_t *spriteGameOver;

xpm_image_t imgInstructions;
uint8_t *spriteInstructions;

xpm_image_t imgMoon;
uint8_t *spriteMoon;

xpm_image_t imgSun;
uint8_t *spriteSun;

xpm_image_t imgBackground;
uint8_t *spriteBackground;

Player p1;

int increaseWidth = 1;
bool spriteFront = false;
int otherSide[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
Mouse mouse;

void initialize_game_cards(card *cardArray) {
  // Generate random Cards
  int first[8];
  int second[8];

  first[0] = rand() % 4;
  second[0] = rand() % 13;
  for (int i = 1; i < 8; i++) {
    int rf = rand() % 4;
    int rs = rand() % 13;
    for (int j = 0; j < i; j++) {
      if (rf == first[j] && rs == second[j]) {
        i--;
        break;
      }
      else {
        first[i] = rf;
        second[i] = rs;
      }
    }
  }

  for (int i = 0; i < 8; i++) {
    int cardposY = first[i];
    int cardposX = second[i];
    int secondHalf = 15 - i;
    cardArray[i] = (card){.x = cardposX, .y = cardposY, .isFlipped = false, .isFlipping = false, .num = getCardNumber(cardposX), .suit = getCardSuit(cardposY), .flipSuccess = false};
    cardArray[secondHalf] = (card){.x = cardposX, .y = cardposY, .isFlipped = false, .isFlipping = false, .num = getCardNumber(cardposX), .suit = getCardSuit(cardposY), .flipSuccess = false};
  }

  size_t i;
  size_t n = 16;
  for (i = 0; i < n - 1; i++) {
    size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
    card t = cardArray[j];
    cardArray[j] = cardArray[i];
    cardArray[i] = t;
  }
}

void initializePlayer() {
  p1.name = "TestNome";
  p1.lives = 5;
  p1.isAlive = true;
  p1.bestTime = 0;
}

int getCardsScreenXY(int *x, int *y) {
  int startX = modeinfo.XResolution / 2 - 2 * (CARD_WIDTH + 30);
  int startY = modeinfo.YResolution / 2 - 2 * (CARD_HEIGHT + 20);
  *x = startX + (CARD_WIDTH + 30) * (*x);
  *y = startY + (CARD_HEIGHT + 20) * (*y);
  return 0;
}

void create_mouse() {
  mouse.x = 0;
  mouse.y = 0;
}

void draw_letter(int screenX, int screenY, xpm_map_t letter) {
  xpm_image_t img;
  uint8_t *sprite;
  sprite = xpm_load(letter, XPM_8_8_8, &img);

  for (int line = screenY; line < (img.height + screenY); line++) {
    for (int col = screenX; col < (img.width + screenX); col++) {
      uint32_t color;
      memcpy(&color, sprite, 3);
      my_draw_pixel(col, line, color);
      sprite += ((modeinfo.BitsPerPixel + 7) / 8);
    }
  }
}

void draw_mouse() {
  uint8_t *sprite = spriteMouse;
  for (int line = mouse.y; line < imgMouse.height + mouse.y; line++) {
    for (int col = mouse.x; col < imgMouse.width + mouse.x; col++) {
      uint32_t color;
      memcpy(&color, sprite, 3);
      my_draw_pixel(col, line, color);
      sprite += ((modeinfo.BitsPerPixel + 7) / 8);
    }
  }
}

void update_mouse_position(int x, int y) {

  if (mouse.x + x < 0) {
    mouse.x = 0;
  }
  else if (mouse.x + x >= modeinfo.XResolution) {
    mouse.x = modeinfo.XResolution - 15;
  }
  else {
    mouse.x += x / 2;
  }

  if (mouse.y - y < 0) {
    mouse.y = 0;
  }
  else if (mouse.y + y >= modeinfo.YResolution) {
    mouse.y = modeinfo.YResolution - 15;
  }
  else {
    mouse.y -= y / 2;
  }
}

void draw_menu() {

  uint8_t *map = spriteGameName;
  for (int line = 50; line < (imgGameName.height + 50); line++) {
    for (int col = 268; col < (imgGameName.width + 268); col++) {
      uint32_t color;
      memcpy(&color, map, 3);
      my_draw_pixel(col, line, color);
      map += ((modeinfo.BitsPerPixel + 7) / 8);
    }
  }

  // LETTERS SIZE
  /* columns rows colors chars-per-pixel */
  // 13 20 2 1

  xpm_map_t wordPlay[4] = {p, l, a, y};
  int startX = modeinfo.XResolution / 2 - 2 * (LETTER_WIDTH + 5);
  int startY = 300;
  for (int i = 0; i < 4; i++) {
    draw_letter(startX, startY, wordPlay[i]);
    startX += LETTER_WIDTH + 5;
  }

  xpm_map_t wordInstructions[12] = {i, n, s, t, r, u, c, t, i, o, n, s};
  startX = modeinfo.XResolution / 2 - 6 * (LETTER_WIDTH + 5);
  startY = 400;
  for (int i = 0; i < 12; i++) {
    draw_letter(startX, startY, wordInstructions[i]);
    startX += LETTER_WIDTH + 5;
  }

  xpm_map_t wordExit[4] = {e, x, i, t};
  startX = modeinfo.XResolution / 2 - 2 * (LETTER_WIDTH + 5);
  startY = 500;
  for (int i = 0; i < 4; i++) {
    draw_letter(startX, startY, wordExit[i]);
    startX += LETTER_WIDTH + 5;
  }
}

void draw_pause() {
  uint8_t *map = spriteGameName;
  for (int line = 50; line < (imgGameName.height + 50); line++) {
    for (int col = 268; col < (imgGameName.width + 268); col++) {
      uint32_t color;
      memcpy(&color, map, 3);
      my_draw_pixel(col, line, color);
      map += ((modeinfo.BitsPerPixel + 7) / 8);
    }
  }

  // LETTERS SIZE
  /* columns rows colors chars-per-pixel */
  // 13 20 2 1

  xpm_map_t wordResume[6] = {r, e, s, u, m, e};
  int startX = modeinfo.XResolution / 2 - 3 * (LETTER_WIDTH + 5);
  int startY = 300;
  for (int i = 0; i < 6; i++) {
    draw_letter(startX, startY, wordResume[i]);
    startX += LETTER_WIDTH + 5;
  }

  xpm_map_t wordInstructions[12] = {i, n, s, t, r, u, c, t, i, o, n, s};
  startX = modeinfo.XResolution / 2 - 6 * (LETTER_WIDTH + 5);
  startY = 400;
  for (int i = 0; i < 12; i++) {
    draw_letter(startX, startY, wordInstructions[i]);
    startX += LETTER_WIDTH + 5;
  }

  xpm_map_t wordExit[4] = {e, x, i, t};
  startX = modeinfo.XResolution / 2 - 2 * (LETTER_WIDTH + 5);
  startY = 500;
  for (int i = 0; i < 4; i++) {
    draw_letter(startX, startY, wordExit[i]);
    startX += LETTER_WIDTH + 5;
  }
}

void draw_game_cards() {
  // greenScreen();
  drawBackground();
  // int startX = 150;
  // int startY = 50;
  int startX = modeinfo.XResolution / 2 - 2 * (CARD_WIDTH + 30);
  int startY = modeinfo.YResolution / 2 - 2 * (CARD_HEIGHT + 20);
  int cardsPerLine = 0;
  for (int i = 0; i < 16; i++) {
    if (cardArray[i].isFlipping && cardArray[i].isFlipped == true)
      flipACardUp();
    else if (cardArray[i].isFlipping && cardArray[i].isFlipped == false) {
      flipACardDown();
    }
    else {

      if (!cardArray[i].isFlipped)
        vg_draw_card(startX, startY, cardArray[i].x, cardArray[i].y);
      else
        vg_draw_card(startX, startY, -1, -1);
    }

    cardsPerLine++;
    startX += CARD_WIDTH + 30;
    if (cardsPerLine % 4 == 0) {
      startX = modeinfo.XResolution / 2 - 2 * (CARD_WIDTH + 30);
      startY += CARD_HEIGHT + 20;
    }
    if (startX + CARD_WIDTH + 20 > modeinfo.XResolution || startY + CARD_HEIGHT + 10 > modeinfo.YResolution)
      break;
  }
}

void draw_victory() {
  uint8_t *map = spriteVictory;
  int screenX = modeinfo.XResolution / 2 - imgVictory.width / 2;
  int screenY = modeinfo.YResolution / 2 - imgVictory.height / 2;
  for (int line = screenY; line < imgVictory.height + screenY; line++) {
    for (int col = screenX; col < imgVictory.width + screenX; col++) {
      uint32_t color;
      memcpy(&color, map, 3);
      my_draw_pixel(col, line, color);
      map += ((modeinfo.BitsPerPixel + 7) / 8);
    }
  }
}

void draw_game_over() {
  uint8_t *map = spriteGameOver;
  int screenX = modeinfo.XResolution / 2 - imgGameOver.width / 2;
  int screenY = modeinfo.YResolution / 2 - imgGameOver.height / 2;
  for (int line = screenY; line < imgGameOver.height + screenY; line++) {
    for (int col = screenX; col < imgGameOver.width + screenX; col++) {
      uint32_t color;
      memcpy(&color, map, 3);
      my_draw_pixel(col, line, color);
      map += ((modeinfo.BitsPerPixel + 7) / 8);
    }
  }
}

bool flipACardDown() {
  int currentFlipCard;
  for (int i = 0; i < 16; i++) {
    if (cardArray[i].isFlipping == true) {
      currentFlipCard = i;
      if (increaseWidth > 10 && cardArray[i].isFlipped == false)
        otherSide[i] = 1;

      if (otherSide[i] == 0) {
        int screenX = i % 4, screenY = i / 4;
        getCardsScreenXY(&screenX, &screenY);
        greenPartOfScreen(screenX, screenY, CARD_WIDTH, CARD_HEIGHT);

        // enum xpm_image_type type = XPM_8_8_8;
        xpm_image_t img = imgDeck;
        uint8_t *sprite = spriteDeck;

        int spriteStartX = cardArray[i].x * 73;
        int spriteStartY = cardArray[i].y * 99;

        sprite += (spriteStartX + (img.width * spriteStartY)) * ((modeinfo.BitsPerPixel + 7) / 8);

        for (int line = screenY; line < (CARD_HEIGHT + screenY); line++) {
          for (int col = screenX; col < ((CARD_WIDTH / increaseWidth) + screenX); col++) {
            uint32_t color;
            memcpy(&color, sprite, 3);
            my_draw_pixel(col, line, color);
            sprite += ((modeinfo.BitsPerPixel + 7) / 8) * increaseWidth;
          }
          sprite += ((modeinfo.BitsPerPixel + 7) / 8) * (img.width - CARD_WIDTH);
        }
      }
      else {
        int screenX = i % 4, screenY = i / 4;
        // enum xpm_image_type type = XPM_8_8_8;
        xpm_image_t img = imgCardBack;
        uint8_t *map = spriteCardBack;
        getCardsScreenXY(&screenX, &screenY);
        greenPartOfScreen(screenX, screenY, CARD_WIDTH, CARD_HEIGHT);

        for (int line = screenY; line < (img.height + screenY); line++) {
          for (int col = screenX; col < ((img.width / increaseWidth) + screenX); col++) {
            uint32_t color;
            memcpy(&color, map, 3);
            my_draw_pixel(col, line, color);

            map += ((modeinfo.BitsPerPixel + 7) / 8) * increaseWidth;
          }
        }
      }
    }
  }

  for (int i = 0; i < 16; i++) {
    if (otherSide[i] == 0 && cardArray[i].isFlipping == true) {
      increaseWidth++;
    }
    else if (cardArray[i].isFlipping == true) {
      increaseWidth--;
    }
  }

  if (increaseWidth == 1) {

    for (int i = 0; i < 16; i++) {
      if (cardArray[i].isFlipping) {
        cardArray[i].isFlipping = false;
        otherSide[i] = 0;
        cardArray[i].isFlipped = true;
        increaseWidth = 1;
      }
    }

    return true;
  }
  else
    return false;
}

bool flipACardUp() {
  int cardArrayPos;

  for (int i = 0; i < 16; i++) {
    // spriteFront = cardArray[i].isFlipped;
    if (cardArray[i].isFlipping == true) {
      cardArrayPos = i;
      if (increaseWidth > 10 && cardArray[i].isFlipped == true)
        otherSide[i] = 1;

      if (otherSide[i] != 0) {
        int screenX = i % 4, screenY = i / 4;
        getCardsScreenXY(&screenX, &screenY);
        greenPartOfScreen(screenX, screenY, CARD_WIDTH, CARD_HEIGHT);

        // enum xpm_image_type type = XPM_8_8_8;
        xpm_image_t img = imgDeck;
        uint8_t *sprite = spriteDeck;

        int spriteStartX = cardArray[i].x * 73;
        int spriteStartY = cardArray[i].y * 99;

        sprite += (spriteStartX + (img.width * spriteStartY)) * ((modeinfo.BitsPerPixel + 7) / 8);

        for (int line = screenY; line < (CARD_HEIGHT + screenY); line++) {
          for (int col = screenX; col < ((CARD_WIDTH / increaseWidth) + screenX); col++) {
            uint32_t color;
            memcpy(&color, sprite, 3);
            my_draw_pixel(col, line, color);
            sprite += ((modeinfo.BitsPerPixel + 7) / 8) * increaseWidth;
          }
          sprite += ((modeinfo.BitsPerPixel + 7) / 8) * (img.width - CARD_WIDTH);
        }
      }
      else {
        int screenX = i % 4, screenY = i / 4;
        // enum xpm_image_type type = XPM_8_8_8;
        xpm_image_t img = imgCardBack;
        uint8_t *map = spriteCardBack;
        getCardsScreenXY(&screenX, &screenY);
        greenPartOfScreen(screenX, screenY, CARD_WIDTH, CARD_HEIGHT);

        for (int line = screenY; line < (img.height + screenY); line++) {
          for (int col = screenX; col < ((img.width / increaseWidth) + screenX); col++) {
            uint32_t color;
            memcpy(&color, map, 3);
            my_draw_pixel(col, line, color);

            map += ((modeinfo.BitsPerPixel + 7) / 8) * increaseWidth;
          }
        }
      }
    }
  }

  for (int i = 0; i < 16; i++) {
    if (otherSide[i] == 0 && cardArray[i].isFlipping == true) {
      increaseWidth++;
    }
    else if (cardArray[i].isFlipping == true) {
      increaseWidth--;
    }
  }

  if (increaseWidth == 0) {

    for (int i = 0; i < 16; i++) {
      if (cardArray[i].isFlipping) {
        cardArray[i].isFlipped = false;
        cardArray[i].isFlipping = false;
        otherSide[i] = 0;
        increaseWidth = 1;
      }
    }
    return true;
  }
  else
    return false;
}

bool flipAllCardsDown() {

  // int currentFlipCard;
  for (int i = 0; i < 16; i++) {
    // spriteFront = cardArray[i].isFlipped;
    if (cardArray[i].isFlipping == true) {
      // currentFlipCard = i;
      if (increaseWidth > 10 && cardArray[i].isFlipped == false)
        otherSide[i] = 1;

      if (otherSide[i] == 0) {
        int screenX = i % 4, screenY = i / 4;
        getCardsScreenXY(&screenX, &screenY);
        greenPartOfScreen(screenX, screenY, CARD_WIDTH, CARD_HEIGHT);

        // enum xpm_image_type type = XPM_8_8_8;
        xpm_image_t img = imgDeck;
        uint8_t *sprite = spriteDeck;

        int spriteStartX = cardArray[i].x * 73;
        int spriteStartY = cardArray[i].y * 99;

        sprite += (spriteStartX + (img.width * spriteStartY)) * ((modeinfo.BitsPerPixel + 7) / 8);

        for (int line = screenY; line < (CARD_HEIGHT + screenY); line++) {
          for (int col = screenX; col < ((CARD_WIDTH / increaseWidth) + screenX); col++) {
            uint32_t color;
            memcpy(&color, sprite, 3);
            my_draw_pixel(col, line, color);
            sprite += ((modeinfo.BitsPerPixel + 7) / 8) * increaseWidth;
          }
          sprite += ((modeinfo.BitsPerPixel + 7) / 8) * (img.width - CARD_WIDTH);
        }
      }
      else {
        int screenX = i % 4, screenY = i / 4;
        // enum xpm_image_type type = XPM_8_8_8;
        xpm_image_t img = imgCardBack;
        uint8_t *map = spriteCardBack;
        getCardsScreenXY(&screenX, &screenY);
        greenPartOfScreen(screenX, screenY, CARD_WIDTH, CARD_HEIGHT);

        for (int line = screenY; line < (img.height + screenY); line++) {
          for (int col = screenX; col < ((img.width / increaseWidth) + screenX); col++) {
            uint32_t color;
            memcpy(&color, map, 3);
            my_draw_pixel(col, line, color);

            map += ((modeinfo.BitsPerPixel + 7) / 8) * increaseWidth;
          }
        }
      }
    }
  }

  if (otherSide[15] == 0 && cardArray[15].isFlipping == true) {
    increaseWidth++;
  }
  else if (cardArray[15].isFlipping == true) {
    increaseWidth--;
  }

  if (increaseWidth == 1) {

    for (int i = 0; i < 16; i++) {
      if (cardArray[i].isFlipping) {
        cardArray[i].isFlipping = false;
        otherSide[i] = 0;
        cardArray[i].isFlipped = true;
      }
    }

    return true;
  }
  else
    return false;
}

void drawHearts() {

  int screenY = 15;
  int screenX = 10;

  for (unsigned int i = 0; i < p1.lives; i++) {

    uint8_t *spriteHeartAux = spriteHeart;
    for (int line = screenY; line < (imgHeart.height + screenY); line++) {
      for (int col = screenX; col < (imgHeart.width + screenX); col++) {
        uint32_t color;
        memcpy(&color, spriteHeartAux, 3);
        my_draw_pixel(col, line, color);
        spriteHeartAux += ((modeinfo.BitsPerPixel + 7) / 8);
      }
    }
    screenX += imgHeart.width + 5;
  }
}

void drawMoon() {
  int screenY = 10;
  int screenX = 600;
  uint8_t *spriteHeartAux = spriteMoon;
  for (int line = screenY; line < (imgMoon.height + screenY); line++) {
    for (int col = screenX; col < (imgMoon.width + screenX); col++) {
      uint32_t color;
      memcpy(&color, spriteHeartAux, 3);
      my_draw_pixel(col, line, color);
      spriteHeartAux += ((modeinfo.BitsPerPixel + 7) / 8);
    }
  }
}

void drawSun() {
  int screenY = 10;
  int screenX = 0;
  uint8_t *map = spriteSun;
  for (int line = screenY; line < (imgSun.height + screenY); line++) {
    for (int col = screenX; col < (imgSun.width + screenX); col++) {
      uint32_t color;
      memcpy(&color, map, 3);
      my_draw_pixel(col, line, color);
      map += ((modeinfo.BitsPerPixel + 7) / 8);
    }
  }
}

void drawBackground() {
  int screenY = 0;
  int screenX = 0;
  uint8_t *map = spriteBackground;
  for (int line = screenY; line < (imgBackground.height + screenY); line++) {
    for (int col = screenX; col < (imgBackground.width + screenX); col++) {
      uint32_t color;
      memcpy(&color, map, 3);
      my_draw_pixel(col, line, color);
      map += ((modeinfo.BitsPerPixel + 7) / 8);
    }
  }
}

void drawInstructions() {

  int screenY = 0;
  int screenX = 0;

  uint8_t *map = spriteInstructions;
  for (int line = screenY; line < (imgInstructions.height + screenY); line++) {
    for (int col = screenX; col < (imgInstructions.width + screenX); col++) {
      uint32_t color;
      memcpy(&color, map, 3);
      my_draw_pixel(col, line, color);
      map += ((modeinfo.BitsPerPixel + 7) / 8);
    }
  }
}

void flipCardOnClick() {

  int screenX, screenY;
  int x, y;
  int posInArray;

  // CARD 0-0
  x = 0, y = 0, screenX = 0, screenY = 0;
  getCardsScreenXY(&screenX, &screenY);
  if ((mouse.x >= screenX && mouse.x <= screenX + CARD_WIDTH && mouse.y >= screenY && mouse.y <= screenY + CARD_HEIGHT)) {
    posInArray = x + y * 4;
    cardArray[posInArray].isFlipping = true;
  }

  // CARD 0-1
  x = 0, y = 1, screenX = 0, screenY = 1;
  getCardsScreenXY(&screenX, &screenY);
  if ((mouse.x >= screenX && mouse.x <= screenX + CARD_WIDTH && mouse.y >= screenY && mouse.y <= screenY + CARD_HEIGHT)) {
    posInArray = x + y * 4;
    cardArray[posInArray].isFlipping = true;
  }

  // CARD 2-0
  x = 2, y = 0, screenX = 2, screenY = 0;
  getCardsScreenXY(&screenX, &screenY);
  if ((mouse.x >= screenX && mouse.x <= screenX + CARD_WIDTH && mouse.y >= screenY && mouse.y <= screenY + CARD_HEIGHT)) {
    posInArray = x + y * 4;
    cardArray[posInArray].isFlipping = true;
  }

  // CARD 3-0
  x = 3, y = 0, screenX = 3, screenY = 0;
  getCardsScreenXY(&screenX, &screenY);
  if ((mouse.x >= screenX && mouse.x <= screenX + CARD_WIDTH && mouse.y >= screenY && mouse.y <= screenY + CARD_HEIGHT)) {
    posInArray = x + y * 4;
    cardArray[posInArray].isFlipping = true;
  }

  // CARD 1-0
  x = 1, y = 0, screenX = 1, screenY = 0;
  getCardsScreenXY(&screenX, &screenY);
  if ((mouse.x >= screenX && mouse.x <= screenX + CARD_WIDTH && mouse.y >= screenY && mouse.y <= screenY + CARD_HEIGHT)) {
    posInArray = x + y * 4;
    cardArray[posInArray].isFlipping = true;
  }

  // CARD 1-1
  x = 1, y = 1, screenX = 1, screenY = 1;
  getCardsScreenXY(&screenX, &screenY);
  if ((mouse.x >= screenX && mouse.x <= screenX + CARD_WIDTH && mouse.y >= screenY && mouse.y <= screenY + CARD_HEIGHT)) {
    posInArray = x + y * 4;
    cardArray[posInArray].isFlipping = true;
  }

  // CARD 2-1
  x = 2, y = 1, screenX = 2, screenY = 1;
  getCardsScreenXY(&screenX, &screenY);
  if ((mouse.x >= screenX && mouse.x <= screenX + CARD_WIDTH && mouse.y >= screenY && mouse.y <= screenY + CARD_HEIGHT)) {
    posInArray = x + y * 4;
    cardArray[posInArray].isFlipping = true;
  }

  // CARD 3-1
  x = 3, y = 1, screenX = 3, screenY = 1;
  getCardsScreenXY(&screenX, &screenY);
  if ((mouse.x >= screenX && mouse.x <= screenX + CARD_WIDTH && mouse.y >= screenY && mouse.y <= screenY + CARD_HEIGHT)) {
    posInArray = x + y * 4;
    cardArray[posInArray].isFlipping = true;
  }

  // CARD 0-2
  x = 0, y = 2, screenX = 0, screenY = 2;
  getCardsScreenXY(&screenX, &screenY);
  if ((mouse.x >= screenX && mouse.x <= screenX + CARD_WIDTH && mouse.y >= screenY && mouse.y <= screenY + CARD_HEIGHT)) {
    posInArray = x + y * 4;
    cardArray[posInArray].isFlipping = true;
  }

  // CARD 1-2
  x = 1, y = 2, screenX = 1, screenY = 2;
  getCardsScreenXY(&screenX, &screenY);
  if ((mouse.x >= screenX && mouse.x <= screenX + CARD_WIDTH && mouse.y >= screenY && mouse.y <= screenY + CARD_HEIGHT)) {
    posInArray = x + y * 4;
    cardArray[posInArray].isFlipping = true;
  }

  // CARD 2-2
  x = 2, y = 2, screenX = 2, screenY = 2;
  getCardsScreenXY(&screenX, &screenY);
  if ((mouse.x >= screenX && mouse.x <= screenX + CARD_WIDTH && mouse.y >= screenY && mouse.y <= screenY + CARD_HEIGHT)) {
    posInArray = x + y * 4;
    cardArray[posInArray].isFlipping = true;
  }

  // CARD 3-2
  x = 3, y = 2, screenX = 3, screenY = 2;
  getCardsScreenXY(&screenX, &screenY);
  if ((mouse.x >= screenX && mouse.x <= screenX + CARD_WIDTH && mouse.y >= screenY && mouse.y <= screenY + CARD_HEIGHT)) {
    posInArray = x + y * 4;
    cardArray[posInArray].isFlipping = true;
  }
  // CARD 0-3
  x = 0, y = 3, screenX = 0, screenY = 3;
  getCardsScreenXY(&screenX, &screenY);
  if ((mouse.x >= screenX && mouse.x <= screenX + CARD_WIDTH && mouse.y >= screenY && mouse.y <= screenY + CARD_HEIGHT)) {
    posInArray = x + y * 4;
    cardArray[posInArray].isFlipping = true;
  }

  // CARD 1-3
  x = 1, y = 3, screenX = 1, screenY = 3;
  getCardsScreenXY(&screenX, &screenY);
  if ((mouse.x >= screenX && mouse.x <= screenX + CARD_WIDTH && mouse.y >= screenY && mouse.y <= screenY + CARD_HEIGHT)) {
    posInArray = x + y * 4;
    cardArray[posInArray].isFlipping = true;
  }

  // CARD 2-3
  x = 2, y = 3, screenX = 2, screenY = 3;
  getCardsScreenXY(&screenX, &screenY);
  if ((mouse.x >= screenX && mouse.x <= screenX + CARD_WIDTH && mouse.y >= screenY && mouse.y <= screenY + CARD_HEIGHT)) {
    posInArray = x + y * 4;
    cardArray[posInArray].isFlipping = true;
  }

  // CARD 3-3
  x = 3, y = 3, screenX = 3, screenY = 3;
  getCardsScreenXY(&screenX, &screenY);
  if ((mouse.x >= screenX && mouse.x <= screenX + CARD_WIDTH && mouse.y >= screenY && mouse.y <= screenY + CARD_HEIGHT)) {
    posInArray = x + y * 4;
    cardArray[posInArray].isFlipping = true;
  }

  if (!cardArray[posInArray].isFlipped)
    cardArray[posInArray].isFlipping = false;
}

cardNumber getCardNumber(int x) {
  // Ace = 1,Two,Three,Four,Five,Six,Seven,Eight,Nine,Ten,Jack,Queen,King
  switch (x) {
    case 0:
      return Ace;

    case 1:
      return Two;

    case 2:
      return Three;

    case 3:
      return Four;

    case 4:
      return Five;

    case 5:
      return Six;

    case 6:
      return Seven;

    case 7:
      return Eight;

    case 8:
      return Nine;

    case 9:
      return Ten;

    case 10:
      return Jack;

    case 11:
      return Queen;

    case 12:
      return King;
  }
  return 1;
}

cardSuit getCardSuit(int y) {
  switch (y) {
    case 0:
      return Clubs;
    case 1:
      return Spades;
    case 2:
      return Hearts;
    case 3:
      return Diamonds;
  }

  return 1;
}
