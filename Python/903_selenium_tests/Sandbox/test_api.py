"""
File contains set of functions used in testing.
"""
from time import sleep


def open_url(url='https://www.python.org', browser='Edge'):
    """
    Opens a URL in the specified browser.

    :param url: URL to open, default is 'https://www.python.org'
    :param browser: Browser to use, default is 'Edge'
    :return: Handle to the opened browser window
    """
    from selenium import webdriver

    if browser == 'Edge':
        w_driver = webdriver.Edge()
    elif browser == 'Chrome':
        w_driver = webdriver.Chrome()
    else:
        raise ValueError(f"Unsupported browser: {browser}")
    w_driver.get(url)
    return w_driver


if __name__ == "__main__":
    # Example usage
    try:
        driver = open_url()
        print("Page opened successfully.")
        sleep(3)  # Wait for a few seconds to see the page
    except Exception as e:
        print(f"An error occurred: {e}")
    finally:
        if 'driver' in locals():
            driver.close()
            print("Browser closed.")

