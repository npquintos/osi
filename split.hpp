#include <cstring>
#include <string.h>
#include <stdio.h>
#include <span>
#include <string>
#include <iostream>
#include <array>
#include <string_view>
#include <algorithm>
#include "include/doctest.h"

#define MAX_SPLIT_WORDS 1024
#define MAX_JOINED_CHARACTERS 2048
#define MAX_JOINING_CHAR 80

using ReadOnlyString = std::string_view;
using SplitWords = std::span<ReadOnlyString>;

class Qstring {
    // Usage:
    // auto xsplit = Qstring(line).split(delimeter);
    // for (auto word : xsplit) {
    //      std::cout << word << std::endl
    // }
    // if 'word' is to be split, 'ptr' below will
    // hold the starting indexes of the start of
    // the word. 'word' will then be modified to
    // be delimited with \0. For example, say you have
    // this word and you want to split with 'abcd' as
    // delimiter, similar to word.split('abcd') in python
    // for splitting -> "abcdFirstabcdSecondabcdThirdabcd"
    //                   01234567890123456789012345678901
    //                              1         2         3
    //                   ^   ^        ^         ^        ^
    //                   |   |        |         |        |
    // ptr --------------+---+--------+---------+--------+
    //                 s+0,0
    //                     s+4,5
    // modified word ->"\0bcdFirst\0bcdSecond\0bcdThird\0bcd"
    ReadOnlyString line_to_process;
    std::array<ReadOnlyString, MAX_SPLIT_WORDS> arr_lines_to_process;
    char arr_joining_char[MAX_JOINING_CHAR];
    SplitWords lines_to_process{};
    SplitWords split_words_result;
    ReadOnlyString strip_default {" \t\n\r"};
    ReadOnlyString &for_stripping {strip_default};
    ReadOnlyString join_default {""};
    ReadOnlyString joining_word;
    char joined_words[MAX_JOINED_CHARACTERS];

    // This is "greedy"  split where it tries to match as much delimeter as possible
    // instead of just once. qs(",,,a,,b,,,c,,,").gsplit(",") will return
    // {"", a", "b", "c", ""}
    const SplitWords &v_gsplit(const ReadOnlyString delim) {
        int index = 0; // tracks where we are in the line to split
        int split_words_count = 0;
        auto pdelim0 = delim.begin();
        const int delim_length = delim.length();
        int match = 0;
        const auto v_word_start = line_to_process.begin();
        int prev_index = 0;
        for (auto line_char : line_to_process) {
            // std::cout << line_char << *(pdelim0+match)<< " ";
            ++index;
            if (line_char == *(pdelim0+match)) {
                ++match;
                if (match == delim_length) {
                    // std::cout << "got a match, pushing" << std::basic_string_view(v_word_start+prev_index, index-prev_index-delim_length) << std::endl;
                    if ( !split_words_count ) {
                        split_words[split_words_count] = std::basic_string_view(v_word_start+prev_index, index-prev_index-delim_length);
                        ++split_words_count;
                    }
                    else {
                        if (delim_length != index-prev_index) {
                            // std::cout << "before inserting" << std::endl;
                            // for (auto xw : split_words) {
                                // std::cout << ">" << xw << "<" << std::endl;
                            // }
                            split_words[split_words_count] = std::basic_string_view(v_word_start+prev_index, index-prev_index-delim_length);
                            ++split_words_count;
                            // std::cout << "after inserting" << std::endl;
                            // for (auto xw : split_words) {
                                // std::cout << ">" << xw << "<" << std::endl;
                            // }
                        }
                    }
                    prev_index = index;
                    match = 0;
                }
            }
            else {
                match = 0;
            }
        }
        // std::cout << "reached the end" << std::endl;
        // for (auto xw : split_words) {
        //     std::cout << ">" << xw << "<" << std::endl;
        // }
        // std::cout << "indeces" << index << " " << prev_index << " " << delim_length;
        if (prev_index != index) {
            split_words[split_words_count] = std::basic_string_view(v_word_start+prev_index, index-prev_index);
            ++split_words_count;
        }
        else {
            split_words[split_words_count] = std::basic_string_view(v_word_start+prev_index, 0);
            ++split_words_count;
        }
        // std::cout << "final" << std::endl;
        // for (auto xw : split_words) {
        //     std::cout << ">" << xw << "<" << std::endl;
        // }
        split_words_result = SplitWords(split_words.begin(), split_words_count);
        return split_words_result;
    };

