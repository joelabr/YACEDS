#include "YACEDS.h"

YACEDS::YACEDS() : chip8(), back_buffer(0), background(-1), current_scale(0), game_loaded(false), running(false), sound_id(0)
{
  if (!init())
  {
    fprintf(stderr, "Failed to initialize YACE DS!\n");
    return;
  }
}

YACEDS::YACEDS(const char* filename) : chip8(), back_buffer(0), background(-1), current_scale(0), game_loaded(false), running(false),
                                       sound_id(0)
{
  if (!init())
  {
    fprintf(stderr, "Failed to initialize YACE DS!\n");
    return;
  }
  
  try
  {
    chip8.load_game(filename);
    game_loaded = true;

    set_scale(current_scale);
  }
  catch (const char* text)
  {
    fprintf(stderr, "Exception caught!\n%s\n", text);
  }
}

/*
 *  Private methods
 */
/**
 *  Check keys and touch
 */
void YACEDS::check_input()
{
  scanKeys();
  
  if (keysDown() & KEY_UP)
  {
    int cycles = chip8.get_cpu_cycles();
    if (cycles < 30)
      chip8.set_cpu_cycles(cycles + 1);
  }
  else if (keysDown() & KEY_DOWN)
  {
    int cycles = chip8.get_cpu_cycles();
    if (cycles > 1)
      chip8.set_cpu_cycles(cycles - 1);
  }
  
  if (keysDown() & KEY_LEFT)
  {
    if (current_scale > 0)
      set_scale(--current_scale);
  }
  if (keysDown() & KEY_RIGHT)
  {
    if (current_scale < (2 >> chip8.get_video_mode()))
      set_scale(++current_scale);
  }
}

/**
 *  Initialize resources
 */
bool YACEDS::init()
{
  // Initialize video
  videoSetMode(MODE_3_2D);
  vramSetBankA(VRAM_A_MAIN_BG);
  
  // Initialize sound
  soundEnable();
  
  sound_id = soundPlayPSG(DutyCycle_50, 7500, 127, 64);
  if (!sound_id)
    return false;
   
  soundPause(sound_id);
  
  // Initialize console
  consoleDemoInit();
  
  background = bgInit(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);
  if (background == -1)
  {
    fprintf(stderr, "Failed to create background!\n");
    return false;
  }
  
  back_buffer = bgGetGfxPtr(background) + (256 * 256);
  
  set_scale(current_scale);
  
  BG_PALETTE[0] = RGB15(31, 31, 31);
  BG_PALETTE[1] = 0;
  
  if (!fatInitDefault())
  {
    fprintf(stderr, "Failed to initialize FAT!\n");
    return false;
  }

  return true;
}

/**
 *  Main emulation loop
 */
void YACEDS::loop()
{
  check_input();
  
  if (chip8.get_sound_timer() > 0)
    soundResume(sound_id);
  else
    soundPause(sound_id);
  
  chip8.step();
}

/**
 *  Renders graphics
 */
void YACEDS::render()
{
  const char* video = chip8.get_video();

  int width = 64 << chip8.get_video_mode();
  int height = 32 << chip8.get_video_mode();
  int length = width * height;
  
  // Flush DMA and copy video to back buffer
  DC_FlushAll();
  for (int i = 0; i < length; i += width)
    dmaCopy(&video[i], back_buffer + (i / width) * 128, width);
    
  // Change buffer
  back_buffer = (unsigned short*) bgGetGfxPtr(background);
  
  if (bgGetMapBase(background) == 4)
    bgSetMapBase(background, 0);
  else
    bgSetMapBase(background, 4);
}

/**
 *  Sets background- and foreground colors.
 *  Color range: 0 (black) - 32767 (white)
 */
void YACEDS::set_colors(int background, int foreground)
{
  if (background >= 0 && background <= 32767)
    BG_PALETTE[0] = (unsigned short) background & 0xFFFF;
  
  if (foreground >= 0 && foreground <= 32767)
    BG_PALETTE[1] = (unsigned short) foreground & 0xFFFF;
} 

/**
 *  Sets scale of background and centers it.
 *  Scale ratio = 2^scale
 */
void YACEDS::set_scale(int scale)
{
  if (scale >= 0 && scale <= 8)
  {
    int width = 32 << chip8.get_video_mode();
    int height = 16 << chip8.get_video_mode();

    bgSetScale(background, 1 << (8 - scale), 1 << (8 - scale));
    bgSetScroll(background, -((256 >> scale) >> 1) + width, -((192 >> scale) >> 1) + height);
    bgUpdate();
  }
}

/*
 *  Public methods
 */
void YACEDS::read_configuration(char* filename)
{
  dictionary* configuration = 0;
  
  configuration = iniparser_load(filename);
  if (!configuration)
  {
    fprintf(stderr, "Failed to read configuration!\n");
    return;
  }
  
  // DEBUG - Dump contents of configuration file.
  // iniparser_dump(configuration, stderr);
  
  // Set foreground- and background colors
  int ini_background = iniparser_getint(configuration, "general:bgcolor", 32767);
  int ini_foreground = iniparser_getint(configuration, "general:fgcolor", 0);
  
  set_colors(ini_background, ini_foreground);
  
  // Set CPU cycles
  int ini_cycles = iniparser_getint(configuration, "general:cycles", 1);
  
  if (ini_cycles > 0)
    chip8.set_cpu_cycles(ini_cycles);
    
  // Set scaling
  int ini_scaling = iniparser_getint(configuration, "general:scaling", 0);
  
  current_scale = ini_scaling;

  iniparser_freedict(configuration);
}
 
void YACEDS::run()
{
  running = true;
  
  while (running)
  {
    if (game_loaded)
    {
      loop();
      render();
    }
    
    swiWaitForVBlank();
  }
}
