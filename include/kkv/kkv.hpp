
#pragma once

#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace KKV {

using str = std::string;
/**
 * @name ast_type types
 * {@
 */
 enum class Type {
    NUMBER,
    STRING,
    /** Only available if parse_num_options.returnIsString */
    NUMSTR,
    OBJECT,
    VECTOR


 };
template <typename T, Type Differentiator>
struct ast_typed {
    T value;
    static constexpr Type type =  Differentiator;
};
using ast_string = ast_typed<str, Type::STRING>;
using ast_number = ast_typed<double, Type::NUMBER>;
using ast_numstr = ast_typed<str, Type::NUMSTR>;
struct ast_object;
struct ast_vector;

using ast_object_ptr = std::shared_ptr<ast_object>;
using ast_vector_ptr = std::shared_ptr<ast_vector>;

using ast_type = std::variant<ast_numstr, ast_string, ast_number, ast_object_ptr, ast_vector_ptr>;

struct value_ast {
    str name;
    ast_type value;
};

struct ast_object {
    std::vector<value_ast> value;
    static constexpr Type type = Type::OBJECT;
};

struct ast_vector {
    std::vector<ast_type> value;
    static constexpr Type type = Type::VECTOR;
};
/** @} */
class Parser {
    std::vector<ast_type> ast;
    str& fileContents;
    Parser(str& fileContents);
};

namespace ParserAPI {
    struct completion_detectable {
        bool isCompletionConfirmed;
    };
    struct valued {
        str value;
    };
    struct brief_value_ast: public completion_detectable, public valued {
        str name;
    };
    struct brief_empty_ast: public completion_detectable, public valued {};
    using brief_ast = std::variant<brief_value_ast, brief_empty_ast>;
    struct continutation {
        brief_ast of;
    };
    using maybe_continued = std::optional<continutation>;
    std::vector<brief_ast> tryRead(str line);
    ast_type translateStr(str string);
    struct parse_num_config {
        /** Whether to return a number as a string */
        bool returnIsString;
        bool allowUncertainDecimal;
    };
    struct read_as_num_result {
        bool isANum;
        ast_number num;
        ast_numstr nstr;
    };
    read_as_num_result tryReadAsNum(str val, parse_num_config pnc);
    struct failure {
        str reason;
        bool failed;
    };
    struct hunt_sentence_result {
        str name;
        str value;
    };
    struct hunt_sentences_result {
        std::vector<hunt_sentence_result> sentences;
        failure fail;
    };
    hunt_sentences_result huntSentences(str file);
}
} // namespace KKV
