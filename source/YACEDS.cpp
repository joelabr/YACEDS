#include "YACEDS.h"

YACEDS::YACEDS() : chip8(), running(false)
{
  if (!init())
  {
    fprintf(stderr, "Failed to initialize YACE DS!\n");
    return;
  }
}

YACEDS::YACEDS(const char* filename) : chip8(), running(false)
{
  if (!init())
  {
    fprintf(stderr, "Failed to initialize YACE DS!\n");
    return;
  }
  
  try
  {
    chip8.load_game(filename);
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
 *  Initialize resources
 */
bool YACEDS::init()
{
  videoSetMode(MODE_0_2D);
  vramSetBankA(VRAM_A_MAIN_BG);
  
  consoleDemoInit();
  
  if (!fatInitDefault())
  {
    fprintf(stderr, "Failed to initialize FAT!\n");
    return -1;
  }

  return true;
}

void YACEDS::loop()
{
}

void YACEDS::render()
{
}

/*
 *  Public methods
 */
void YACEDS::run()
{
  while (running)
  {
    loop();
    render();
    
    swiWaitForVBlank();
  }
}