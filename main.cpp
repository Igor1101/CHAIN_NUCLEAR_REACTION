#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cmath>

#define FREQ 11
#define NEUTRONS_PER_CORE 2
using namespace std;

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

void Decay_exit()
{
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  SDL_Quit();
  exit(0);
}

inline void select_to_die(void)
{
  /* select probability: should selected atom die?*/
  if(rand() % 100 == 4)
  {/* Ok, select now */
    unsigned UDestroyed = rand() % Umax;
    if(Uran_map[UDestroyed].indent == Uran) 
      Uran_map[UDestroyed].indent = UranDestroyed;
  }
}
inline void make_new_neutrons(Element* elem)
{  
  register int times = NEUTRONS_PER_CORE;
  for(Element  *i = &Neutron_map[0]; i < &Neutron_map[drect.w * drect.h]; i++)
  {
    if(i -> indent != Neutron)
    {
      float randx = ( static_cast<float>( rand() % 361) ) / 360;
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

inline int step_element(Element* elem)
  /* 
   * return !0 if out of bounds 
   * or collision occured */
{
  elem -> x += elem -> vecx;
  elem -> y += elem -> vecy;
  /* verify out of bounds */
  for(Element  *i = (Element*)&Uran_map[0]; i<&Uran_map[Umax]; i++)
  {
    if( (i -> indent == Uran) && 
        ( static_cast<int> (elem -> x) == static_cast<int>(i -> x) ) &&
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

inline void draw_element(register Element* elem)
{
  SDL_RenderDrawPoint(ren, (int)elem -> x, (int) elem -> y);
}

void draw_all(void)
{
  SDL_SetRenderDrawColor(ren, 255, 148, 10, 255);
  /* draw atoms firstly */
  for(Element  *i = (Element*)&Uran_map[0]; i<&Uran_map[Umax]; i++)
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
  for(Element  *i = (Element*)&Neutron_map[0]; i<&Neutron_map[drect.w * drect.h]; i++)
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
  srand( (unsigned) time(&t) );
  /*
   * Initializing array
   */
  SDL_GetDisplayUsableBounds(0, &drect);
  cout << "Display bounds are:" \
    << drect.x << setw(5) \
    << drect.y << setw(5) \
    << drect.w << setw(5) \
    << drect.h << endl;
  Uran_map = (Element*)malloc( drect.w * drect.h * sizeof(Element) );
  memset(Uran_map, 0, drect.w * drect.h * sizeof(Element));
  int i, j, indexj=0;
  /* Make Uran crystalline lattice */
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

  
  Neutron_map = (Element*)malloc( drect.w * drect.h * sizeof(Element) );
  memset(Neutron_map, 0, drect.w * drect.h * sizeof(Element));
  ren = \
  SDL_CreateRenderer(win, -1, 
      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if(ren == NULL)
  {
    puts("error when creating renderer");
    puts(SDL_GetError());
    return -1;
  }
  register int times;
  while(1)
  {
    select_to_die();
    SDL_PollEvent(&event);
    if(event.key.keysym.sym == 'q')
      break;
    draw_all();
    SDL_RenderPresent(ren);
    SDL_RenderClear(ren);
    times++;
  }
  Decay_exit();
}
