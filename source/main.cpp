#include <nds.h>

#include "main.h"
#include "YACEDS.h"

int main(int argc, char** argv)
{ 
  YACEDS* yaceds = 0;
  
  if (argc > 1)
    yaceds = new YACEDS(argv[1]);
  else
    yaceds = new YACEDS();
    
  /*
  printf("argc: %i\n", argc);
  for (int i = 0; i < argc; i++)
    printf("argv[%i]: %s\n", i, argv[i]);
  */
  
  if (argc > 2)
    yaceds->read_configuration(argv[2]);
  else
  {
    char filename[] = "config.ini";
    yaceds->read_configuration(filename);
  }
    
    
  yaceds->run();
  
  if (yaceds)
    delete yaceds;
  
  return 0;
}

/*
const char* browse_fat(const char* path)
{
  DIR* dir = opendir(path);
  struct dirent* entry;
  struct stat statbuf;
  
  if (dir)
  {
    while ((entry = readdir(dir)) != 0)
    {
      stat(entry->d_name, &statbuf);
      
      printf("%s\n", entry->d_name);
    }
  }
  else
    printf("Couldn't open directory: %s\n", path);
    
  return "";
}
*/
