from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from time import sleep
from test_reporting import *


# This script uses Selenium to open a web page and print its title.
def test_001_open_page(url='https://www.python.org'):
    test_description('Open a web page and check its title')
    step(1)
    action(f'Open a web page: {url}')
    expected('The page should load successfully and display its title.')
    exp = 'ROBOBIONIC'
    expected(f'Title contain: {exp}')
    driver = webdriver.Edge()
    driver.get(url)
    sleep(3)
    response = driver.title
    result(f'Title received: {response}')
    if exp in response:
        status('PASS')
    else:
        status('FAIL')
        add_to_report(f'Title does not contain expected text: {exp}')
    driver.close()


# This script uses Selenium to open a web page and print its title.
def test_002_menu_home(url='https://www.python.org'):
    test_description('Open a web page and check its menu')
    try:
        driver = webdriver.Edge()
        # Set a timeout for waiting for elements
        wait = WebDriverWait(driver, 10)

        step(1)
        action(f'Open a web page: {url}')
        expected('The page should load successfully and display its menu.')
        driver.get(url)
        locator = (By.LINK_TEXT, 'HOME')
        wait.until(lambda d: d.find_element(*locator))
        response = driver.find_element(*locator).text
        result(f'Menu item received: {response}')
        if response == 'HOME':
            status('PASS')
        else:
            status('FAIL')
            add_to_report(f'Menu item does not match expected: HOME, received: {response}')

    except Exception as e:
        status('FAIL')
        add_to_report(f'General Error during test. See log for details: {e}')
        return
    finally:
        driver.close()
        add_to_report('Test completed and browser closed.')


if __name__ == "__main__":
    print('START')
    test_001_open_page(url='https://www.robobionic.eu')
    test_002_menu_home(url='https://www.robobionic.eu')
    print_test_report()
    print('END')