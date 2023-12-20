#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../../include/split.hpp"
#include "../../include/doctest.h"

using TupleVectorStringString = std::tuple< std::vector<std::string>, std::string>;

using VectorTupleStringString_view = std::vector<std::tuple<std::string, std::string_view>>;

using TupleVectorCstrString = std::tuple<std::vector<const char *>, std::string >;

TEST_CASE("(I). split tests") {
    auto qs = Qstring();
    SUBCASE("(1). split with char * line_to_split and char * delimiter") {
        const char *test = "abcdFirstabcdSecondabcdThirdabcabcd";
        std::vector<std::string_view> correct_answer {
                                                        std::string_view(""),
                                                        std::string_view("First"),
                                                        std::string_view("Second"),
                                                        std::string_view("Thirdabc"),
                                                        std::string_view("")
                                                     };
        auto split_result = qs(test).split("abcd");
        for (long unsigned int i=0; i<correct_answer.size(); ++i) {
            CHECK(split_result[i] == correct_answer[i]);
        }
    }
    
    SUBCASE("(2). split with std::string line_to_split and std::string_view delimiter") {
        std::string test{"FirstabcdSecondabcdThird"};
        std::vector<std::string_view> correct_answer {
                                                        std::string_view("First"),
                                                        std::string_view("Second"),
                                                        std::string_view("Third")
                                                     };
        std::string_view delimeter{"abcd"};
        auto split_result = qs(test).split(delimeter);
        for (long unsigned int i=0; i<correct_answer.size(); ++i) {
            CHECK(split_result[i] == correct_answer[i]);
        }
    }
    
    SUBCASE("(3). split with std::string line_to_split and std::string_view delimiter") {
        std::string test{"abcdabcdabcdabcd"};
        std::vector<std::string_view> correct_answer {
                                                        std::string_view(""),
                                                        std::string_view(""),
                                                        std::string_view(""),
                                                        std::string_view(""),
                                                        std::string_view("")
                                                     };
        std::string_view delimeter{"abcd"};
        auto split_result = qs(test).split(delimeter);
        CHECK(split_result.size() == correct_answer.size());
        for (long unsigned int i=0; i<correct_answer.size(); ++i) {
            CHECK(split_result[i] == correct_answer[i]);
        }
    }
    
    SUBCASE("(4). split with std::string line_to_split and no supplied delimeter") {
        std::string test{"abc def g x yz "};
        std::vector<std::string_view> correct_answer {
                                                        std::string_view("abc"),
                                                        std::string_view("def"),
                                                        std::string_view("g"),
                                                        std::string_view("x"),
                                                        std::string_view("yz"),
                                                        std::string_view("")
                                                     };
        auto split_result = qs(test).split();
        CHECK(split_result.size() == correct_answer.size());
        for (long unsigned int i=0; i<correct_answer.size(); ++i) {
            CHECK(split_result[i] == correct_answer[i]);
        }
    }
    
    SUBCASE("(5). split with std::string line_to_split and std::string_view delimiter") {
        std::string test{"abc"};
        std::vector<std::string_view> correct_answer {
                                                        std::string_view("abc")
                                                     };
        std::string_view delimeter{"abcd"};
        auto split_result = qs(test).split(delimeter);
        CHECK(split_result.size() == correct_answer.size());
        for (long unsigned int i=0; i<correct_answer.size(); ++i) {
            CHECK(split_result[i] == correct_answer[i]);
        }
    }
    
    SUBCASE("(6). test that it correctly revert to default afterwards") {
        std::vector<std::string_view> correct_answer {
                                                        std::string_view("abc"),
                                                        std::string_view("def"),
                                                        std::string_view("g"),
                                                        std::string_view("x"),
                                                        std::string_view("yz"),
                                                        std::string_view("")
                                                     };
        auto split_result = qs("abc def g x yz ").split();
        CHECK(split_result.size() == correct_answer.size());
        for (long unsigned int i=0; i<correct_answer.size(); ++i) {
            CHECK(split_result[i] == correct_answer[i]);
        }
    }
    
    SUBCASE("(7). split with C-string line_to_split and comma delimeter") {
        std::vector<std::string_view> correct_answer {
                                                        std::string_view("abc"),
                                                        std::string_view("def"),
                                                        std::string_view("g"),
                                                        std::string_view("x"),
                                                        std::string_view("yz"),
                                                        std::string_view("")
                                                     };
        auto split_result = qs("abc,def,g,x,yz,").split(",");
        CHECK(split_result.size() == correct_answer.size());
        for (long unsigned int i=0; i<correct_answer.size(); ++i) {
            CHECK(split_result[i] == correct_answer[i]);
        }
    }
    SUBCASE("(8). split with C-string line_to_split with no delimeter") {
        std::vector<std::string_view> correct_answer {
                                                        std::string_view("abc"),
                                                     };
        auto split_result = qs("abc").split(",");
        CHECK(split_result.size() == correct_answer.size());
        for (long unsigned int i=0; i<correct_answer.size(); ++i) {
            CHECK(split_result[i] == correct_answer[i]);
        }
    }
}

