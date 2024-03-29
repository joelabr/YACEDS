#ifndef _YACEDS_H_
#define _YACEDS_H_

#include <nds.h>
#include <fat.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>

#include "../YACE/include/Chip8.h"
#include <iniparser.h>

class YACEDS
{
  public:
    YACEDS();
    YACEDS(const char* filename);
    
    void read_configuration(char* filename);
    void run();
  
  private:
    YACE::Chip8 chip8;
    unsigned short* back_buffer;
    int background;
    int current_scale;
    bool game_loaded; 
    bool running;
    int sound_id;
    
    void check_input();
    bool init();
    void loop();
    void render();
    
    void set_colors(int background, int foreground);
    void set_scale(int scale);
  
};

#endif
