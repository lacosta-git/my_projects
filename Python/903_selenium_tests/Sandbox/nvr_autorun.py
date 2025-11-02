
"""
Open the terminal in administrator then paste:
Disable Edge updates:
netsh advfirewall firewall add rule name="Disable Edge Updates" dir=out action=block program="C:\Program Files (x86)\Microsoft\EdgeUpdate\MicrosoftEdgeUpdate.exe"

To reenable updates:
netsh.exe advfirewall firewall delete rule name="Disable Edge Updates" program="C:\Program Files (x86)\Microsoft\EdgeUpdate\MicrosoftEdgeUpdate.exe"

"""
import time

from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from time import sleep


def open_url(url='http://192.168.9.95'):
    driver = webdriver.Edge()
    driver.get(url)
    sleep(3)
    response = driver.title
    print(f'Title received: {response}')

    username = driver.find_element(By.ID, 'userName')
    username.click()
    username.send_keys('admin')
    time.sleep(1)

    password = driver.find_element(By.ID, 'pwd')
    password.click()
    password.send_keys('admin123')
    time.sleep(1)

    login_button = driver.find_element(By.XPATH, "//button[text()='Login']")
    login_button.click()

    time.sleep(1000)

    driver.close()


if __name__ == "__main__":
    # Example usage
    try:
        open_url()
        print("Page opened successfully.")
        sleep(3)  # Wait for a few seconds to see the page
    except Exception as e:
        print(f"An error occurred: {e}")
    finally:
        if 'driver' in locals():
            driver.close()
            print("Browser closed.")