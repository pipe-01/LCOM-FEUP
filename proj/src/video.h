#include <lcom/lcf.h>
#include <machine/int86.h>
#include <stdint.h>
#include <stdio.h>
#include <lcom/lab5.h>

#define CARD_WIDTH 69
#define CARD_HEIGHT 93
#define LETTER_WIDTH 20
#define LETTER_HEIGHT 30
#define MOUSE_WIDTH 50
#define MOUSE_HEIGHT 50

/** @defgroup video video
 * @{
 *
 * Functions for using video card
 */


/**
 * @brief Starts the video_card in mode 'mode'
 * 
 * @param mode 
 * @return 0 on success
 */
int vg_start(uint16_t mode);

/**
 * @brief Copies the buffer (color of each pixel) into the memory 
 * 
 * @return 0 on success
 */
int my_updateVideoMem();

/**
 * @brief Draws a rectangle
 * 
 * @param x x_position
 * @param y y_position
 * @param width width
 * @param height height
 * @param color color
 * @return 0 on success 
 */
int my_draw_rectangle (uint16_t x, uint16_t y,uint16_t width, uint16_t height, uint32_t color );



/**
 * @brief Draws a pixel
 * 
 * @param x x_position
 * @param y y_position
 * @param color color
 * @return 0 on success
 */
int my_draw_pixel(uint16_t x, uint16_t y, uint32_t color);

/**
 * @brief Makes every pixel of the screen green
 * 
 * @return 0 on success 
 */
int greenScreen();

/**
 * @brief Makes part of the screen green
 *
 * @param x X start position (top left)
 * @param y Y start position  ""  ""
 * @param width width of the "rectangle"
 * @param height height of the "rectangle"
 * @return 0 on success 
 */
void greenPartOfScreen(int x, int y, int width, int height);


/**
 * @brief 
 * 
 * @param screenX 
 * @param screenY 
 * @param positionX 
 * @param positionY 
 */
void vg_draw_card(int screenX, int screenY, int positionX, int positionY);

/**
 * @brief free the memory
 * 
 */
void clear_buffer();
