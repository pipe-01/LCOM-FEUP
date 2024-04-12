#include <lcom/lcf.h>
#include <lcom/lab5.h>
#include "i8042.h"
#include "video_gr.h"

static void *video_mem;
char* buffer;
vbe_mode_info_t vmi_p; // vai guardar informação do mode

/*
chamar a função vbe_get_mode_info
a partir de vmi_p definir o vram_base e vram_size (address space)
alocar spcae for our buffer (variável global)
inicializar uma variável minix_mem_range (page 4 lab)
garantir permissão ao processo para aceder ao address range (page 4 lab)
protected mode to real mode:
 - inicializar reg86
 - pô-lo a 0 com memset
 - iniciar o ax (ah e al) e bx (bx tem o bit 14 a 1 e leva o mode)
 - sys_int86(&reg86)
ver se video_mem == MAP_FAILED (page 4) -> return NULL
*/
void *(vg_init)(uint16_t mode){ 
  if(vbe_get_mode_info(mode, &vmi_p) != 0)
    return NULL;
  
  unsigned int vram_base;  /* VRAM's physical addresss */
  unsigned int vram_size;  /* VRAM's size, but you can use the frame-buffer size, instead */

  vram_base = vmi_p.PhysBasePtr;
  if(vmi_p.BitsPerPixel == 15){ // mode 0x110
    vram_size = vmi_p.XResolution * vmi_p.YResolution * 2;
  }
  else{ // others modes
    vram_size = vmi_p.XResolution * vmi_p.YResolution * (vmi_p.BitsPerPixel/8);
  }
  buffer = (char *) malloc(vram_size);
  
  struct minix_mem_range mr;
  int r;

  mr.mr_base = (phys_bytes) vram_base;	
  mr.mr_limit = mr.mr_base + vram_size;  

  if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))){ // garantir permissão ao processo para aceder ao address range
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);
    return NULL;
  }

  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
  
  reg86_t reg86;
  memset(&reg86, 0, sizeof(reg86));	/* zero the structure */
  reg86.intno = 0x10; // porta das interrupções
  reg86.ax = 0x4F02; // set mode function (ah = 4F e al = 02)
  reg86.bx = 1 << 14 | mode;
  if(sys_int86(&reg86) != OK){
    printf("\tvg_exit(): sys_int86() failed \n");
    return NULL;
  }
  
  if(video_mem == MAP_FAILED){
    panic("couldn't map video memory");
    return NULL;
  }
  
  return video_mem;
}

/*
desenha um retangulo na posição, com comprimento, altura e cor dadas
*/
int (vg_draw_rectangle)	(uint16_t	x, uint16_t	y, uint16_t	width, uint16_t	height, uint32_t color ){
  for(int i = 0; i < height; i++){
    if(vg_draw_hline(x, y+i, width, color) != 0)
      return 1;
  }

  return 0;
}

/*
para cada pixel de altura, desenha uma linha horizontal com a cor dada
*/
int (vg_draw_hline)	(	uint16_t 	x, uint16_t 	y, uint16_t 	len, uint32_t 	color){
  for(int i = 0; i < len; i++){
    if(vg_draw_pixel(x+i, y, color) != 0)
      return 1;
  }
  return 0;
}	

/*
pinta o pixel da posição dada, com a cor "color"
*/
int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color){
  if ( x > vmi_p.XResolution || y > vmi_p.YResolution) return 1;

  if(vmi_p.BitsPerPixel == 15){
    memcpy(buffer + vmi_p.XResolution * y * 2 + x * 2, &color,2);
  }
  else{
    memcpy(buffer + vmi_p.XResolution * y * (vmi_p.BitsPerPixel/8) + x * (vmi_p.BitsPerPixel/8), &color, (vmi_p.BitsPerPixel/8)); 
  }
  return 0;
}

/*
copia o que está no buffer para memória
*/
void (double_buffer)(){
  if (vmi_p.BitsPerPixel == 15){
    memcpy(video_mem, buffer, vmi_p.XResolution * vmi_p.YResolution * 2);
  }else{
    memcpy(video_mem, buffer, vmi_p.XResolution * vmi_p.YResolution * (vmi_p.BitsPerPixel/8));
  }
}

/*
for loop duplo para img.height e img.width
chamar a função vg_draw_pixel
incrementar ao adress (sprite) (que vai ter a cor de cada pixel)
*/
void(draw_sprite)(xpm_image_t img, uint8_t *sprite, int x, int y){
  for(int i = 0; i < img.height; i++){
    for(int j = 0; j < img.width; j++){
      vg_draw_pixel(x + j,i + y,*(sprite));
      sprite++;
    }
  }
}
