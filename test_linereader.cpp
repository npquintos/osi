#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../../include/qreader.hpp"
#include "../../include/doctest.h"
#include "string.h"

TEST_CASE("Test one-time utility call") {
    char line[QLINEMAX];
    const char *correct[] = {"First line 111", "Second line 222", "Third line 333 "};
    const char *correct2[] = {"", "", "", "First line 111", "Second line 222", "", "", "Third line 333 ", "", "",};
    SUBCASE("######## Test a well-formed file ########") {
        auto qr = Qreader("../data/test.txt");
        int i = 0;
        while (qr.read(line)) {
            CHECK(!strcmp(line, correct[i]));
            ++i;
        }
    }
    SUBCASE("######## Test a file with empty lines ########") {
        auto qr = Qreader("../data/test2.txt");
        int i = 0;
        while (qr.read(line)) {
            CAPTURE(line);
            CAPTURE(correct[i]);
            CHECK(!strcmp(line, correct2[i]));
            ++i;
        }
    }
}
