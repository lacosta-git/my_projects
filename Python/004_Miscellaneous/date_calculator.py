# generate function to calculate the difference between two dates
from datetime import datetime


def calculate_date_difference(date1, date2):
    """
    Calculate the difference in days between two dates.

    :param date1: First date in 'YYYY-MM-DD' format.
    :param date2: Second date in 'YYYY-MM-DD' format.
    :return: Difference in days as an integer.
    """
    d1 = datetime.strptime(date1, '%Y-%m-%d')
    d2 = datetime.strptime(date2, '%Y-%m-%d')
    return abs((d2 - d1).days)

# Example usage
if __name__ == '__main__':
    date1 = '2023-10-01'
    date2 = '2023-10-15'
    difference = calculate_date_difference(date1, date2)
    print(f"The difference between {date1} and {date2} is {difference} days.")

