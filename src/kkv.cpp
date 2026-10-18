#include <cstddef>
#include <cstdint>
#include <format>
#include <functional>
#include <iostream>
#include <kkv/kkv.hpp>
#include <kkv/priv.hpp>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include <rang.hpp>

namespace KKV {
std::unordered_map<char, uint8_t> numLkp{
    {'0', 0}, {'1', 1}, {'2', 2}, {'3', 3}, {'4', 4},
    {'5', 5}, {'6', 6}, {'7', 7}, {'8', 8}, {'9', 9},
};
bool isAValidNumChar(char chr) { return numLkp.contains(chr); };
double addDigitRight(double it, char digit) {
    return (double)(it * 10.0) + (double)numLkp.at(digit);
}
double addDigitRight_type_deci(double it, char digit, double decimul) {
    return it + (decimul * (double)numLkp.at(digit));
};
ParserAPI::read_as_num_result failureResult{.isANum = false, .num = {0}, .nstr = {""}};
ParserAPI::read_as_num_result ParserAPI::tryReadAsNum(str val, ParserAPI::parse_num_config pnc) {
    if (val.empty())
        return failureResult;
    bool negetate = val.at(0) == '-';
    double currentInt = 0;
    std::string currstr{(negetate ? "-" : "")};
    bool hasStartedDecimal = false;
    double decimalFPMul = 0.1;
    bool hasHadAIntBforeDeci = false;
    int len = (int)val.length();
    for (int i = static_cast<int>(negetate); i < len; i++) {
        const char st = val.at((size_t)i);
        if (!isAValidNumChar(st)) {
            if (st == '.' && !hasStartedDecimal) {
                bool isLastChar = len - 1 == i;
                if (isLastChar)
                    return failureResult;

                if (!hasHadAIntBforeDeci && !pnc.allowUncertainDecimal)
                    return failureResult;
                hasStartedDecimal = true;
                if (pnc.returnIsString)
                    currstr.append(std::string{st});
                continue;
            };
            return failureResult;
        };
        hasHadAIntBforeDeci = true;
        if (pnc.returnIsString) {
            currstr.append(std::string{st});
        } else {
            if (hasStartedDecimal) {
                currentInt = addDigitRight_type_deci(currentInt, st, decimalFPMul);
                decimalFPMul *= 0.1;

            } else {
                currentInt = addDigitRight(currentInt, st);
            }
        }
    };
    if (!hasHadAIntBforeDeci)
        return failureResult;
    return {.isANum = true, .num = {negetate ? -1 * currentInt : currentInt}, .nstr = {currstr}};
};
ParserAPI::hunt_sentences_result createHSFailure(str reason) {
    return {.sentences = {}, .fail = {.reason = reason, .failed = true}};
};
struct lecms {
    bool escapeCodeIsSpecial;
    bool escapeCodeIsFunctional;
    std::function<char(const char *)> ifFunctionalFn;
    char ifSpecialChar;
    size_t ifFunctionalArgLength;
    explicit lecms(std::function<char(const char *)> fn, size_t argLength)
        : escapeCodeIsSpecial(false), escapeCodeIsFunctional(true), ifFunctionalFn(fn),
          ifSpecialChar('_'), ifFunctionalArgLength(argLength) {}

    explicit lecms(char specialChar)
        : escapeCodeIsSpecial(true), escapeCodeIsFunctional(false),
          ifFunctionalFn([](const char *a) { return a[0]; }), ifSpecialChar(specialChar),
          ifFunctionalArgLength(0) {}
};
std::unordered_map<char, lecms> lkpEscapeCode{
    {'n', lecms{'\n'}},
    {'t', lecms{'\t'}},
    {'b', lecms{'\b'}},
    {'r', lecms{'\r'}},
    {'0', lecms{'\0'}},
    {'\\', lecms{'\\'}},
    {'"', lecms{'\"'}},
    {'\'', lecms{'\''}},
    {'f', lecms{'\f'}},
    {'v', lecms{'\v'}},
    {'a', lecms{'\a'}},
    {'x', lecms{[](const char *arg) { return arg[0]; }, 1}},
    {'u', lecms{[](const char *arg) -> char {
                    return static_cast<char>(std::strtoul(arg, nullptr, 16));
                },
                4}},
    {'U', lecms{[](const char *arg) -> char {
                    return static_cast<char>(std::strtoul(arg, nullptr, 16));
                },
                8}},
    {'o',
     lecms{[](const char *arg) -> char { return static_cast<char>(std::strtoul(arg, nullptr, 8)); },
           3}}};
struct ParseError : public std::runtime_error {
    explicit ParseError(std::string message, std::optional<const char *> stackInfo)
        : std::runtime_error(std::string{"ParseError: "} + message + 
                             (stackInfo.has_value() ? std::string{"\n"}+stackInfo.value() : std::string{""})) {};
    explicit ParseError(const char *message, std::optional<const char *> stackInfo)
        : std::runtime_error(std::string{"ParseError: "} + message +
                              (stackInfo.has_value() ?std::string{"\n"}+ (stackInfo.value()) : std::string{""})) {};
};
void ParseWarn(std::string &message, std::optional<std::string> stackInfo) {
    std::cerr<<rang::fg::yellow<<"ParseWarn"<<rang::style::reset<<std::format(": {}{}", message, stackInfo.has_value() ?'\n'+stackInfo.value(): "");
};
struct try_ec_result {
    size_t ptrAdd=1;
    char result;
};
try_ec_result tryEscapeCode(str &file, size_t ptr) {
    auto p = ptr + 1;
    if(p>=file.length())throw ParseError("Uncompleted Escape Code at EOF", "KKV::tryEscapeCode");
    char escapeCode = file.at(p);
    if (!lkpEscapeCode.contains(escapeCode))
        throw ParseError(std::string{"Unknown Escape Code"}+(escapeCode), "KKV::tryEscapeCode");
    auto lecms = lkpEscapeCode.at(escapeCode);
    if(lecms.escapeCodeIsSpecial)return {.result=lecms.ifSpecialChar};
    if(lecms.escapeCodeIsFunctional){
        auto expectedFileTill = ptr + lecms.ifFunctionalArgLength;
        if(expectedFileTill>=file.length())throw ParseError("Uncompleted Escape Code at EOF", "KKV::tryEscapeCode");
        return {.result=lecms.ifFunctionalFn(file.substr(p, expectedFileTill).c_str())};
    };
    
    throw ParseError("Unexpected Failure", "KKV::tryEscapeCode");
};
/*ParserAPI::hunt_sentences_result ParserAPI::huntSentences(str file) {
};*/
} // namespace KKV
