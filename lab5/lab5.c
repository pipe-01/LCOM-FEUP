#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include "video_gr.h"
#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "i8042.h"
#include "keyboard.h"

extern int counter;
extern uint8_t scancode;
extern vbe_mode_info_t vmi_p; 

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("./trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("./output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

/*
if vg_init == NULL -> print error e return 1
sleep(delay)
vg_exit()
*/
int(video_test_init)(uint16_t mode, uint8_t delay) {
  if(vg_init(mode) == NULL){
    printf("Problem setting mode\n");
    return 1;
  }
  sleep(delay);
  vg_exit();

  return 0;
}

/*
inicializar a vg no mode dado
subscrever o keyboard
chamar a função vg_draw_retangle para desenhar um retangulo na posição escolhida com o comprimento, altura e cor escolidas
copiar o que está no buffer para memoria através da função double_buffer
ciclo de paragem (press ESC)
unsubscribe keyboard
vg_exit()
*/
int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  uint8_t bit_no = IRQ1;
  uint32_t irq_set = BIT(bit_no);
  int ipc_status;
  message msg;
  double r = 0;

  if(kbc_subscribe() != 0)
    return 1;

  if(vg_init(mode) == NULL){
    printf("Problem setting mode\n");
    return 1;
  }

  if(vg_draw_rectangle(x, y, width, height, color) != 0){
    printf("Error drawing the rectangle\n");
    return 1;
  }

  double_buffer();

  while (scancode != ESCAPE) { //critério ESC
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             /* hardware interrupt notification */
          if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
            kbc_ih();
          }
          break;
        default: break;
      }
    }
  }

  kbc_unsubscribe();
  vg_exit();

  return 0;
}

/*
inicializar a vg no mode dado
subscrever o keyboard
dividir a first color em R G B perante o mode dado (switch) (mode 0x105 não tem de dividir)
inicializar o tamanho de cada retangulo
fazer um duplo for loop até ao no_rectangles
 - fazer o cálculo da cor de cada retângulo perante o mode dado (switch) (apenas o 0x105 é que é diferente) (formulas dadas)
 - chamar a função vg_draw_rectangle para cada retangulo
 - técnica de paragem, caso tenha arredondado para cima o número de triangulos (y*vmi_r)
copiar o que está no buffer para memoria através da função double_buffer
ciclo de paragem (press ESC)
unsubscribe keyboard
vg_exit()
*/
int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  vg_init(mode);

  uint8_t bit_no = IRQ1;
  uint32_t irq_set = BIT(bit_no);
  int ipc_status;
  message msg;
  double r = 0;

  if(kbc_subscribe() != 0)
    return 1;
  
  uint16_t R_first, G_first, B_first;

  switch (mode)
  {
    case 0x105: // este mode usa a first color mesmo
      break;
    case 0x110: // 5:5:5
      R_first = (first & 0x00007C00) >> vmi_p.RedFieldPosition;
      G_first = (first & 0x000003E0) >> vmi_p.GreenFieldPosition;
      B_first = (first & 0x0000001F) >> vmi_p.BlueFieldPosition;
      break;
    case 0x115: // 8:8:8
      R_first = (first & 0x00FF0000) >> vmi_p.RedFieldPosition;
      G_first = (first & 0x0000FF00) >> vmi_p.GreenFieldPosition;
      B_first = (first & 0x000000FF) >> vmi_p.BlueFieldPosition;
      break;
    case 0x11A: // 5:6:5
      R_first = (first & 0x0000F800) >> vmi_p.RedFieldPosition;
      G_first = (first & 0x000007E0) >> vmi_p.GreenFieldPosition;
      B_first = (first & 0x0000001F) >> vmi_p.BlueFieldPosition;
      break;
    case 0x14c: // 8:8:8
      R_first = (first & 0x00FF0000) >> vmi_p.RedFieldPosition;
      G_first = (first & 0x0000FF00) >> vmi_p.GreenFieldPosition;
      B_first = (first & 0x000000FF) >> vmi_p.BlueFieldPosition;
      break;
    default:
      break;
  }

  int x_rectangle = vmi_p.XResolution / no_rectangles;
  int y_rectangle = vmi_p.YResolution / no_rectangles;

  int R, G, B, color;

  for(int i = 0; i < no_rectangles; i++){ // i -> row
    for(int j = 0; j < no_rectangles; j++){ // j -> col
      switch (mode)
      {
      case 0x105: // fórmula dada (page 5 lab)
        color = (first + (i * no_rectangles + j) * step) % (1 << vmi_p.BitsPerPixel);
        break;
      default: // fórmula dada (page 5 lab)
        R = (R_first + j * step) % (1 << vmi_p.RedMaskSize);
        G = (G_first + i * step) % (1 << vmi_p.GreenMaskSize);
        B = (B_first + (j + i) * step) % (1 << vmi_p.BlueMaskSize);
        color = 0 | (R << vmi_p.RedFieldPosition) | (G << vmi_p.GreenFieldPosition) | (B << vmi_p.BlueFieldPosition); // fórmula não é dada, mas é o contrário que fizemos à first(color)
        break;
      }

      vg_draw_rectangle(j * x_rectangle , i * y_rectangle, x_rectangle, y_rectangle, color);
    }
    if (i * y_rectangle > vmi_p.YResolution) { // caso tenha arredondado para cima, dá break e fica uma linha preta em baixo e na direita
      break;
    }
  }

  double_buffer();

  while (scancode != ESCAPE) { //critério ESC
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             /* hardware interrupt notification */
          if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
            kbc_ih();
          }
          break;
        default: break;
      }
    }
  }

  kbc_unsubscribe();
  vg_exit();

  return 0;
}

