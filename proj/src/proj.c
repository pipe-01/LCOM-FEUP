#include <lcom/lcf.h>

#include "game.h"
#include "keyboard.h"
#include "mouse.h"
#include "pixmap.h"
// #include "structs.h"
#include "rtc.h"
#include "timer.h"
#include "video.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern uint8_t scancode;
extern vbe_mode_info_t modeinfo;
extern int tmr_tck_counter;

extern xpm_image_t imgGameName;
extern uint8_t *spriteGameName;

extern xpm_image_t imgDeck;
extern uint8_t *spriteDeck;

extern xpm_image_t imgCardBack;
extern uint8_t *spriteCardBack;

extern xpm_image_t imgMouse;
extern uint8_t *spriteMouse;

extern xpm_image_t imgHeart;
extern uint8_t *spriteHeart;

extern xpm_image_t imgVictory;
extern uint8_t *spriteVictory;

extern xpm_image_t imgGameOver;
extern uint8_t *spriteGameOver;

extern xpm_image_t imgInstructions;
extern uint8_t *spriteInstructions;

extern xpm_image_t imgMoon;
extern uint8_t *spriteMoon;

extern xpm_image_t imgSun;
extern uint8_t *spriteSun;

extern xpm_image_t imgBackground;
extern uint8_t *spriteBackground;

// victory

extern card cardArray[16];
extern Mouse mouse;
extern struct packet packet1;
extern int counter;
extern uint8_t read_packet;
extern bool reading_error;

extern Date date;
extern Player p1;

bool initialAllCardsFlip = true;
bool isAnyCardFlipping = false;

