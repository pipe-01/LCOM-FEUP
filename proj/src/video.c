
#include "video.h"

static char *video_mem; /* Process (virtual) address to which VRAM is mapped */
static char *buffer;

xpm_image_t imgDeck;
uint8_t *spriteDeck;
xpm_image_t imgCardBack;
uint8_t *spriteCardBack;

// static unsigned h_res;	        /* Horizontal resolution in pixels */
// static unsigned v_res;	        /* Vertical resolution in pixels */
// static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

vbe_mode_info_t modeinfo;

int vg_start(uint16_t mode) {

  unsigned int vram_base; /* VRAM's physical addresss */
  unsigned int vram_size; /* VRAM's size, but you can use
             the frame-buffer size, instead */
  int r;

  /* Use VBE function 0x01 to initialize vram_base and vram_size */

  vbe_get_mode_info(mode, &modeinfo);

  vram_base = modeinfo.PhysBasePtr;
  if (modeinfo.BitsPerPixel == 15) { // Not a multiple of 8 (1 byte) for mode 0x110
    vram_size = modeinfo.XResolution * modeinfo.YResolution * 2;
  }
  else {
    vram_size = modeinfo.XResolution * modeinfo.YResolution * ((modeinfo.BitsPerPixel) / 8);
  }

  /* Allow memory mapping */
  struct minix_mem_range mr;
  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size;

  if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);

  /* Map memory */
  buffer = (char *) malloc(vram_size);
  video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);

  // SET OPERATING MODE
  reg86_t regMode;
  memset(&regMode, 0, sizeof(regMode)); /* zero the structure */
  regMode.ax = 0x4F02;                  // VBE call, function 02 -- set VBE mode
  regMode.bx = 1 << 14 | mode;          // set bit 14: linear framebuffer
  regMode.intno = 0x10;

  if (sys_int86(&regMode)) {
    printf("set_vbe_mode: sys_int86() failed \n");
    return 1;
  }

  if (video_mem == MAP_FAILED) {
    panic("couldn't map video memory");
    return 1;
  }

  return 0;
}


int my_updateVideoMem() {

  if (modeinfo.BitsPerPixel == 15) {
    memcpy(video_mem, buffer, 2 * modeinfo.XResolution * modeinfo.YResolution);
  }
  else {
    memcpy(video_mem, buffer, (modeinfo.BitsPerPixel / 8) * modeinfo.XResolution * modeinfo.YResolution);
  }

  return 0;
}

int greenScreen() {

  for (int i = 0; i < modeinfo.YResolution; i++) {
    for (int j = 0; j < modeinfo.XResolution; j++) {
      my_draw_pixel(j, i, 0x22B14C);
    }
  }

  return 0;
}

void greenPartOfScreen(int x, int y, int width, int height) {

  for (int i = y; i < height + y; i++) {
    if (i == modeinfo.YResolution)
      break;

    for (int j = x; j < width + x; j++) {
      if (j == modeinfo.XResolution)
        break;

      my_draw_pixel(j, i, 0x22B14C);
    }
  }
}


int my_draw_pixel(uint16_t x, uint16_t y, uint32_t color) {

  if (x > modeinfo.XResolution || y > modeinfo.YResolution)
    return 1;

  if (modeinfo.BitsPerPixel == 15) {
    memcpy(buffer + modeinfo.XResolution * y * 2 + x * 2, &color, 2); // (*2) por cause de bytes per pixel, 15 bits - 2 bytes
  }
  else {
    memcpy(buffer + modeinfo.XResolution * y * (modeinfo.BitsPerPixel / 8) + x * (modeinfo.BitsPerPixel / 8), &color, (modeinfo.BitsPerPixel / 8));
  }

  return 0;
}



void vg_draw_card(int screenX, int screenY, int positionX, int positionY) {
  // TOTAL WIDTH - HEIGHT --> 950 392
  //  NUMBER CARDS WIDTH - HEIGHT -->  13 4
  int spriteStartX = positionX * 73;
  int spriteStartY = positionY * 99;

  if (positionX == -1 && positionY == -1) {
    uint8_t *spritecardbackaux = spriteCardBack;

    for (int line = screenY; line < (imgCardBack.height + screenY); line++) {
      for (int col = screenX; col < (imgCardBack.width + screenX); col++) {
        uint32_t color;
        memcpy(&color, spritecardbackaux, 3);
        my_draw_pixel(col, line, color);
        spritecardbackaux += ((modeinfo.BitsPerPixel + 7) / 8);
      }
    }
  }
  else {

    uint8_t *spriteDeckAux = spriteDeck;
    spriteDeckAux += (spriteStartX + (imgDeck.width * spriteStartY)) * ((modeinfo.BitsPerPixel + 7) / 8);

    for (int line = screenY; line < (CARD_HEIGHT + screenY); line++) {
      for (int col = screenX; col < (CARD_WIDTH + screenX); col++) {
        uint32_t color;
        memcpy(&color, spriteDeckAux, 3);
        my_draw_pixel(col, line, color);
        spriteDeckAux += ((modeinfo.BitsPerPixel + 7) / 8);
      }
      spriteDeckAux += ((modeinfo.BitsPerPixel + 7) / 8) * (imgDeck.width - CARD_WIDTH);
    }
  }
}


void clear_buffer(){
  free(buffer);
}
