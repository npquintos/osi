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
#include "include/statemachine_enum.hpp"

using FtellLocation = long;
using SplitRecordFields = std::span<std::string_view>;
using Cargo = class {
                    char line[QLINEMAX];
                    Qreader qr;
              };

namespace Prelim {
    enum state {seek_object_start, process_object_header, seek_object_end, end};
}

namespace RunTime {
    enum state {seek_object_start, process_object_records, end};
}


// sz should be something like Emap::total_objects
// or in general <db name>::total_objects
// where 'db name' is Capitalized
// e.g. db = OsiDb<Emap::total_objcts>("emap.dat");
template <size sz>
class OsiDb {
    private:
        // index would be something like Opennet_breaker::no
        // or in general, <DB name>_<object name>::no
        // DB name is capitalized, all the ressstsstat lower case
        // so, if this is emap, object start for emap_switch
        // would be object_start[Emap_emap_switch::no] and
        // you should do std::fseek(object_start[Emap_emap_switch::no])
        // prior to reading.
        FtellLocation object_start[sz+1];
        Qstring qs;
        int current_object_no;
        class Cargo {
            char line[QLINEMAX];
            Qreader qr;
            int obj_no;
        } cargo;

        // This procedure assumes that pointer is AFTER the first
        // line which is pointing to the database header. Hence,
        // you need to read a line first after opening the dump file
        // before passing the file pointer to this procedure.
        StateAndData<Cargo> seek_object_start(Cargo cargo) {
            while(cargo.qr.read(line)) {
                if (line[0] == '*') continue;
                if (line[0] == '0') break;
                return std::make_tuple(Prelim::process_object_header, cargo);
            }
            return std::make_tuple(Prelim::end, cargo);
        };

        StateAndData<Cargo> process_object_header(Cargo cargo) {
            // cargo.line contains object header with number
            ++cargo.obj_no;
            cargo.qr.read(line); // skip 2 lines so that tell would
            cargo.qr.read(line); // point to the first record
            object_start[cargo.obj_no] = qr.tell();
            return std::make_tuple(Prelim::seek_object_end, cargo)

        };

        StateAndData<Cargo> seek_object_end(Cargo cargo) {
            while(cargo.qr.read(line)) {
                if (line[0] == '0') break;
            }
            return std::make_tuple(Prelim::seek_object_start, cargo);
        };
        
        void mark_object_start() {
            StateMachine<Cargo, 3> sm = StateMachine<Cargo, 3>();
            sm.add(Prelim::seek_object_start, seek_object_start);
            sm.add(Prelim::process_object_header, process_object_header);
            sm.add(Prelim::seek_object_end, seek_object_end);
            sm.add(Prelim::end, seek_object_end, true);
            cargo.qr.read(line); // skip DB header
            sm.run(Prelim::seek_object_start, cargo);
        };
xxxxxxxxxxxxxxxxx stopped here
    public:
        std::optional<std::span<std::string_view>> result;
        OsiDb(const char* dump_file_name)
        {
            cargo.qr = Qreader(dump_file_name);
            mark_object_start();
        };

        OsiDb(const std::string dump_file_name) :
        {
            cargo.qr = Qreader(dump_file_name.c_str());
            mark_object_start();
        };

        bool seek(int object_no) {
            if (object_start.size() < object_no+1) {
                qr.seek(object_start[object_name]);
                current_object_no = object_no;
                return true;
            }
            return false;
        };

        std::optional<std::span<std::string_view>> next() {
            while (qr.read(line) && *line == '*');
            if (qr.eol || *line == '0') {
                return {};
            }
            result = qs(line).split("\t");
            return result;
        };

};
