#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "include/osiDb.hpp"
#include "include/doctest.h"
#include "string.h"

using OsiObject = std::string;
using OsiField = std::string;
using OsiValue = std::string;
using VectorOfTupleOfObjectFieldValue = std::vector<std::tuple<OsiObject, OsiField, OsiValue>>;

TEST_CASE("Access any field value in Osi database") {
    char line[QLINEMAX];
    SUBCASE("Beginning of Object data properly determined") {
        VectorOfTupleOfObjectFieldValue object_field_correctValue = {
            {"NA_CONST1", "case_title", "External17May2023"},
            {"LOAD", "name", "STN SERV"},
            {"TAP", "name", "1_7_1.05_0.9_0_0"},
            {"DBUS", "kv_mag", "0.946760237"},
            {"OPF_CONTINGENCY", "sol_status", ""} // this last one, no record
        }
        auto db = OsiDb("../include/opennet.dat");
        for (std::tuple [xobject, xfield, correct_value] : object_field_correctValue) {
            if (db.seek(xobject)) {
                if (const ValueGivenField &row = db.next()) {
                    CHECK(correct_value == row[xfield]);
                }
            }
        }
    }

}