int maxNumOfCardsFlipped = 2;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/g02/proj/src/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/g02/proj/src/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(proj_main_loop)(int argc, char *argv[]) {

  srand(time(NULL)); // Initialization, should only be called once.

  // CARDS INTIALIZE
  initialize_game_cards(cardArray);
  initializePlayer();
  create_mouse();
  spriteGameName = xpm_load(gameName, XPM_8_8_8, &imgGameName);
  spriteDeck = xpm_load(deck, XPM_8_8_8, &imgDeck);
  spriteCardBack = xpm_load(cardback, XPM_8_8_8, &imgCardBack);
  spriteMouse = xpm_load(mouse_cursor, XPM_8_8_8, &imgMouse);
  spriteHeart = xpm_load(heart, XPM_8_8_8, &imgHeart);
  spriteVictory = xpm_load(victory, XPM_8_8_8, &imgVictory);
  spriteGameOver = xpm_load(game_over, XPM_8_8_8, &imgGameOver);
  spriteInstructions = xpm_load(instructions, XPM_8_8_8, &imgInstructions);
  spriteSun = xpm_load(sun, XPM_8_8_8, &imgSun);
  spriteMoon = xpm_load(moon, XPM_8_8_8, &imgMoon);
  spriteBackground = xpm_load(background, XPM_8_8_8, &imgBackground);

  gameState gs = Menu;

  // int waitTime = 0;
  gameState prevState = Exit;

  // Fill the struct date with the current date
  get_date();

  int ipc_status;
  message msg;
  double r = 0;
  // KBC
  uint8_t hookid = 1;
  if (kbc_subscribe(&hookid) != OK) {
    printf("Error subscribing to KBC\n");
    return 1;
  }
  uint8_t irq_set = BIT(hookid);

  // TIMER
  uint8_t hookid2 = 0;
  if (timer_subscribe_int(&hookid2) != OK) {
    printf("Error subscribing to timer\n");
    return 1;
  }
  uint8_t irq_set2 = BIT(hookid2);

  // MOUSE
  uint8_t hookid3 = 2;
  if (mouse_subscribe(&hookid3) != OK) {
    printf("Error subscribing to mouse\n");
    return 1;
  }
  uint8_t irq_set3 = BIT(hookid3);

  if (mouse_enable_data_reporting() != 0) {
    printf("Error enabling mouse data reporting\n");
    return 1;
  }

  // Video
  uint16_t mode = 0x115;
  if (vg_start(mode) != 0) {
    printf("Error starting video\n");
    return 1;
  }

  bool endGame = true;
  bool initGame = true;
  bool condition = true, game_over = false, victory = false;

  while (condition) {
    /* Get a request message. */
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %f\n", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification*/
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & (irq_set)) { // KEYBOARD
            kbc_ih();
          }

          if (msg.m_notify.interrupts & (irq_set3)) { // MOUSE

            mouse_ih(); // calls interrupt handler
            if (reading_error == true && counter != 2) {
              continue;
            }
            else if (reading_error == true && counter == 2) {
              reading_error = false;
            }
            else {
              print_mouse_packet();
              update_mouse_position(packet1.delta_x, packet1.delta_y);
            }
          }

          if (msg.m_notify.interrupts & (irq_set2)) { // TIMER -- main code here...
            timer_int_handler();

            switch (gs) {
              case Menu:
                greenScreen();

                if (date.hour < 20 && date.hour >= 6) {
                  drawSun();
                }
                else {
                  drawMoon();
                }

                draw_menu();

                my_updateVideoMem();

                if (scancode == P_KEY_BREAKCODE || (packet1.lb && mouse.x >= 360 && mouse.x <= 445 && mouse.y >= 300 && mouse.y <= 330)) { // Play
                  scancode = 0x00;
                  tmr_tck_counter = 0;
                  gs = Game;
                  packet1.lb = false;
                }
                if (scancode == I_KEY_BREAKCODE || (packet1.lb && mouse.x >= 280 && mouse.x <= 520 && mouse.y >= 400 && mouse.y <= 430)) { // Instructions
                  gs = Instructions;
                  packet1.lb = false;
                }
                if (scancode == E_KEY_BREAKCODE || (packet1.lb && mouse.x >= 360 && mouse.x <= 440 && mouse.y >= 500 && mouse.y <= 530)) // Exit
                  gs = Exit;

                break;
              case Game:

                if (initGame) {

                  if (tmr_tck_counter % 240 == 0) {
                    for (int i = 0; i < 16; i++) {
                      cardArray[i].isFlipping = true;
                    }
                    initGame = false;
                  }
                  //greenScreen();

                  draw_game_cards();
                  drawHearts();
                  my_updateVideoMem();
                }
                else {
                  if (tmr_tck_counter % 2 == 0)
                    draw_game_cards();

                  drawHearts();
                  my_updateVideoMem();

                  if (scancode == 0x81) { // pressing ESC opens de pause menu
                    gs = Pause;
                  }

                  isAnyCardFlipping = false;
                  if (packet1.lb) {
                    for (int i = 0; i < 16; i++) {
                      if (cardArray[i].isFlipping == true)
                        isAnyCardFlipping = true;
                    }

                    if (!isAnyCardFlipping) {
                      flipCardOnClick();
                    }
                  }

                  if (!isAnyCardFlipping) {
                    int flipCount = 0;
                    int cardPosInArray;
                    for (int i = 0; i < 16; i++) {
                      if (cardArray[i].isFlipped == false && flipCount == 0 && cardArray[i].flipSuccess == false) {
                        flipCount++;
                        cardPosInArray = i;
                      }
                      else if (cardArray[i].isFlipped == false && flipCount == 1 && cardArray[i].flipSuccess == false) {

                        if (cardArray[i].num == cardArray[cardPosInArray].num && cardArray[i].suit == cardArray[cardPosInArray].suit) {
                          cardArray[i].flipSuccess = true;
                          cardArray[cardPosInArray].flipSuccess = true;
                        }
                        else {
                          if (!cardArray[i].isFlipping) {
                            sleep(1);
                            p1.lives--;
                            if (p1.lives == 0) {
                              p1.isAlive = false;
                              game_over = true;
                              gs = Exit;
                            }
                          }
                          cardArray[i].isFlipping = true;
                          cardArray[cardPosInArray].isFlipping = true;
                        }
                      }
                    }
                  }

                  endGame = true;
                  for (int i = 0; i < 16; i++) {
                    if (cardArray[i].flipSuccess == false) {
                      endGame = false;
                    }
                  }

                  if (tmr_tck_counter % 120 == 0) {
                    if (endGame) { // game is now over
                      gs = Exit;
                      victory = true;
                    }
                  }
                }

                break;
              case Pause: // pause menu
                greenScreen();
                draw_pause();

                if (scancode == P_KEY_BREAKCODE || (packet1.lb && mouse.x >= 360 && mouse.x <= 445 && mouse.y >= 300 && mouse.y <= 330)) { // Resume
                  scancode = 0x00;
                  gs = Game;
                }
                if (scancode == I_KEY_BREAKCODE || (packet1.lb && mouse.x >= 280 && mouse.x <= 520 && mouse.y >= 400 && mouse.y <= 430)) { // Instructions
                  gs = Instructions;
                  packet1.lb = false;
                  prevState = Game;
                  scancode = 0x00;
                }
                if (scancode == E_KEY_BREAKCODE || (packet1.lb && mouse.x >= 360 && mouse.x <= 440 && mouse.y >= 500 && mouse.y <= 530)) { // Exit
                  gs = Exit;
                  scancode = 0x00;
                }
                break;
              case Instructions:
                greenScreen();
                drawInstructions();
                if (prevState == Game) {
                  if (scancode == ESCAPE || packet1.lb) {
                    gs = Game;
                    prevState = Instructions;
                    packet1.lb = false;
                  }
                }
                else {
                  if (scancode == ESCAPE)
                    gs = Menu;
                    packet1.lb = false;
                }
                break;
              case Exit: // exit
                greenScreen();
                if (game_over) {
                  draw_game_over();

                  if (tmr_tck_counter % 240 == 0) {
                    gs = Menu;
                    game_over = false;
                    victory = false;
                    initGame = true;
                    initialize_game_cards(cardArray);
                    initializePlayer();
                  }
                }
                else if (victory) {
                  draw_victory();

                  if (tmr_tck_counter % 240 == 0) {
                    gs = Menu;
                    game_over = false;
                    victory = false;
                    initGame = true;
                    initialize_game_cards(cardArray);
                    initializePlayer();
                  }
                }
                else {
                  condition = false;
                }
                my_updateVideoMem();

                break;
              default:
                break;
            }
            draw_mouse();
            my_updateVideoMem();
          }
      }
    }
  }

  if (disable_mouse_data() != 0) {
    printf("Error disabling mouse data reporting\n");
    return 1;
  }
  if (timer_unsubscribe_int() != 0) {
    printf("Error unsubscribing timer\n");
    return 1;
  }
  if (kbc_unsubscribe() != 0) {
    printf("Error unsubscribing keyboard\n");
    return 1;
  }
  if (mouse_unsubscribe() != 0) {
    printf("Error unsubscribing mouse\n");
    return 1;
  }

  if (vg_exit() != 0) {
    printf("Error exiting video mode\n");
    return 1;
  }

  clear_buffer();

  printf("Thank you for playing LCOM Memory\nSee you soon!\n");

  return 0;
}
