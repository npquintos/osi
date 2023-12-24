#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "include/qreader.hpp"
#include "include/doctest.h"
#include <string.h>
#include <limits.h>
#include <string_view>

TEST_CASE("Test one-time utility call") {
    char line[QLINEMAX];
    const char *correct[] = {"First line 111", "Second line 222", "Third line 333 "};
    const char *correct2[] = {"", "", "", "First line 111", "Second line 222", "", "", "Third line 333 ", "", "",};
    SUBCASE("######## Test a well-formed file ########") {
        auto qr = Qreader("../include/test.txt");
        int i = 0;
        while (qr.read(line)) {
            CHECK(!strcmp(line, correct[i]));
            ++i;
        }
    }
    SUBCASE("######## Test a file with empty lines ########") {
        auto qr = Qreader("../include/test2.txt");
        int i = 0;
        while (qr.read(line)) {
            CAPTURE(line);
            CAPTURE(correct[i]);
            CHECK(!strcmp(line, correct2[i]));
            ++i;
        }
    }
    SUBCASE("######## Test seek and tell functionality ########") {
        auto qr = Qreader("../include/test3.txt");
        int i = 0;
        while (i < 1 && qr.read(line))
            ++i;
        long line1 = qr.tell();
        std::cout << line1 << " line1: " << line << std::endl;
        while (i < 12 && qr.read(line))
            ++i;
        long line12 = qr.tell();
        std::cout << line12 << " line12: " << line << std::endl;
        while (i < 45 && qr.read(line))
            ++i;
        long line45 = qr.tell();
        std::cout << line45 << " line45: " << line << std::endl;
        while (i < 50 && qr.read(line))
            ++i;
        long line50 = qr.tell();
        std::cout << line50 << " line50: " << line << std::endl;
        qr.seek(line1); 
        qr.read(line);
        CHECK(std::string_view("This is absolutely at line number 1") == line);
        qr.seek(line12); 
        qr.read(line);
        CHECK(std::string_view("This is absolutely at line number 12") == line);
        qr.seek(line45); 
        qr.read(line);
        CHECK(std::string_view("This is absolutely at line number 45") == line);
        qr.seek(line50); 
        qr.read(line);
        CHECK(std::string_view("This is absolutely at line number 50") == line);
    }
}
