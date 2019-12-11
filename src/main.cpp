#include "scan.h"

int main() {

    iiran::Scan *s = iiran::create_scan("/Users/yiranfeng/repo/cuddly-potato/a.cpp");
    s->init();
    s->run();

    return 0;
}
