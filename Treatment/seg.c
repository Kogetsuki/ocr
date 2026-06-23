#include "seg.h"
#include "toBlackWhite.h"
#include "new_try.h"

void seg(char *string) {
    toBlackWhite(string);
    newTry("../Treatment/.BaW");
}
