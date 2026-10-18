import test from "node:test";
import assert from "node:assert/strict";
import createKKV from "../em/kkv.js";

const kkv = await createKKV();

const defaultPNC = {
    returnIsString: false,
    allowUncertainDecimal: false
};

const strPNC = {
    returnIsString: true,
    allowUncertainDecimal: false
};

const uncertainPNC = {
    returnIsString: false,
    allowUncertainDecimal: true
};

const uncertainStrPNC = {
    returnIsString: true,
    allowUncertainDecimal: true
};

function parseNum(input, config = defaultPNC) {
    return kkv.tryReadAsNum(input, config);
}

function expectNumber(input, expected, config = defaultPNC) {
    const result = parseNum(input, config);

    assert.equal(result.isANum, true, `${JSON.stringify(input)} should parse as a number`);

    if (config.returnIsString) {
        assert.equal(result.nstr.value, expected);
    } else {
        assert.ok(
            Math.abs(result.num.value - expected) < 1e-10,
            `Expected ${result.num.value} to equal ${expected}`
        );
    }
}

function expectInvalid(input, config = defaultPNC) {
    assert.equal(
        parseNum(input, config).isANum,
        false,
        `${JSON.stringify(input)} should not parse as a number`
    );
}

// Valid integers

test("tryReadAsNum: parsing 42 returns 42", () => {
    expectNumber("42", 42);
    expectNumber("42", "42", strPNC);
});

test("tryReadAsNum: parsing -42 returns -42", () => {
    expectNumber("-42", -42);
    expectNumber("-42", "-42", strPNC);
});

test("tryReadAsNum: parsing 0 returns 0", () => {
    expectNumber("0", 0);
});

test("tryReadAsNum: parsing 00042 returns 42", () => {
    expectNumber("00042", 42);
});

// Valid decimals

test("tryReadAsNum: parsing 42.2 returns 42.2", () => {
    expectNumber("42.2", 42.2);
    expectNumber("42.2", "42.2", strPNC);
});

test("tryReadAsNum: parsing -67.67 returns -67.67", () => {
    expectNumber("-67.67", -67.67);
});

test("tryReadAsNum: parsing 0.5 returns 0.5", () => {
    expectNumber("0.5", 0.5);
});

test("tryReadAsNum: parsing 10.01 returns 10.01", () => {
    expectNumber("10.01", 10.01);
});

test("tryReadAsNum: parsing 99.999 returns 99.999", () => {
    expectNumber("99.999", 99.999);
});

// Uncertain decimals

test("tryReadAsNum: decimals without integer parts", async (t) => {
    await t.test(".7 is rejected by default", () => {
        expectInvalid(".7");
    });

    await t.test("-.7 is rejected by default", () => {
        expectInvalid("-.7");
    });

    await t.test(".7 is accepted when allowed", () => {
        expectNumber(".7", 0.7, uncertainPNC);
    });

    await t.test("-.7 is accepted when allowed", () => {
        expectNumber("-.7", -0.7, uncertainPNC);
    });

    await t.test("string parser preserves -.7", () => {
        expectNumber("-.7", "-.7", uncertainStrPNC);
    });
});

// Invalid input

test("tryReadAsNum: invalid literals are rejected", async (t) => {
    const invalidLiterals = [
        ["empty string", ""],
        ["negative sign only", "-"],
        ["decimal point only", "."],
        ["negative sign and decimal point only", "-."],
        ["multiple negative signs", "--42"],
        ["positive sign", "+42"],
        ["multiple decimal points", "1.2.3"],
        ["decimal point at the end", "10."],
        ["letters", "abc"],
        ["digits followed by letters", "42abc"],
        ["whitespace", "42 "],
        ["embedded whitespace", "4 2"]
    ];

    for (const [description, input] of invalidLiterals) {
        await t.test(description, () => {
            expectInvalid(input);
        });
    }
});
