#ifndef MediaTypes_H_
#define MediaTypes_H_
#include <string.h>

class MediaTypes {
  public:
    static const char* getType(const char * path);
    static const char* getExtension(const char * type, int skip = 0);

  private:
    struct entry {
      const char* fileExtension;
      const char* mimeType;
    };
    static MediaTypes::entry types[350];
    static int strcmpi(const char *s1, const char *s2);
};

#endif
