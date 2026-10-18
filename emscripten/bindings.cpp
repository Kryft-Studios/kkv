#include <emscripten/bind.h>
#include "kkv/kkv.hpp"

using namespace emscripten;






EMSCRIPTEN_BINDINGS(kkv_module) {
    // AST primitive types
    value_object<KKV::ast_number>("ast_number")
        .field("value", &KKV::ast_number::value);

    value_object<KKV::ast_numstr>("ast_numstr")
        .field("value", &KKV::ast_numstr::value);

    value_object<KKV::ParserAPI::parse_num_config>("parse_num_config")
        .field("returnIsString", &KKV::ParserAPI::parse_num_config::returnIsString)
        .field("allowUncertainDecimal", &KKV::ParserAPI::parse_num_config::allowUncertainDecimal);

    value_object<KKV::ParserAPI::read_as_num_result>("read_as_num_result")
        .field("isANum", &KKV::ParserAPI::read_as_num_result::isANum)
        .field("num", &KKV::ParserAPI::read_as_num_result::num)
        .field("nstr", &KKV::ParserAPI::read_as_num_result::nstr);

    // Parser API
    function("tryReadAsNum", &KKV::ParserAPI::tryReadAsNum);
}