TEST_CASE("(II). strip test") {
    auto qs = Qstring();
    SUBCASE("(9). strip words with leading and trailing spaces") {
        VectorTupleStringString_view testValue_correctAnswer {
            {"hello word", "hello word"},
            {"  hello word      ", "hello word"},
            {"\thello   word      \n", "hello   word"},
            {"\t\t   l          \n \n    ", "l"},
            {"                  ", ""},
            {"", ""}
        };
        for (const auto& [testValue, correctAnswer] : testValue_correctAnswer) {
            CHECK(correctAnswer == qs(testValue).strip());
        }
    }

    SUBCASE("(10). strip words with leading and trailing specified characters") {
        VectorTupleStringString_view testValue_correctAnswer {
            {"hello word", "hello word"},
            {"00hello word######", "hello word"},
            {"##hello   word########", "hello   word"},
            {"0####00l000000#############", "l"},
            {"00000######00#0", ""},
            {"", ""}
        };
        for (const auto& [testValue, correctAnswer] : testValue_correctAnswer) {
            CHECK(correctAnswer == qs(testValue).strip("0#"));
        }
    }

    SUBCASE("(11). Test that it reverts to default without arguments") {
        VectorTupleStringString_view testValue_correctAnswer {
            {"hello word", "hello word"},
            {"  hello word      ", "hello word"},
            {"\thello   word      \n", "hello   word"},
            {"\t\t   l          \n \n    ", "l"},
            {"                  ", ""},
            {"", ""}
        };
        for (const auto& [testValue, correctAnswer] : testValue_correctAnswer) {
            CHECK(correctAnswer == qs(testValue).strip());
        }
    }
}

TEST_CASE("(III). join test") {
    auto qs = Qstring();
    SUBCASE("(12). Join vector of C-string words with C-string delimiter xyz") {
        TupleVectorCstrString testValue_correctAnswer {
            {
                "", 
                "FIRST", 
                "", 
                "", 
                "SECOND", 
                "" 
            },                  std::string("xyzFIRSTxyzxyzxyzSECONDxyz")
        };  // using "xyz" as delimiter
        // IMPORTANT!!! using const auto & below will cause segmentation fault
        const auto [testValue, correctAnswer] = testValue_correctAnswer;
        CHECK(correctAnswer == qs(testValue).join("xyz"));
    }
    SUBCASE("(13). Join vector of C-string words with C-string delimiter ~") {
        TupleVectorCstrString testValue_correctAnswer {
            {
                "abc", 
                "FIRST", 
                "def", 
                "SECOND", 
            },                  std::string("abc~FIRST~def~SECOND")
        };  // using "xyz" as delimiter
        // IMPORTANT!!! using const auto & below will cause segmentation fault
        const auto [testValue, correctAnswer] = testValue_correctAnswer;
        CHECK(correctAnswer == qs(testValue).join("~"));
    }
    SUBCASE("(14). Join vector of string words with C-string delimiter") {
        TupleVectorStringString testValue_correctAnswer {
            {
                std::string(""), 
                std::string("FIRST"), 
                std::string(""), 
                std::string(""), 
                std::string("SECOND"), 
                std::string("") 
            },                  std::string("xyzFIRSTxyzxyzxyzSECONDxyz")
        };  // using "xyz" as delimiter
        // IMPORTANT!!! using const auto & below will cause segmentation fault
        const std::tuple [testValue, correctAnswer] = testValue_correctAnswer;
        auto calc_value = qs(testValue).join("xyz");
        // std::cout << "Returned value in main: " << calc_value << std::endl;
        // std::cout << "correctAnswer: " << correctAnswer << std::endl;
        CHECK(correctAnswer == calc_value);
        // std::cout << "After CHECK" << std::endl;
    }

    SUBCASE("(15). Join vector of string words with string_view delimiter") {
        // std::cout << "NEXT subcase" << std::endl;
        TupleVectorStringString testValue_correctAnswer {
            {
                std::string(""), 
                std::string("FIRST"), 
                std::string(""), 
                std::string(""), 
                std::string("SECOND"), 
                std::string("") 
            },                  std::string("xyzFIRSTxyzxyzxyzSECONDxyz")
        };  // using "xyz" as delimiter
        // IMPORTANT!!! using const auto & below will cause segmentation fault
        const std::tuple [testValue, correctAnswer] = testValue_correctAnswer;
        CHECK(correctAnswer == qs(testValue).join(std::string_view("xyz")));
    }

    SUBCASE("(16). Join vector of string words with default delimiter") {
        TupleVectorStringString testValue_correctAnswer {
            {
                std::string(""), 
                std::string("FIRST"), 
                std::string(""), 
                std::string(""), 
                std::string("SECOND"), 
                std::string("") 
            },                  std::string("FIRSTSECOND")
        };  // No delimeter argument is supplied; same as concatenate
        // IMPORTANT!!! using const auto & below will cause segmentation fault
        const std::tuple [testValue, correctAnswer] = testValue_correctAnswer;
        CHECK(correctAnswer == qs(testValue).join());
    }
}

