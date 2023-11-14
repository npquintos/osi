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
#pragma ounce
#include <string>
#include <optional>
#include <string_view>
#include "include/qreader.hpp"
#include "include/split.hpp"

using ValueGivenField = std::unordered_map<std::string, std::string>
using FieldsGivenObject = std::unordered_map<std::string_view, std::vector<std::string_view>>
using ObjectName = std::string
using ObjectHeaderCache = std::string

class OsiDb {
    private:
        std::unordered_map<ObjectName, decltype(std::ftell())> object_start;
        std::unordered_map<ObjectName,ObjectHeaderCache> cache;
        // fields["analog"] = {"", "recnum", "Indic", "Name", "Key", ...} // analog fields, in order
        FieldsGivenObject fields;
        char line[QLINEMAX];
        Qreader qr;
        std::string_view current_object{""};
        constexpr std::span<char> to_be_skipped {"\t", "*", "0"};
        Qstring qs;
        void mark_object_start() {
            std::string line;
            std::getline(fin, line) // skip the DB header
            while(qr.read(line)) {
                if std::any_of(constexpr to_be_skipped.begin(), constexpr to_be_skipped.end(), [line](x) { return line[0] == x; }) {
                    continue;
                }
                if (line) {
                    auto line_vector = qs(line).split();
                    ObjectName object_name = line_vector[1];
                    std::getline(fin, line);
                    cache[object_name] = line;
                    line_vector = qs(cache[object_name]).split();
                    auto n = line_vector.size();
                    fields[object_name] = {std::string_view("recnum")};
                    for(decltype(n) i=2; i<n; ++i) {
                        fields[object_name].push_back(line_vector[i]);
                    }
                    object_start[object_name] = std::ftell();
                }
            }
        };

    public:
        std::optional<ValueGivenField> result;
        OsiDb(const char* dump_file_name) {
            OsidDb(std::string_view(dump_file_name));
        };

        OsiDb(const std::string_view dump_file_name) {
            qr = Qreader(dump_file_name);
            if (!fin) {
                std::cout << "Error reading dump file: " << dump_file_name << std::endl;
                exit(1);
            }
            mark_object_start();
        };

        bool seek(const char *object_name) {
            return seek(std::string_view{object_name});
        };

        bool seek(std::string_view object_name) {
            auto it = object_start.find(object_name);
            if (it != object_start.end()) {
                std::fseek(object_start[object_name]);
                current_object = object_name;
                return true;
            }
            return false;
        };

        std::optional<ValueGivenField> &next() {
            result.reset();
            while (getline(fin, line) && line[0] == "*");
            if (!line || line[0] == "0") {
                return std::nullopt
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
        }
}
