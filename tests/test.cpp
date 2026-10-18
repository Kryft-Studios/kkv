#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <kkv/kkv.hpp>

KKV::ParserAPI::parse_num_config defaultPNC{
    .returnIsString = false,
    .allowUncertainDecimal = false
};

KKV::ParserAPI::parse_num_config strPNC{
    .returnIsString = true,
    .allowUncertainDecimal = false
};

KKV::ParserAPI::parse_num_config uncertainPNC{
    .returnIsString = false,
    .allowUncertainDecimal = true
};

KKV::ParserAPI::parse_num_config uncertainStrPNC{
    .returnIsString = true,
    .allowUncertainDecimal = true
};

// Valid integers

TEST_CASE("tryReadAsNum: parsing 42 returns 42") {
    SUBCASE("number parser") {
        auto res = KKV::ParserAPI::tryReadAsNum("42", defaultPNC);
        CHECK(res.isANum);
        CHECK(res.num.value == 42);
    }

    SUBCASE("string parser") {
        auto res = KKV::ParserAPI::tryReadAsNum("42", strPNC);
        CHECK(res.isANum);
        CHECK_EQ(res.nstr.value, "42");
    }
}

TEST_CASE("tryReadAsNum: parsing -42 returns -42") {
    SUBCASE("number parser") {
        auto res = KKV::ParserAPI::tryReadAsNum("-42", defaultPNC);
        CHECK(res.isANum);
        CHECK(res.num.value == -42);
    }

    SUBCASE("string parser") {
        auto res = KKV::ParserAPI::tryReadAsNum("-42", strPNC);
        CHECK(res.isANum);
        CHECK_EQ(res.nstr.value, "-42");
    }
}

TEST_CASE("tryReadAsNum: parsing 0 returns 0") {
    auto res = KKV::ParserAPI::tryReadAsNum("0", defaultPNC);
    CHECK(res.isANum);
    CHECK(res.num.value == 0);
}

TEST_CASE("tryReadAsNum: parsing 00042 returns 42") {
    auto res = KKV::ParserAPI::tryReadAsNum("00042", defaultPNC);
    CHECK(res.isANum);
    CHECK(res.num.value == 42);
}

// Valid decimals

TEST_CASE("tryReadAsNum: parsing 42.2 returns 42.2") {
    SUBCASE("number parser") {
        auto res = KKV::ParserAPI::tryReadAsNum("42.2", defaultPNC);
        REQUIRE(res.isANum);
        CHECK(doctest::Approx(res.num.value) == 42.2);
    }

    SUBCASE("string parser") {
        auto res = KKV::ParserAPI::tryReadAsNum("42.2", strPNC);
        CHECK(res.isANum);
        CHECK_EQ(res.nstr.value, "42.2");
    }
}

TEST_CASE("tryReadAsNum: parsing -67.67 returns -67.67") {
    auto res = KKV::ParserAPI::tryReadAsNum("-67.67", defaultPNC);
    REQUIRE(res.isANum);
    CHECK(doctest::Approx(res.num.value) == -67.67);
}

TEST_CASE("tryReadAsNum: parsing 0.5 returns 0.5") {
    auto res = KKV::ParserAPI::tryReadAsNum("0.5", defaultPNC);
    REQUIRE(res.isANum);
    CHECK(doctest::Approx(res.num.value) == 0.5);
}

TEST_CASE("tryReadAsNum: parsing 10.01 returns 10.01") {
    auto res = KKV::ParserAPI::tryReadAsNum("10.01", defaultPNC);
    REQUIRE(res.isANum);
    CHECK(doctest::Approx(res.num.value) == 10.01);
}

TEST_CASE("tryReadAsNum: parsing 99.999 returns 99.999") {
    auto res = KKV::ParserAPI::tryReadAsNum("99.999", defaultPNC);
    REQUIRE(res.isANum);
    CHECK(doctest::Approx(res.num.value) == 99.999);
}

// Uncertain decimals

TEST_CASE("tryReadAsNum: decimals without integer parts") {
    SUBCASE(".7 is rejected by default") {
        CHECK_FALSE(
            KKV::ParserAPI::tryReadAsNum(".7", defaultPNC).isANum
        );
    }

    SUBCASE("-.7 is rejected by default") {
        CHECK_FALSE(
            KKV::ParserAPI::tryReadAsNum("-.7", defaultPNC).isANum
        );
    }

    SUBCASE(".7 is accepted when allowed") {
        auto res = KKV::ParserAPI::tryReadAsNum(".7", uncertainPNC);
        REQUIRE(res.isANum);
        CHECK(doctest::Approx(res.num.value) == 0.7);
    }

    SUBCASE("-.7 is accepted when allowed") {
        auto res = KKV::ParserAPI::tryReadAsNum("-.7", uncertainPNC);
        REQUIRE(res.isANum);
        CHECK(doctest::Approx(res.num.value) == -0.7);
    }

    SUBCASE("string parser preserves -.7") {
        auto res = KKV::ParserAPI::tryReadAsNum("-.7", uncertainStrPNC);
        CHECK(res.isANum);
        CHECK_EQ(res.nstr.value, "-.7");
    }
}

// Invalid input

TEST_CASE("tryReadAsNum: invalid literals are rejected") {
    SUBCASE("empty string") {
        CHECK_FALSE(
            KKV::ParserAPI::tryReadAsNum("", defaultPNC).isANum
        );
    }

    SUBCASE("negative sign only") {
        CHECK_FALSE(
            KKV::ParserAPI::tryReadAsNum("-", defaultPNC).isANum
        );
    }

    SUBCASE("decimal point only") {
        CHECK_FALSE(
            KKV::ParserAPI::tryReadAsNum(".", defaultPNC).isANum
        );
    }

    SUBCASE("negative sign and decimal point only") {
        CHECK_FALSE(
            KKV::ParserAPI::tryReadAsNum("-.", defaultPNC).isANum
        );
    }

    SUBCASE("multiple negative signs") {
        CHECK_FALSE(
            KKV::ParserAPI::tryReadAsNum("--42", defaultPNC).isANum
        );
    }

    SUBCASE("positive sign") {
        CHECK_FALSE(
            KKV::ParserAPI::tryReadAsNum("+42", defaultPNC).isANum
        );
    }

    SUBCASE("multiple decimal points") {
        CHECK_FALSE(
            KKV::ParserAPI::tryReadAsNum("1.2.3", defaultPNC).isANum
        );
    }

    SUBCASE("decimal point at the end") {
        CHECK_FALSE(
            KKV::ParserAPI::tryReadAsNum("10.", defaultPNC).isANum
        );
    }

    SUBCASE("letters") {
        CHECK_FALSE(
            KKV::ParserAPI::tryReadAsNum("abc", defaultPNC).isANum
        );
    }

    SUBCASE("digits followed by letters") {
        CHECK_FALSE(
            KKV::ParserAPI::tryReadAsNum("42abc", defaultPNC).isANum
        );
    }

    SUBCASE("whitespace") {
        CHECK_FALSE(
            KKV::ParserAPI::tryReadAsNum("42 ", defaultPNC).isANum
        );
    }

    SUBCASE("embedded whitespace") {
        CHECK_FALSE(
            KKV::ParserAPI::tryReadAsNum("4 2", defaultPNC).isANum
        );
    }
}
