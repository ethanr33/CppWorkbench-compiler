
import pytest
import json
from pathlib import Path
import subprocess
import os

ROOT_DIR = Path(__file__).resolve().parents[2]
CURRENT_DIR = Path(__file__).resolve().parent

RUN_SCRIPT = ROOT_DIR / "run.sh"
BASIC_TESTS_ROOT_DIR = CURRENT_DIR / "basic_tests"
BASIC_TESTS_ANSWER_SOURCE = BASIC_TESTS_ROOT_DIR / "expected.json"

def check_for_error(stdout):
    return "error" in stdout.decode()

def test_basic_tests():
    with open(BASIC_TESTS_ANSWER_SOURCE) as f:
        content = f.read()
        answer_key = json.loads(content)

        for file_name in answer_key:
            file_path = CURRENT_DIR.relative_to(ROOT_DIR) / BASIC_TESTS_ROOT_DIR / file_name
            print(answer_key[file_name])
            if not answer_key[file_name]["error"]:
                proc = subprocess.run([str(RUN_SCRIPT), file_path], capture_output=True)
                assert proc.returncode == answer_key[file_name]["returncode"]
                assert not check_for_error(proc.stdout)
            else:
                proc = subprocess.run([str(RUN_SCRIPT), file_path], capture_output=True)
                assert check_for_error(proc.stdout)
