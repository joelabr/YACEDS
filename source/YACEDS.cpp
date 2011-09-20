#include "YACEDS.h"

YACEDS::YACEDS() : chip8(), background(-1), current_scale(0), game_loaded(false), running(false)
{
  if (!init())
  {
    fprintf(stderr, "Failed to initialize YACE DS!\n");
    return;
  }
}

YACEDS::YACEDS(const char* filename) : chip8(), background(-1), current_scale(0), game_loaded(false), running(false)
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
  }
  catch (const char* text)
  {
    fprintf(stderr, "Exception caught!\n%s\n", text);
  }
  
  // TEMPORARY!
  chip8.set_cpu_cycles(2);
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
  if (keysDown() & KEY_L)
  {
    if (current_scale > 0)
      set_scale(--current_scale);
  }
  
  if (keysDown() & KEY_R)
  {
    if (current_scale < 2)
      set_scale(++current_scale);
  }
}

/**
 *  Initialize resources
 */
bool YACEDS::init()
{
  videoSetMode(MODE_3_2D);
  vramSetBankA(VRAM_A_MAIN_BG);
  
  consoleDemoInit();
  
  background = bgInit(3, BgType_Bmp8, BgSize_B8_128x128, 0, 0);
  if (background == -1)
  {
    fprintf(stderr, "Failed to create background!\n");
    return false;
  }
  
  set_scale(1);
  
  BG_PALETTE[0] = RGB15(31, 31, 31);
  BG_PALETTE[1] = 0;
  
  if (!fatInitDefault())
  {
    fprintf(stderr, "Failed to initialize FAT!\n");
    return false;
  }

  return true;
}

void YACEDS::loop()
{
  check_input();
  chip8.step();
}

void YACEDS::render()
{
  const char* video = chip8.get_video();
  
  for (int i = 0; i < 0x800; i += 64)
    dmaCopy(&video[i], bgGetGfxPtr(background) + ((i / 64) * 64), 64);
}

void YACEDS::set_scale(int scale)
{
  if (scale >= 0 && scale <= 8)
  {
    bgSetScale(background, 1 << (8 - scale), 1 << (8 - scale));
    bgSetScroll(background, -((256 >> scale) >> 1) + 32, -((192 >> scale) >> 1) + 16);
    bgUpdate();
  }
}

/*
 *  Public methods
 */
void YACEDS::run()
{
  running = true;
  
  while (running)
  {
    if (game_loaded)
    {
      // loop();
      // render();
    }
    
    swiWaitForVBlank();
  }
}