#ifndef _YACEDS_H_
#define _YACEDS_H_

#include <nds.h>
#include <fat.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>

#include "Chip8.h"

class YACEDS
{
  public:
    YACEDS();
    YACEDS(const char* filename);
    
    void run();
  
  private:
    YACE::Chip8 chip8;
    int background;
    int current_scale;
    bool game_loaded;
    bool running;
    
    void check_input();
    bool init();
    void loop();
    void render();
    void set_scale(int scale);
  
};

#endif