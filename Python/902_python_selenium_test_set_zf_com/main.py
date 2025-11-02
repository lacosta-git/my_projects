from selenium import webdriver
from selenium.webdriver.common.keys import Keys
import time


def test_001_open_page():
    print('START: Test 001 - open page')
    driver = webdriver.Edge()
    driver.get('https://www.python.org')
    print(driver.title)
    time.sleep(3)
    driver.close()
    print('END: Test 001')


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    print('START')
    test_001_open_page()
    print('STOP')

