#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define FREQ 30
#define NEUTRONS_PER_CORE 5

enum 
{
  Neutron=1,
  Uran,
  UranDestroyed
};

SDL_Rect drect;
SDL_Renderer *ren;
SDL_Event event;
SDL_Window *win;
int Umax;
int Nmax;
struct Element
{
  int indent;
  float x;
  float y;
  float vecx;
  float vecy;
} *Uran_map,  
  *Neutron_map;

void Decay_exit(void)
{
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  SDL_Quit();
  exit(0);
}

inline void make_new_neutrons(struct Element* elem)
{  
  register int times = NEUTRONS_PER_CORE;
  for(struct Element  *i = &Neutron_map[0]; i < &Neutron_map[drect.w * drect.h]; i++)
  {
    if(i -> indent != Neutron)
    {
      float randx = ( (float)( rand() % 360) ) / 360;
      int signy = rand() % 2;
      int signx = rand() % 2;
      if(signy == 0)
        signy = -1;
      else
        signy = 1;

      if(signx == 0)
        signx = -1;
      else
        signx = 1;
      i -> indent = Neutron;
      i -> x = elem -> x;
      i -> y = elem -> y;
      i -> vecx = randx * signx;
      i -> vecy = signy * sqrt(1 - randx * randx);
      if(--times==0)
        break;
    }
  }
  elem -> indent = 0;
}

inline int step_element(struct Element* elem)
  /* 
   * return !0 if out of bounds 
   * or collision occured */
{
  elem -> x += elem -> vecx;
  elem -> y += elem -> vecy;
  /* verify out of bounds */
  for(struct Element  *i = (struct Element*)&Uran_map[0]; i<&Uran_map[Umax]; i++)
  {
    if( (i -> indent == Uran) && 
        ( (int) (elem -> x) == (int)i -> x) &&
        ( (int) (elem -> y) == (int)i -> y) )
    {
      /* destroy this! */
      i -> indent = UranDestroyed;
      return 1;
    }
  }

  if(elem -> x > drect.w
      || elem -> y > drect.h
      || elem -> x < drect.x
      || elem -> y < drect.y)
  {
    return 1;
  }
  return 0;
}

inline void draw_element(struct Element* elem)
{
  SDL_RenderDrawPoint(ren, (int)elem -> x, (int) elem -> y);
}

void draw_all(void)
{
  SDL_SetRenderDrawColor(ren, 255, 148, 10, 255);
  /* draw atoms firstly */
  for(struct Element  *i = (struct Element*)&Uran_map[0]; i<&Uran_map[Umax]; i++)
  {
    if(i -> indent == Uran)
    {
      draw_element(i);
    }
    else if(i -> indent == UranDestroyed)
    {
      make_new_neutrons(i);
    }
  }
  SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
  for(struct Element  *i = (struct Element*)&Neutron_map[0]; i<&Neutron_map[drect.w * drect.h]; i++)
  {
    if( i -> indent == Neutron)
    {
      if(step_element( i ) == 0)
      {
        draw_element(i);
      }
      else
      {
        i -> indent = 0;/* clear this element */
      }
    }
  }
  SDL_SetRenderDrawColor(ren, 0, 0, 0, 0);
}
int main(int argc, char ** argv)
{
  if(SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
  {
    return -1;
  }
  win = SDL_CreateWindow("U DECAY", 2, 2, 
      0, 
      0,
      SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_SHOWN);
  if(win == NULL)
  {
    puts("error when creating window");
    puts(SDL_GetError());
    return -1;
  }
  /*
   * initialize Random generator
   */
  time_t t;
  srand((unsigned) time(&t));
  /*
   * Initializing array
   */
  SDL_GetDisplayUsableBounds(0, &drect);
  printf("Display bounds are: %d, %d, %d, %d ", 
      drect.x, drect.y, drect.w, drect.h);
  Uran_map = (struct Element*)malloc( drect.w * drect.h * sizeof(struct Element) );
  memset(Uran_map, 0, drect.w * drect.h * sizeof(struct Element));
  int i, j, indexj=0;
  for(i=drect.x; i<drect.w; i += FREQ)
  {
    for(j=drect.y; j<drect.h; j += FREQ, indexj++)
    {
      Uran_map[indexj].indent = Uran;
      Uran_map[indexj].x = i;
      Uran_map[indexj].y = j;
      Uran_map[indexj].vecx = 0;
      Uran_map[indexj].vecy = 0;
    }
  }
  Umax = indexj;

  
  Neutron_map = (struct Element*)malloc( drect.w * drect.h * sizeof(struct Element) );
  memset(Neutron_map, 0, drect.w * drect.h * sizeof(struct Element));

  indexj=0;
  for(i=drect.x; i<drect.w; i+=FREQ*4 + 1)
  {
    for(j=drect.y; j<drect.h; j+=FREQ*4 + 1, indexj++)
    {
      float randx = ( (float)( rand() % 360) ) / 360;
      int signy = rand() % 2;
      int signx = rand() % 2;
      if(signy == 0)
        signy = -1;
      else
        signy = 1;

      if(signx == 0)
        signx = -1;
      else
        signx = 1;
      Neutron_map[indexj].indent = Neutron;
      Neutron_map[indexj].x = i;
      Neutron_map[indexj].y = j;
      Neutron_map[indexj].vecx = randx * signx;
      Neutron_map[indexj].vecy = signy * sqrt(1 - randx * randx);
    }
  }
  Nmax = indexj;

  ren = \
  SDL_CreateRenderer(win, -1, 
      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if(ren == NULL)
  {
    puts("error when creating renderer");
    puts(SDL_GetError());
    return -1;
  }
  
  while(1)
  {
    SDL_PollEvent(&event);
    if(event.key.keysym.sym == 'q')
      break;
    draw_all();
    SDL_RenderPresent(ren);
    SDL_RenderClear(ren);
  }
  Decay_exit();
}
