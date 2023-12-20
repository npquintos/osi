#include<stdio.h>
#include<stdexcept>
#include<filesystem>
#define QMAXBUFFER 10240 // chosen for performance
#define QLINEMAX 1024 // assumes that one line is less than 1023 chars

class Qreader {
    private:
        char buffer[QMAXBUFFER];
        char line[QLINEMAX];
        FILE *fin;
        int bufInd = 0;
        int bufRemaining = 0;
        int lineInd = 0;
    public:
        bool eol = false;
        Qreader(const char* filename) : eol(false) {
            if(!(fin = fopen(filename, "r"))) {
                exit(1);
            }
        };
        Qreader() = default;
        Qreader(const Qreader &other) = delete;
        Qreader(Qreader &&other) = delete;
        Qreader &operator=(const Qreader &other) = delete;
        Qreader &operator=(Qreader &&other) = delete;
        ~Qreader() { fclose(fin); };

        // IMPORTANT: user should declare at his side "char out_buf[QLINEMAX];"
        bool read(char *out_buf) {
            int out_index = 0;
            if (!bufRemaining) {
                bufRemaining = fread(buffer, sizeof(char), QMAXBUFFER, fin);
                if (!bufRemaining) {
                    eol = true;
                    return false;
                }
            }
            while(bufRemaining && buffer[bufInd] != '\n' && out_index < QLINEMAX-1) {
                out_buf[out_index] = buffer[bufInd];
                --bufRemaining;
                ++bufInd;
                ++out_index;
            }
            --bufRemaining;
            ++bufInd;
            out_buf[out_index] = '\0';
            return true;
        };
        
        void seek(const long offset) {
            std::fseek(fin, offset, SEEK_SET);
            eol = false;
        };

        long tell(void) const {
            return std::ftell(fin);
        };
};
