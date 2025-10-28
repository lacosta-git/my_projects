import random
import curses
import time


def matrix_effect(stdscr):
    # Set up the screen
    curses.curs_set(0)  # Hide the cursor
    stdscr.nodelay(1)  # Non-blocking input
    stdscr.timeout(100)  # Refresh rate in milliseconds

    height, width = stdscr.getmaxyx()  # Get screen dimensions
    columns = width // 2  # Define the number of columns

    # Create a list to hold the 'falling' letters in each column
    drops = [0] * columns
    brightness = [0] * columns  # To store the current brightness level of each column

    # The alphabet and characters to be used
    chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789@#$%^&*()"

    # Define color pairs to simulate fading (we will use 8 shades of green)
    for i in range(8):
        curses.init_pair(i + 1, curses.COLOR_GREEN, curses.COLOR_BLACK)

    while True:
        stdscr.clear()  # Clear the screen

        for i in range(columns):
            # Randomly pick a character and display it
            char = random.choice(chars)
            x = i * 2  # Space out the columns a little for visual effect
            y = drops[i]

            # Determine the current brightness level (fading effect)
            fade_level = brightness[i]

            # Set color pair to simulate fading
            stdscr.addstr(y, x, char, curses.color_pair(fade_level + 1))

            # Move the drop position and update the brightness
            drops[i] += 1
            brightness[i] = min(fade_level + 1, 7)  # Increase brightness over time

            if drops[i] >= height:
                drops[i] = 0  # Reset drop to the top when it reaches the bottom
                brightness[i] = 0  # Reset the brightness

        stdscr.refresh()  # Refresh the screen to show the new frame
        time.sleep(0.05)  # Adjust the speed of the falling effect


# Initialize the curses application
def main():
    curses.initscr()
    curses.start_color()
    # We use 8 shades of green (color pairs 1 to 8)
    try:
        matrix_effect(curses.initscr())
    finally:
        curses.endwin()


if __name__ == "__main__":
    main()
