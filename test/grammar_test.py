import subprocess
from os import walk

TEST_PROGRAMS_PATH = "programs/test_programs"
ANTLR_SUGAR_GRAMMAR_PATH = "antlr/Brainfuck2D.g4"


def test_antlr_interpretation_of_grammar():
    files = next(walk(TEST_PROGRAMS_PATH), (None, None, []))[2]
    error_collection = ""
    for file in files:
        cmd = f"antlr4-parse {ANTLR_SUGAR_GRAMMAR_PATH} program {TEST_PROGRAMS_PATH}/{file}"
        output = subprocess.run(
            cmd.split(' '),
            capture_output=True,
            text=True,
            check=True
        ).stderr
        if output != "":
            error_collection += f"\n{file}:\n{output}"
    assert error_collection == "", error_collection


if __name__ == '__main__':
    test_antlr_interpretation_of_grammar()
