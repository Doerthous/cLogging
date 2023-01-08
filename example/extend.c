#include "Logging.h"

int main()
{
    char a[256] = { 1, 2, 2, 3 };
    const char *ret[] = { "ok", "ok", "error", "ok" };
    for (int i = 0; i < 4; ++i) {
        LOG_IF_CHANGED(a[i], "%s", ret[i]);
    }

    int b = 1;
    LOG_IF(b == 1, "ok");
    LOG_IF(b == 0, "error");
    b = !b;
    LOG_IF(b == 1, "error");
    LOG_IF(b == 0, "ok");

    for (int i = 0; i < 256; ++i) {
        a[i] = i;
    }

    LOG_BUFFER("a: ", a, 256);

    return 0;
}
