#include "data.h"



int main() {
    FILE *fp;
    fp = createFileWithSuffix("aviv.barak", "etedgui");
    fclose(fp);
    return 0;
}