    const SplitWords &v_wsplit() {
        int index = 0; // tracks where we are in the line to split
        constexpr ReadOnlyString delim {" \t"};
        int split_words_count = 0;
        int match = 0;
        const auto v_word_start = line_to_process.begin()-1;
        int prev_index = 0;
        for (auto line_char : line_to_process) {
            ++index;
            if (std::any_of(delim.begin(), delim.end(), [line_char](char x) {return x == line_char; })) {
                ++match;
                continue;
            }
            else {
                if (match) {
                    if (match != index-1) {
                        split_words[split_words_count] = std::basic_string_view(v_word_start+prev_index, index-prev_index-match);
                        ++split_words_count;
                    }
                    prev_index = index;
                    match = 0;
                }
                else {
                    match = 0;
                }
            }
        }
        // std::cout << index << " " << prev_index << " " << delim_length;
        if ((index-prev_index) != match) {
            split_words[split_words_count] = std::basic_string_view(v_word_start+prev_index, index-prev_index);
            ++split_words_count;
        }
        split_words_result = SplitWords(split_words.begin(), split_words_count);
        return split_words_result;
    };


    const SplitWords &v_split(const ReadOnlyString delim) {
        int index = 0; // tracks where we are in the line to split
        int split_words_count = 0;
        auto pdelim0 = delim.begin();
        const int delim_length = delim.length();
        int match = 0;
        const auto v_word_start = line_to_process.begin();
        int prev_index = 0;
        for (auto line_char : line_to_process) {
            // std::cout << line_char << *(pdelim0+match)<< " ";
            ++index;
            if (line_char == *(pdelim0+match)) {
                ++match;
                if (match == delim_length) {
                    split_words[split_words_count] = std::basic_string_view(v_word_start+prev_index, index-prev_index-delim_length);
                    ++split_words_count;
                    prev_index = index;
                    match = 0;
                }
            }
            else {
                match = 0;
            }
        }
        // std::cout << index << " " << prev_index << " " << delim_length;
        if (prev_index == index) {
            split_words[split_words_count] = std::basic_string_view(v_word_start+prev_index, 0);
            ++split_words_count;
        }
        else {
            split_words[split_words_count] = std::basic_string_view(v_word_start+prev_index, index-prev_index);
            ++split_words_count;
        }
        split_words_result = SplitWords(split_words.begin(), split_words_count);
        return split_words_result;
    };

    bool isToBeTrimmed(const char c) const {
        for(const char w : for_stripping) {
            if(c == w)
                return true;
        }
        return false;
    };

    ReadOnlyString v_strip() const {
        auto ppre = line_to_process.begin();
        auto ppost = line_to_process.end();
        if (ppre == ppost) {
            return ReadOnlyString(ppre, 0);
        }
        // strip first from the front
        while ( ppre <= ppost && isToBeTrimmed(*ppre)) {
            ++ppre;
        }
        // Beware of situation where everything are to be stripped.
        // If so, stop!
        if (ppre == ppost) {
            --ppre;
            return ReadOnlyString(ppre, 0);
        }
        // Then, strip coming from the end, going forward
        // Take note that end() is pointing AFTER the last
        // characted and therefore have to be moved one
        // character back so that it points to the last
        // character of the string before parsing backwards.
        --ppost;
        while ( ppre < ppost && isToBeTrimmed(*ppost)) {
            --ppost;
        }
        return ReadOnlyString(ppre, ppost-ppre+1);
    };

    int char_count_of_result() {
        const int no_elements = lines_to_process.size();
        int array_size = 0;
        for (const auto &line: lines_to_process) {
            array_size += line.size();
        }
        return array_size+(no_elements-1)*joining_word.size();
    };

    std::string v_join() {
        auto pChar = joined_words;
        int count = 0;
        decltype(lines_to_process.size()) processed_elements = 0;
        const decltype(lines_to_process.size()) no_elements = lines_to_process.size();
        // std::cout<< "inside v_join now, no_elements is: " << no_elements << std::endl;
        if (char_count_of_result() >= MAX_JOINED_CHARACTERS) {
            std::cout << "Error!!! MAX_JOINED_CHARACTERS exceeded!" << std::endl;
            exit(1);
        }
        for (const auto word : lines_to_process) {
            // std::cout<< "processing word " << word << std::endl;
            for (const auto c : word) {
                *pChar++ = c;
                ++count;
            }
            // std::cout<< "so far >>" << std::string_view(joined_words, count) << std::endl;
            if (++processed_elements != no_elements) {
                // std::cout<< "appending delimit " << joining_word << std::endl;
                for (const auto c : joining_word) {
                    *pChar++ = c;
                    ++count;
                }
            // std::cout<< "so far >>" << std::string_view(joined_words, count) << std::endl;
            }
        }
        // std::cout<< "before move >>" << std::string_view(joined_words, count) << std::endl;
        return std::move(std::string(joined_words, count));
    };

    public:
        Qstring() {};
        std::array<ReadOnlyString, MAX_SPLIT_WORDS> split_words;

        Qstring &operator()(const char* line) {
            line_to_process = ReadOnlyString(line, strlen(line));
            return *this;
        };

        Qstring &operator()(ReadOnlyString line) {
            line_to_process = line;
            return *this;
        };

        Qstring &operator()(const std::vector<const char*> &lines) {
            auto lines_size = lines.size();
            for (decltype(lines_size) i=0; i<lines_size; ++i) {
                arr_lines_to_process[i] = std::string_view(lines[i]);
            }
            lines_to_process = SplitWords(arr_lines_to_process.begin(), lines_size);
            return *this;
        };