/*
inicializar a vg no mode 0x105
subscrever o keyboard
inicializa um xpm_image_type (XMP_INDEXED) e uma xpm_image_t img
fazer parse do xpm dado para a img (com a função xpm_load()) e passar para um uint8_t *sprite o mem address
chama a função draw_sprite
copiar o que está no buffer para memoria através da função double_buffer
ciclo de paragem (press ESC)
unsubscribe keyboard
vg_exit()
*/
int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  int mode = 0x105;
  vg_init(mode);

  uint8_t bit_no = IRQ1;
  uint32_t irq_set = BIT(bit_no);
  int ipc_status;
  message msg;
  double r = 0;

  if(kbc_subscribe() != 0)
    return 1;
  
  enum xpm_image_type type = XPM_INDEXED;
  xpm_image_t img;
  uint8_t *sprite = xpm_load(xpm, type, &img);

  draw_sprite(img, sprite, x, y);

  double_buffer();

  while (scancode != ESCAPE) { //critério ESC
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             /* hardware interrupt notification */
          if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
            kbc_ih();
          }
          break;
        default: break;
      }
    }
  }

  kbc_unsubscribe();
  vg_exit();

  return 0;
}

/*
fazer timer_set_frequency com a fr_rate
inicializar a vg no mode 0x105
subscrever o keyboard
subscrever o timer
inicializa um xpm_image_type (XMP_INDEXED) e uma xpm_image_t img
fazer parse do xpm dado para a img (com a função xpm_load()) e passar para um uint8_t *sprite o mem address
chama a função draw_sprite para desenhar a função na posição inicial
copiar o que está no buffer para memoria através da função double_buffer
ciclo de paragem (press ESC)
 - fazer a imagem mexer (vg_draw_rectangle(), incrementar ao eixo pelo qual a imagem está a mexer e chamar a draw_sprite)
 - condições de movimento, speed < 0 e speed > 0, x == xf || y == yf
 - condições de paragem, x == xf || y == yf
unsubscribe keyboard
unsubscribe timer
vg_exit()
*/
int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate) {
  bool done_moving = false;

  timer_set_frequency(0, fr_rate);

  int mode = 0x105;
  vg_init(mode);

  uint8_t bit_no_timer = IRQ0;
  uint8_t bit_no_kbd = IRQ1;
  uint32_t irq_set_timer = BIT(bit_no_timer);
  uint32_t irq_set_kbd = BIT(bit_no_kbd);
  int ipc_status;
  message msg;
  double r = 0;

  if(timer_subscribe_int(&bit_no_timer) != 0)
    return 1;

  if(kbc_subscribe() != 0)
    return 1;

  enum xpm_image_type type = XPM_INDEXED;
  xpm_image_t img;
  uint8_t *sprite = xpm_load(xpm, type, &img);

  draw_sprite(img, sprite, xi, yi);

  double_buffer();
  while (scancode != ESCAPE) { //critério ESC
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             /* hardware interrupt notification */
          if (msg.m_notify.interrupts & irq_set_kbd) { /* subscribed interrupt */
            kbc_ih();
          }
           if (msg.m_notify.interrupts & irq_set_timer) { /* subscribed interrupt */
            timer_int_handler();
            if (!done_moving) { // se ainda não acabou a movimentação
              //mover a sprite com speed adequado
              if(speed < 0 && counter % (-speed) == 0){ // se speed negativo, incrementar 1
                if (xi == xf) { // either xf is equal to xi or yf is equal to yi, condição de parametros da função
                  vg_draw_rectangle(xi, yi, img.width, img.height, 0);
                  yi = yi + 1;  // incremento
                  if (yi > yf) { // se ultrapassar regressa à posição final
                    yi = yf;
                  }
                  draw_sprite(img, sprite, xi, yi); // draw the img
                  if (yi == yf)   // critério de paragem
                    done_moving = true;
                }
                else if (yi == yf) { // either xf is equal to xi or yf is equal to yi, condição de parametros da função
                  vg_draw_rectangle(xi, yi, img.width, img.height, 0);
                  xi = xi + 1;  // incremento
                  if (xi > xf) { // se ultrapassar regressa à posição final
                    xi = xf;
                  }
                  draw_sprite(img, sprite, xi, yi); // draw the img
                  if (xi == xf)   // critério de paragem
                    done_moving = true;
                }
              }
              else if(speed > 0){ // se speed positivo, incrementar speed
                if (xi == xf) { // either xf is equal to xi or yf is equal to yi, condição de parametros da função
                  vg_draw_rectangle(xi, yi, img.width, img.height, 0);
                  yi = yi + speed;  // incremento
                  if (yi > yf) { // se ultrapassar regressa à posição final
                    yi = yf;
                  }
                  draw_sprite(img, sprite, xi, yi); // draw the img
                  if (yi == yf)   // critério de paragem
                    done_moving = true;
                }
                else if (yi == yf) { // either xf is equal to xi or yf is equal to yi, condição de parametros da função
                  vg_draw_rectangle(xi, yi, img.width, img.height, 0);
                  xi = xi + speed;  // incremento
                  if (xi > xf) { // se ultrapassar regressa à posição final
                    xi = xf;
                  }
                  draw_sprite(img, sprite, xi, yi); // draw the img
                  if (xi == xf)   // critério de paragem
                    done_moving = true;
                }
              }
              double_buffer();
            }
          }
          break;
        default: break;
      }
    }
  }

  timer_unsubscribe_int();
  kbc_unsubscribe();
  vg_exit();

  return 0;
}
