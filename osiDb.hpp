/*
 * Utility to parse and read Osi databases
 * All headers only
 *
 * Usage:
 * db{OsiDb(/path/to/dump_file_name)};
 *          or
 * db = OsiDb(/path/to/dump_file_name);
 *      where dump file is created using
 *          dbdump -e -o <dump_file_name> <db_no>
 * then, to access record fields,
 * if (db.seek(object_name)) {
 *     while(const ValueGivenField &row = db.next()) {
 *         field_value = row["field_name"];
 *         do_something_with(field_value);
 *     }
 * }
 * if (db.seek(another_object_name) {
 *     while(const ValueGivenField &row = db.next()) {
 *         field_value = row["field_name"];
 *         do_something_with(field_value);
 *     }
 *
 * }
 *  ...
 */
#include <string>
#include <map>
#include <optional>
#include <string_view>
#include "include/qreader.hpp"
#include "include/split.hpp"

using FtellLocation = long;
using SplitRecordFields = std::span<std::string_view>;

// sz should be something like Emap::total_objects
// or in general <db name>::total_objects
// where 'db name' is Capitalized
// e.g. db = OsiDb<Emap::total_objcts>("emap.dat");
template <size sz>
class OsiDb {
    private:
        // index would be something like Opennet_breaker::no
        // or in general, <DB name>_<object name>::no
        // DB name is capitalized, all the rest lower case
        // so, if this is emap, object start for emap_switch
        // would be object_start[Emap_emap_switch::no] and
        // you should do std::fseek(object_start[Emap_emap_switch::no])
        // prior to reading.
        FtellLocation object_start[sz+1];
        char line[QLINEMAX];
        Qreader qr;
        std::string_view current_object{""};
        static constexpr std::array<char, 3> to_be_skipped{'\t', '*', '0', ' '};
        Qstring qs;
        void mark_object_start() {
            int count = 1;
            qr.read(line); // skip the DB header
            while(qr.read(line)) {
                bool should_skip = false;
                for (const auto &x : to_be_skipped) {
                    if (line[0] == x) {
                        should_skip = true;
                        break;
                    }
                }
                if (should_skip)
                    continue;
                if (!line[0]) {
                    continue;
                }
                // at this point you are at the object header
                qr.read(line); // skip the comment
                object_start[count] = qr.tell();
                ++count;
            }
        };

    public:
        std::optional<ValueGivenField> result;
        OsiDb(const char* dump_file_name) :
            qr{Qreader(dump_file_name)}
        {
            mark_object_start();
        };

        OsiDb(const std::string dump_file_name) :
            qr{Qreader(dump_file_name.c_str())}
        {
            mark_object_start();
        };

        bool seek(const char *object_name) {
            return seek(std::string{object_name});
        };

        bool seek(const std::string &object_name) {
            auto it = object_start.find(object_name);
            if (it != object_start.end()) {
                qr.seek(object_start[object_name]);
                current_object = object_name;
                return true;
            }
            return false;
        };

        std::optional<ValueGivenField> next() {
            result.reset();
            while (qr.read(line) && *line == '*');
            if (qr.eol || *line == '0') {
                return std::nullopt;
            }
            auto line_vector = qs(line).split();
            auto n = line_vector.size();
            for(decltype(n) i=1; i<n; ++i) {
                result[fields[current_object][i]] = line_vector[i];
            }
            return result;
        };

        std::vector<std::string_view> get_fields(const char *object_name) {
            return fields[std::string_view(object_name)];
        };
};
