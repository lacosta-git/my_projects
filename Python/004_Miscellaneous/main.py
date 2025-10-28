
import date_calculator as dt


# Function prints a greeting message
def print_hi(name):
    # Use a breakpoint in the code line below to debug your script.
    print(f'Hi, you must be {name}')  # Press Ctrl+F8 to toggle the breakpoint.


# Function to calculate Fahrenheit temperature based on given Celsius
def calculate_fahrenheit(temp_C=None):
    return (temp_C * 9/5) + 32


# Function to calculate Celsius temperature based on given Fahrenheit
def calculate_celsius(temp_F=None):
    return (temp_F - 32) * 5/9


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    print_hi('PyCharm is')
    temp_c = 45.4
    print("Temp {0} C is {1:.1f} F".format(temp_c, calculate_fahrenheit(temp_C=temp_c)))
    temp_f = 44.0

    print("Temp {0} F is {1:.1f} C".format(temp_f, calculate_celsius(temp_F=temp_f)))
    date1 = '2023-10-01'
    date2 = '2023-10-15'
    difference = dt.calculate_date_difference(date1, date2)
    print(f"The difference between {date1} and {date2} is {difference} days.")