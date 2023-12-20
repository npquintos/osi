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

using ValueGivenField = std::map<std::string, std::string>;
using FieldsGivenObject = std::map<std::string_view, std::vector<std::string_view>>;
using ObjectName = std::string;
using ObjectHeaderCache = std::string;

class OsiDb {
    private:
        std::map<ObjectName, long> object_start;
        std::map<ObjectName, ObjectHeaderCache> cache;
        // fields["analog"] = {"", "recnum", "Indic", "Name", "Key", ...} // analog fields, in order
        FieldsGivenObject fields;
        char line[QLINEMAX];
        Qreader qr;
        std::string_view current_object{""};
        static constexpr std::array<char, 3> to_be_skipped{'\t', '*', '0'};
        Qstring qs;
        void mark_object_start() {
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
                if (line[0]) {
                    auto line_vector = qs(line).split();
                    ObjectName object_name{line_vector[1]};
                    qr.read(line);
                    cache[object_name] = line;
                    line_vector = qs(cache[object_name]).split();
                    auto n = line_vector.size();
                    fields[object_name] = {std::string_view("recnum")};
                    for(decltype(n) i=2; i<n; ++i) {
                        fields[object_name].push_back(line_vector[i]);
                    }
                    object_start[object_name] = qr.tell();
                }
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
