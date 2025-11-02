import time
import random
import matplotlib.pyplot as plt
from collections import deque

# --- Accumulator Configuration ---
MAX_CAPACITY = 100.0  # Maximum capacity of the accumulator (e.g., in %)
MIN_CAPACITY = 0.0  # Minimal capacity of the accumulator (e.g., in %)
CHARGE_RATE = 2.0  # Percentage of capacity gained per second during charging
DISCHARGE_RATE = 1.0  # Percentage of capacity lost per second during discharging
INITIAL_SOC = 50.0  # Initial State of Charge (SoC)
MAX_PLOT_POINTS = 60  # Number of data points to display on the plot (last 60 seconds)

# --- State Variables ---
current_soc = INITIAL_SOC
is_charging = False  # Flag indicating whether the accumulator is charging
cycle_count = 0  # Simulation cycle counter

# --- Matplotlib Setup ---
# Deque to store the last N SoC values and timestamps for plotting
time_data = deque(maxlen=MAX_PLOT_POINTS)
soc_data = deque(maxlen=MAX_PLOT_POINTS)

# Set up the plot
plt.style.use('ggplot')  # Use a nice ggplot style for the plot
fig, ax = plt.subplots(figsize=(10, 6))  # Create a figure and an axes object
line, = ax.plot([], [], 'b-', label='State of Charge (SoC)')  # Initialize an empty plot line
ax.set_title('Accumulator Charging/Discharging Process')
ax.set_xlabel('Time (seconds)')
ax.set_ylabel('State of Charge (%)')
ax.set_ylim(MIN_CAPACITY - 5, MAX_CAPACITY + 5)  # Add some padding to y-axis limits
ax.set_xlim(0, MAX_PLOT_POINTS)  # X-axis will show the last MAX_PLOT_POINTS
ax.grid(True)
ax.legend()

# Enable interactive mode for matplotlib (important for real-time updates)
plt.ion()
plt.show()

print("--- Accumulator Charging/Discharging Process Simulation ---")
print(f"Initial State of Charge (SoC): {current_soc:.1f}%")
print("Waiting for data (simulating every 1 second)...")
print("A plot window will appear showing the SoC over time.")


def simulate_serial_data():
    """
    Simulates receiving data from a serial port.
    In a real application, this would be code to read from the port.
    Here, we randomly change the operating mode (charge/discharge)
    or base it on the State of Charge.
    """
    global is_charging, cycle_count

    # Every 10 seconds, simulate a mode change
    if cycle_count % 10 == 0 and cycle_count > 0:
        is_charging = not is_charging
        if is_charging:
            print("\n--- Mode changed to CHARGING ---")
        else:
            print("\n--- Mode changed to DISCHARGING ---")

    # Additional logic: if accumulator is full, force discharge
    # Subtract CHARGE_RATE to avoid oscillating when current_soc is exactly MAX_CAPACITY
    if current_soc >= MAX_CAPACITY - CHARGE_RATE and is_charging:
        is_charging = False
        print("\n--- Accumulator full, switching to DISCHARGING ---")

    # Additional logic: if accumulator is empty, force charge
    # Add DISCHARGE_RATE to avoid oscillating when current_soc is exactly MIN_CAPACITY
    if current_soc <= MIN_CAPACITY + DISCHARGE_RATE and not is_charging:
        is_charging = True
        print("\n--- Accumulator discharged, switching to CHARGING ---")

    return is_charging  # We return the mode flag directly


try:
    while True:
        cycle_count += 1
        # print(f"\n--- Cycle {cycle_count} ---") # Mute for less console clutter

        # 1. Simulate data reception from serial port (every 1 second)
        should_charge = simulate_serial_data()

        # 2. Process data and update accumulator state
        if should_charge:
            # Accumulator is charging
            current_soc += CHARGE_RATE
            status_text = "CHARGING"
        else:
            # Accumulator is discharging
            current_soc -= DISCHARGE_RATE
            status_text = "DISCHARGING"

        # Ensure SoC stays within the allowed range
        current_soc = max(MIN_CAPACITY, min(MAX_CAPACITY, current_soc))

        # 3. Update plot data
        time_data.append(cycle_count)
        soc_data.append(current_soc)

        # Update the plot line
        line.set_data(list(time_data), list(soc_data))

        # Adjust x-axis limits dynamically to show the last MAX_PLOT_POINTS
        ax.set_xlim(max(0, cycle_count - MAX_PLOT_POINTS), cycle_count)

        # Redraw the plot
        plt.draw()
        plt.pause(0.001)  # Small pause to allow plot to update, non-blocking

        # 4. Wait for the next cycle (simulating 1-second interval)
        time.sleep(1)

except KeyboardInterrupt:
    print("\n--- Simulation ended by user ---")
    plt.ioff()  # Turn off interactive mode
    plt.close(fig)  # Close the plot window
except Exception as e:
    print(f"\nAn error occurred: {e}")
    plt.ioff()
    plt.close(fig)