TEST_CASE("(IV). gsplit tests") {
    auto qs = Qstring();
    SUBCASE("(17). split with char * line_to_split and char * delimiter part1") {
        const char *test = "abcdabcdabcdFirstabcdabcdabcdabcdSecondabcdabcdabcdThirdabcdabcd";
        std::vector<std::string_view> correct_answer {
                                                        std::string_view(""),
                                                        std::string_view("First"),
                                                        std::string_view("Second"),
                                                        std::string_view("Third"),
                                                        std::string_view("")
                                                     };
        auto split_result = qs(test).gsplit("abcd");
        for (long unsigned int i=0; i<correct_answer.size(); ++i) {
            CHECK(split_result[i] == correct_answer[i]);
        }
    }

    SUBCASE("(18). split with char * line_to_split and char * delimiter part2") {
        const char *test = "abcdabcdabcdFirstabcdabcdabcdabcSecondabcdabcdabcdThirdabcabcda";
        std::vector<std::string_view> correct_answer {
                                                        std::string_view(""),
                                                        std::string_view("First"),
                                                        std::string_view("abcSecond"),
                                                        std::string_view("Thirdabc"),
                                                        std::string_view("a")
                                                     };
        auto split_result = qs(test).gsplit("abcd");
        for (long unsigned int i=0; i<correct_answer.size(); ++i) {
            // std::cout << i << ">" << split_result[i] << "< >" << correct_answer[i] << "<" << std::endl;
            CHECK(split_result[i] == correct_answer[i]);
        }
    }

    SUBCASE("(19). split with char * line_to_split and using whitespace delim") {
        const char *test = " \t  \tFirst Second\t\t  Third ";
        std::string test2{" \t  \tFirst Second\t\t  Third "};
        std::string test3{" \t  \tFirst Second\t\t  Third"};
        std::vector<std::string_view> correct_answer {
                                                        std::string_view("First"),
                                                        std::string_view("Second"),
                                                        std::string_view("Third"),
                                                     };
        auto split_result = qs(test).gsplit();
        for (long unsigned int i=0; i<correct_answer.size(); ++i) {
            // std::cout << i << ">" << split_result[i] << "< >" << correct_answer[i] << "<" << std::endl;
            CHECK(split_result[i] == correct_answer[i]);
        }
        split_result = qs(test2).gsplit();
        for (long unsigned int i=0; i<correct_answer.size(); ++i) {
            // std::cout << i << ">" << split_result[i] << "< >" << correct_answer[i] << "<" << std::endl;
            CHECK(split_result[i] == correct_answer[i]);
        }
        split_result = qs(test3).gsplit();
        for (long unsigned int i=0; i<correct_answer.size(); ++i) {
            // std::cout << i << ">" << split_result[i] << "< >" << correct_answer[i] << "<" << std::endl;
            CHECK(split_result[i] == correct_answer[i]);
        }
    }
}

TEST_CASE("(V). Chaining test") {
    auto qs = Qstring();
    SUBCASE("(20). chaining strip, split, then join") {
        const char* test_value = "  \t abcFirstabcabcSecondabcThirdabcabc   \t     \n";
        const std::string correct_answer {"xyFirstxyxySecondxyThirdxyxy"};
        // note that the last  in the chain should not be the "chained" version
        auto calculated_value = qs(test_value).stripc().splitc("abc").join("xy");
        CHECK(correct_answer == calculated_value);
    }
}









