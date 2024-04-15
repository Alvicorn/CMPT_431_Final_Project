import subprocess
import os


#terminal text colours
GREEN = "\033[92m"
RED = "\033[91m"
RESET = "\033[0m"


class TestCase:
    def __init__(
        self, 
        test_case_name, # str
        executable, # str
        expected_output_matrix, # file path
        excess_output_lines, # extra lines from output that appear after the matrix
        args=[], # list
        is_integration=False,
        show_output=True
    ):
        if os.name == "nt": # operating on a windows machine
            executable += ".exe"
        self.command = [f"./{executable}"] + args
        self.expected_output_matrix = expected_output_matrix
        self.eof = excess_output_lines
        self.test_case_name = test_case_name
        self.is_integration = is_integration
        self.show_output = show_output
    
    def _strip_string(self, string):
        return string.replace("\n", "").replace("\r", "").replace(" ", "").replace("\t", "")
    
    def _print_result(self, result):
        print(f"{GREEN}PASS" if result else f"{RED}FAIL", end="")
        print(RESET)


    def execute_and_validate(self):
        process = subprocess.Popen(
            self.command, 
            stdout=subprocess.PIPE, 
            stderr=subprocess.PIPE
        )
        stdout, _ = process.communicate()

        print(f"\n{self.test_case_name}")
        print(f"Command: {' '.join(self.command)}")
        if self.show_output:
            print(stdout.decode())
        else: # still show the timing
            print("\n".join(stdout.decode().split("\n")[-self.eof:]))
        
        if self.is_integration:
            out = stdout.decode().split("\n")
            result = self._strip_string("".join(out[:-self.eof]))
            # print(out[:-self.eof])

            with open(self.expected_output_matrix, 'r') as file:
                file_contents = self._strip_string(file.read())
                self._print_result(file_contents == result)
                return file_contents == result    
        return True


def tester(test_cases):
    failed_tests = []
    pass_count = 0
    for index, test in enumerate(test_cases):
        is_valid = test.execute_and_validate()
        if is_valid:
            pass_count += 1
        else:
            failed_tests.append(index)
    
    return (len(test_cases), pass_count, failed_tests)


def serial_tests():
    tests = [
        TestCase(
            "Serial Test 0: serial_utils unit tests",
            "tests/test_serial_utils",
            "",
            0
        ),
        TestCase(
            "Serial Test 1: all_pairs_serial empty graph",
            "all_pairs_serial",
            "",
            0,
            args=["--inputFile", "./tests/test_inputs/empty_graph.txt"]
        ),
        TestCase(
            "Serial Test 2: all_pairs_serial small graph",
            "all_pairs_serial",
            "./tests/test_outputs/small_graph.txt",
            2,
            args=["--inputFile", "./tests/test_inputs/small_graph.txt"],
            is_integration=True
        ),
        TestCase(
            "Serial Test 3: all_pairs_serial medium graph",
            "all_pairs_serial",
            "./tests/test_outputs/medium_graph.txt",
            2,
            args=["--inputFile", "./tests/test_inputs/medium_graph.txt"],
            is_integration=True
        ),
        TestCase(
            "Serial Test 4: all_pairs_serial 100 graph",
            "all_pairs_serial",
            "./tests/test_outputs/100_graph.txt",
            2,
            args=["--inputFile", "./tests/test_inputs/100_graph.txt"],
            is_integration=True,
            show_output=False
        ),
        TestCase(
            "Serial Test 5: all_pairs_serial 1TH vertices, 50 edges graph",
            "all_pairs_serial",
            "./tests/test_outputs/1TH_vertices_50_edges_graph.txt",
            2,
            args=["--inputFile", "./tests/test_inputs/1TH_vertices_50_edges_graph.txt"],
            is_integration=True,
            show_output=False
        )
    ]

    return tester(tests)
    

