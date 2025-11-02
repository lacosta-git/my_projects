"""
Module used to generate predefined test report
"""
test_report = ""


def init_report(initial_info):
    global test_report
    test_report += f"TEST REPORT\n    {initial_info}\n"


def step(step_number):
    global test_report
    test_report += f"Step: {step_number}\n"


def description(desc):
    global test_report
    test_report += f"Description: {desc}\n"


def action(act):
    global test_report
    test_report += f"Action: {act}\n"


def expected(exp):
    global test_report
    test_report += f"Expected: {exp}\n"


def status(t_result):
    global test_report
    test_report += f"Status: {t_result}\n"


def add_to_report(info):
    global test_report
    test_report += f"Additional Info: {info}\n"


def result(res):
    global test_report
    test_report += f"Result: {res}\n"


def print_test_report():
    global test_report
    print(test_report)
    test_report = ""  # Reset the report after printing


def test_description(desc):
    global test_report
    test_report += f"Test Description: {desc}\n"


def demo_test():
    step(1)
    description('This is a test step description')
    action('Short description of action')

    expected_result = '200'
    expected(f'Server response code should be: {expected_result}')
    # do the action here
    # get result
    received_result = '200'  # Simulated result for demonstration
    result(f'Server response code received: {received_result}')

    if received_result == expected_result:
        status('PASS')
    else:
        status('FAIL')
        add_to_report('Additional information about the failure')


# demo of test reporting module
if __name__ == "__main__":
    init_report('Demo Test Reporting Module')
    demo_test()
    print(test_report)
