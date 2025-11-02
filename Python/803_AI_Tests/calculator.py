import tkinter as tk

# Function to update the display
def button_click(value):
    current = display.get()
    display.delete(0, tk.END)  # Clear the current display
    display.insert(tk.END, current + value)  # Add the new value to the display

# Function to clear the display
def button_clear():
    display.delete(0, tk.END)

# Function to evaluate the expression
def button_equal():
    try:
        result = eval(display.get())  # Evaluate the expression
        display.delete(0, tk.END)
        display.insert(tk.END, result)
    except Exception as e:
        display.delete(0, tk.END)
        display.insert(tk.END, "Error")

# Create the main window
root = tk.Tk()
root.title("Simple Calculator")

# Create the display
display = tk.Entry(root, width=25, font=("Arial", 14), borderwidth=2, relief="solid")
display.grid(row=0, column=0, columnspan=4)

# Button Layout (buttons for digits, operators, and functions)
buttons = [
    ('7', 1, 0), ('8', 1, 1), ('9', 1, 2), ('/', 1, 3),
    ('4', 2, 0), ('5', 2, 1), ('6', 2, 2), ('*', 2, 3),
    ('1', 3, 0), ('2', 3, 1), ('3', 3, 2), ('-', 3, 3),
    ('0', 4, 0), ('.', 4, 1), ('+', 4, 2), ('=', 4, 3),
    ('C', 5, 0)
]

# Create the buttons and add them to the grid
for (text, row, col) in buttons:
    if text == "=":
        button = tk.Button(root, text=text, width=5, height=2, font=("Arial", 14), command=button_equal)
    elif text == "C":
        button = tk.Button(root, text=text, width=5, height=2, font=("Arial", 14), command=button_clear)
    else:
        button = tk.Button(root, text=text, width=5, height=2, font=("Arial", 14), command=lambda value=text: button_click(value))
    
    button.grid(row=row, column=col, padx=5, pady=5)

# Start the Tkinter event loop
root.mainloop()
