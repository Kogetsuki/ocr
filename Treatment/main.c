#include "toBlackWhite.h"
#include "new_try.h"
#include <err.h>

int main(int argc, char *argv[]) {
    if (argc < 2)
        errx(1, "Not enought arguments : prototype <./main `char *filename`> !");
    if (argc > 2)
        errx(1, "Too much arguments : prototype <./main `char *filename`> !");
    toBlackWhite(argv[1]);
    newTry(".BaW");
    return 0;
}