def parallel_tests():
    tests = [
        TestCase(
            "Parallel Test 0: all_pairs_parallel small graph with one thread",
            "all_pairs_parallel",
            "./tests/test_outputs/small_graph.txt",
            4,
            args=[
                "--inputFile", 
                "./tests/test_inputs/small_graph.txt",
                "--nThreads",
                "1"
            ],
            is_integration=True
        ),
        TestCase(
            "Parallel Test 1: all_pairs_parallel small graph with two threads",
            "all_pairs_parallel",
            "./tests/test_outputs/small_graph.txt",
            5,
            args=[
                "--inputFile", 
                "./tests/test_inputs/small_graph.txt",
                "--nThreads",
                "2"
            ],
            is_integration=True
        ),
        TestCase(
            "Parallel Test 2: all_pairs_parallel small graph with four threads",
            "all_pairs_parallel",
            "./tests/test_outputs/small_graph.txt",
            7,
            args=[
                "--inputFile", 
                "./tests/test_inputs/small_graph.txt",
                "--nThreads",
                "4"
            ],
            is_integration=True
        ),
        TestCase(
            "Parallel Test 3: all_pairs_parallel small graph with five threads",
            "all_pairs_parallel",
            "./tests/test_outputs/small_graph.txt",
            8,
            args=[
                "--inputFile", 
                "./tests/test_inputs/small_graph.txt",
                "--nThreads",
                "5"
            ],
            is_integration=True
        ),
        TestCase(
            "Parallel Test 4: all_pairs_parallel medium graph with one thread",
            "all_pairs_parallel",
            "./tests/test_outputs/medium_graph.txt",
            4,
            args=[
                "--inputFile", 
                "./tests/test_inputs/medium_graph.txt",
                "--nThreads",
                "1"
            ],
            is_integration=True
        ),
        TestCase(
            "Parallel Test 5: all_pairs_parallel medium graph with two threads",
            "all_pairs_parallel",
            "./tests/test_outputs/medium_graph.txt",
            5,
            args=[
                "--inputFile", 
                "./tests/test_inputs/medium_graph.txt",
                "--nThreads",
                "2"
            ],
            is_integration=True
        ),
        TestCase(
            "Parallel Test 6: all_pairs_parallel medium graph with 9 thread",
            "all_pairs_parallel",
            "./tests/test_outputs/medium_graph.txt",
            12,
            args=[
                "--inputFile", 
                "./tests/test_inputs/medium_graph.txt",
                "--nThreads",
                "9"
            ],
            is_integration=True
        ),
        TestCase(
            "Parallel Test 7: all_pairs_parallel 100 graph with one thread",
            "all_pairs_parallel",
            "./tests/test_outputs/100_graph.txt",
            4,
            args=[
                "--inputFile", 
                "./tests/test_inputs/100_graph.txt",
                "--nThreads",
                "1"
            ],
            is_integration=True,
            show_output=False
        ),
        TestCase(
            "Parallel Test 8: all_pairs_parallel 100 graph with two thread",
            "all_pairs_parallel",
            "./tests/test_outputs/100_graph.txt",
            5,
            args=[
                "--inputFile", 
                "./tests/test_inputs/100_graph.txt",
                "--nThreads",
                "2"
            ],
            is_integration=True,
            show_output=False
        ),
        TestCase(
            "Parallel Test 9: all_pairs_parallel 100 graph with five thread",
            "all_pairs_parallel",
            "./tests/test_outputs/100_graph.txt",
            8,
            args=[
                "--inputFile", 
                "./tests/test_inputs/100_graph.txt",
                "--nThreads",
                "5"
            ],
            is_integration=True,
            show_output=False
        ),
        TestCase(
            "Parallel Test 10: all_pairs_parallel 1TH vertices, 50 edges graph with one thread",
            "all_pairs_parallel",
            "./tests/test_outputs/1TH_vertices_50_edges_graph.txt",
            4,
            args=[
                "--inputFile", 
                "./tests/test_inputs/1TH_vertices_50_edges_graph.txt",
                "--nThreads",
                "1"
            ],
            is_integration=True,
            show_output=False
        ),
        TestCase(
            "Parallel Test 11: all_pairs_parallel 1TH vertices, 50 edges graph with two threads",
            "all_pairs_parallel",
            "./tests/test_outputs/1TH_vertices_50_edges_graph.txt",
            5,
            args=[
                "--inputFile", 
                "./tests/test_inputs/1TH_vertices_50_edges_graph.txt",
                "--nThreads",
                "2"
            ],
            is_integration=True,
            show_output=False
        ),
        TestCase(
            "Parallel Test 12: all_pairs_parallel 1TH vertices, 50 edges graph with 4 threads",
            "all_pairs_parallel",
            "./tests/test_outputs/1TH_vertices_50_edges_graph.txt",
            7,
            args=[
                "--inputFile", 
                "./tests/test_inputs/1TH_vertices_50_edges_graph.txt",
                "--nThreads",
                "4"
            ],
            is_integration=True,
            show_output=False
        )
    ]
    
    return tester(tests)
    


def main():
    serial_len, serial_pass, serial_failed = serial_tests()
    parallel_len, parallel_pass, parallel_failed = parallel_tests()

    print(f"Serial results: {serial_pass} / {serial_len}")
    print(f"Parallel results: {parallel_pass} / {parallel_len}")
    for fail in serial_failed:
        print(f"Serial {fail}: FAIL")
    for fail in parallel_failed:
        print(f"Parallel {fail}: FAIL")

if __name__ == "__main__":
    main()