        Qstring &operator()(std::vector<ReadOnlyString> &lines) {
            lines_to_process = SplitWords(lines.begin(), lines.size());
            return *this;
        };

        Qstring &operator()(const std::vector<std::string> &lines) {
            auto lines_size = lines.size();
            decltype(lines_size) i=0;
            // std::cout << "Operator () called, lines size is: " << lines.size() << std::endl;
            for (const auto& line : lines) {
                arr_lines_to_process[i] = std::string_view(line);
                ++i;
            }
            // std::cout << "for loop completed " << std::endl;
            lines_to_process = SplitWords{arr_lines_to_process.begin(), lines_size};
            // std::cout << "immediately before return "<< lines_size << std::endl;
            return *this;
        };

        // ****************************************************************
        // ****************   split ***************************************
        // ****************************************************************
        const SplitWords &gsplit() {
            return v_wsplit();
        };

        const SplitWords &split(const char *delim=" ") {
            return v_split(std::basic_string_view(delim, strlen(delim)));
        };

        const SplitWords &split(const ReadOnlyString &delim) {
            return v_split(delim);
        };

        const SplitWords &gsplit(const char *delim) {
            return v_gsplit(std::basic_string_view(delim, strlen(delim)));
        };

        const SplitWords &gsplit(const ReadOnlyString &delim) {
            return v_gsplit(delim);
        };

        // ****************************************************************
        // ****************   split chained *******************************
        // ****************************************************************
        Qstring &gsplitc() {
            lines_to_process = v_wsplit();
            return *this;
        };

        Qstring &splitc(const char *delim=" ") {
            lines_to_process = v_split(std::basic_string_view(delim, strlen(delim)));
            return *this;
        };

        Qstring &splitc(const ReadOnlyString &delim) {
            lines_to_process = v_split(delim);
            return *this;
        };

        Qstring &gsplitc(const char *delim) {
            lines_to_process = v_gsplit(std::basic_string_view(delim, strlen(delim)));
            return *this;
        };

        Qstring &gsplitc(const ReadOnlyString &delim) {
            lines_to_process = v_gsplit(delim);
            return *this;
        };


        // ****************************************************************
        // ****************   strip ***************************************
        // ****************************************************************

        ReadOnlyString strip() {
            for_stripping = strip_default;
            return v_strip();
        };

        ReadOnlyString strip(const char *to_be_stripped_out) {
            for_stripping = ReadOnlyString(to_be_stripped_out, strlen(to_be_stripped_out));
            return v_strip();
        };

        ReadOnlyString strip(const ReadOnlyString &to_be_stripped_out) {
            for_stripping = to_be_stripped_out;
            return v_strip();
        };


        // ****************************************************************
        // ****************   strip chained  ******************************
        // ****************************************************************

        Qstring &stripc() {
            for_stripping = strip_default;
            line_to_process = v_strip();
            return *this;
        };

        Qstring &stripc(const char *to_be_stripped_out) {
            for_stripping = ReadOnlyString(to_be_stripped_out, strlen(to_be_stripped_out));
            line_to_process = v_strip();
            return *this;
        };

        Qstring &stripc(const ReadOnlyString &to_be_stripped_out) {
            for_stripping = to_be_stripped_out;
            line_to_process = v_strip();
            return *this;
        };

        // ****************************************************************
        // ****************   join  ***************************************
        // ****************************************************************

        std::string join() {
            // printf("\n\n+++++++++++++++ join() entered +++++++++++++++++\n\n");
            joining_word = join_default;
            return v_join();
        };

        std::string join(const char *to_be_joined) {
            // printf("\n\n+++++++++++++++ join with const char * called+++++++++++++++++\n\n");
            if (strlen(to_be_joined)+1 > MAX_JOINING_CHAR) {
                std::cout << "Increase size of MAX_JOINING_CHAR !" << std::endl;
                exit(1);
            }
            strcpy(arr_joining_char, to_be_joined);
            joining_word = ReadOnlyString(arr_joining_char, strlen(to_be_joined));
            // std::cout << "join with char* exit" << std::endl;
            return v_join();
        };

        std::string join(const ReadOnlyString &to_be_joined) {
            // printf("\n\n+++++++++++++++ join with ReadOnlyString called+++++++++++++++++\n\n");
            joining_word = to_be_joined;
            return v_join();
        };

        // ****************************************************************
        // ****************   join chained ********************************
        // ****************************************************************

        Qstring &joinc() {
            joining_word = join_default;
            line_to_process = v_join();
            return *this;
        };

        Qstring &joinc(const char *to_be_joined) {
            joining_word = ReadOnlyString(to_be_joined, strlen(to_be_joined));
            line_to_process = v_join();
            return *this;
        };

        Qstring &joinc(const ReadOnlyString &to_be_joined) {
            joining_word = to_be_joined;
            line_to_process = v_join();
            return *this;
        };
};
