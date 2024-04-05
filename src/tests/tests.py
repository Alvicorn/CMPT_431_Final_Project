import subprocess
import os


class TestCase:
    def __init__(
        self, 
        test_case_name, # str
        executable, # str
        expected_output_matrix, # file path
        args=[], # list
        is_integration=False
    ):
        if os.name == "nt": # operating on a windows machine
            executable += ".exe"
        self.command = [f"./{executable}"] + args
        self.expected_output_matrix = expected_output_matrix
        self.test_case_name = test_case_name
        self.is_integration = is_integration
    
    def _stripe_string(self, string):
        return string.replace("\n", "").replace("\r", "").replace(" ", "").replace("\t", "")
    
    def execute_and_validate(self):
        process = subprocess.Popen(
            self.command, 
            stdout=subprocess.PIPE, 
            stderr=subprocess.PIPE
        )
        stdout, _ = process.communicate()

        print(f"\nTest Case: {self.test_case_name}")
        print(f"Command: {' '.join(self.command)}")
        print(stdout.decode())
        
        if self.is_integration:
            out = stdout.decode().split("\n")
            result = self._stripe_string("".join(out[:-2]))
            
            with open(self.expected_output_matrix, 'r') as file:
                file_contents = self._stripe_string(file.read())
                print("Test case " + "PASS" if file_contents == result else "FAIL")
        
        print("\n")

def main():
    
    test_cases = [
        TestCase(
            "serial_utils unit tests",
            "tests/test_serial_utils",
            ""
        ),
        TestCase(
            "all_pairs_serial empty graph",
            "all_pairs_serial",
            "",
            args=["--inputFile", "./tests/test_inputs/empty_graph.txt"]
        ),
        TestCase(
            "all_pairs_serial small graph",
            "all_pairs_serial",
            "./tests/test_outputs/small_graph.txt",
            args=["--inputFile", "./tests/test_inputs/small_graph.txt"],
            is_integration=True
        ),
        TestCase(
            "all_pairs_serial medium graph",
            "all_pairs_serial",
            "./tests/test_outputs/medium_graph.txt",
            args=["--inputFile", "./tests/test_inputs/medium_graph.txt"],
            is_integration=True
        )
    ]
    
    for test_case in test_cases:
        test_case.execute_and_validate()
    
if __name__ == "__main__":
    main()