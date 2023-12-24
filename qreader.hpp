#include<cstdio>
#include<iostream>
#include<stdexcept>
#include<filesystem>
#include<cstring>
#include <limits.h>
#define QMAXBUFFER 10240 // chosen for performance
#define QLINEMAX 1024 // assumes that one line is less than 1023 chars

class Qreader {
    private:
        ulong line_count = 0;
        ulong bytes_read = 0;
        ulong line_start_byte_position_in_file;
        ulong line_max = ULONG_MAX;
        std::FILE *fin;
        int bufInd = 0;
        int bufRemaining = 0;
        int lineInd = 0;
        char buffer[QMAXBUFFER];
        char last_char = 'n';
        char line[QLINEMAX];
    public:
        bool eol = false;
        // std::FILE *fin;
        Qreader(const char* filename) : eol(false) {
            if(!(fin = std::fopen(filename, "rb"))) {
                exit(1);
            }
        };
        Qreader() = default;
        Qreader(const Qreader &other) = delete;
        Qreader(Qreader &&other) = delete;
        Qreader &operator=(const Qreader &other) = delete;
        Qreader &operator=(Qreader &&other) = delete;
        ~Qreader() { std::fclose(fin); };

        ulong get_byte_offset() const {
            return line_start_byte_position_in_file;
        };
        // IMPORTANT: user should declare at his side "char out_buf[QLINEMAX];"
        bool read(char *out_buf) {
            int out_index = 0;
            while (!eol) {
                if (!bufRemaining) {
                    bufRemaining = fread(buffer, sizeof(char), QMAXBUFFER, fin);
                    if (!bufRemaining) {
                        eol = true;
                        line_max = line_count;
                        if (bufInd) { // there is still an unreturned line
                            out_buf[out_index] = '\0';
                            if (buffer[bufInd] == '\n') {

                                line_start_byte_position_in_file = bytes_read - strlen(out_buf)-1;
                                // std::cout << "returned from CR but eof" << std::endl;
                            }
                            else {
                                line_start_byte_position_in_file = bytes_read - strlen(out_buf);
                                // std::cout << "returned without CR but eof" << std::endl;
                            }
                        }
                        return false;
                    }
                    bufInd = 0;
                }
                while(bufRemaining && buffer[bufInd] != '\n' && out_index < QLINEMAX-1) {
                    out_buf[out_index++] = buffer[bufInd++];
                    ++bytes_read;
                    --bufRemaining;
                }
                if (bufRemaining) {
                    --bufRemaining;
                    ++bufInd;
                    ++bytes_read;
                    out_buf[out_index] = '\0';
                    line_start_byte_position_in_file = bytes_read - 1 - strlen(out_buf);
                    ++line_count;
                    // std::cout << "returned from normal CR" << std::endl;
                    return true;
                }
            }
            eol = true;
            return false;
        };
        
        // offset is seek offset  is the number of lines from beginning
        void seek(const long offset) {
            std::fseek(fin, offset, SEEK_SET);
            bufRemaining = 0;
        };

        // this is the byte offset from the beginning of file
        // for the currently read line. Take note that you read
        // the line first to examine if this is the line you
        // are looking for and so, you need to calculate backwards
        // to determine the beginning of this line.
        long tell(void) const {
            return line_start_byte_position_in_file;
        };
};
