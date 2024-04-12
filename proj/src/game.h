#ifndef LCOM_GAME_H
#define LCOM_GAME_H
// #include "structs.h"
#include "structs.h"

uint8_t *load_xpms();

/** @defgroup game game
 * @{
 *
 * Functions for drawing game elements and update game state
 */

/**
 * @brief Create a mouse object
 * 
 */
void create_mouse();

/**
 * @brief Draws the menu, Play button, Instructions Button and Exit button
 * 
 */
void draw_menu();


/**
 * @brief Draws the pause menu, Resume button, Instructions Button and Exit button
 * 
 */	
void draw_pause();

/**
 * @brief Draws the mouse
 * 
 */
void draw_mouse();

/**
 * @brief updates mouse x position and y position
 * 
 * @param x x_position
 * @param y y_position
 */
void update_mouse_position(int x, int y);

/**
 * @brief Generates the board, the 16 cards of the game
 * 
 * @param cardArray array of cards that is updated in this function
 */
void initialize_game_cards(card *cardArray);

/**
 * @brief Initialize the Player stats 
 * 
 */
void initializePlayer();

/**
 * @brief Draws the board, the background and the cards
 * 
 */
void draw_game_cards();

/**
 * @brief Draws the players' lives
 * 
 */
void drawHearts();

/**
 * @brief Draws the MOOOON
 * 
 */
void drawMoon();

/**
 * @brief Draws the Sun
 * 
 */
void drawSun();

/**
 * @brief Draws the Backgorund
 * 
 */
void drawBackground();

/**
 * @brief Draws the victory img
 * 
 */
void draw_victory();

/**
 * @brief Draws the game over img
 * 
 */
void draw_game_over();

/**
 * @brief Draws the instructions of the game
 * 
 */
void drawInstructions();

/**
 * @brief Flips a card that is facing up, to facing down
 * 
 * @return true 
 * @return false 
 */
bool flipACardDown();

/**
 * @brief Flips a card that is facing down, to facing up
 * 
 * @return true 
 * @return false 
 */
bool flipACardUp();

bool flipAllCardsDown();

/**
 * @brief Get the Cards Screen X Y object
 * 
 * @param x x_position
 * @param y y_position
 * @return int 
 */
int getCardsScreenXY(int *x, int *y);

/**
 * @brief Flips a card when the user clicks on it
 * 
 */
void flipCardOnClick();

/**
 * @brief Draws the letter 'letter' in the position (screenX, screenY) 
 * 
 * @param screenX x_position
 * @param screenY y_position
 * @param letter 
 */
void draw_letter(int screenX,int screenY, xpm_map_t letter);

/**
 * @brief Get the Card Number object
 * 
 * @param x 
 * @return cardNumber 
 */
cardNumber getCardNumber(int x);

/**
 * @brief Get the Card Suit object
 * 
 * @param y 
 * @return cardSuit 
 */
cardSuit getCardSuit(int y);

#endif // LCOM_GAME_
