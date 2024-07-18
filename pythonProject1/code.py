import subprocess


def run_interpreter(input_data):
    process = subprocess.Popen(
        [r'C:\Users\bubli\OneDrive\Desktop\Assignment5\ConsoleApplication1\x64\Debug\ConsoleApplication1.exe'],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )
    stdout, stderr = process.communicate(input_data)
    return stdout.strip(), stderr.strip()


def test_interpreter():
    tests = [
        {
            "input": "2+3\n",
            "expected_output": "Result: 5"
        },
        {
            "input": "10-4\n",
            "expected_output": "Result: 6"
        },
        {
            "input": "3*4\n",
            "expected_output": "Result: 12"
        },
        {
            "input": "8/2\n",
            "expected_output": "Result: 4"
        },
        {
            "input": "pow(2,3)\n",
            "expected_output": "Result: 8"
        },
        {
            "input": "abs(-5)\n",
            "expected_output": "Result: 5"
        },
        {
            "input": "max(3,7)\n",
            "expected_output": "Result: 7"
        },
        {
            "input": "min(3,7)\n",
            "expected_output": "Result: 3"
        },
        {
            "input": "var a=5\n",
            "expected_output": "Result: 5"
        },
        {
            "input": "a+2\n",
            "expected_output": "Result: 7",
            "pre_input": "var a=5\n"
        },
        {
            "input": "Add(1,2)\n",
            "expected_output": "Result: 3"
        },
        {
            "input": "Subtract(5,2)\n",
            "expected_output": "Result: 3"
        },
        {
            "input": "Multiply(2,3)\n",
            "expected_output": "Result: 6"
        },
        {
            "input": "Divide(8,4)\n",
            "expected_output": "Result: 2"
        }
    ]

    for test in tests:
        input_data = test["input"]
        expected_output = test["expected_output"]
        pre_input = test.get("pre_input", "")
        combined_input = pre_input + input_data

        actual_output, error = run_interpreter(combined_input)
        actual_output = actual_output.replace("Enter expression:", "").strip()

        output_lines = actual_output.splitlines()
        final_output = output_lines[-1].strip() if output_lines else ""

        assert final_output == expected_output, f"Test failed: {input_data.strip()} | Expected: {expected_output}, Got: {final_output}"


if __name__ == "__main__":
    test_interpreter()
    print("All tests passed!")
