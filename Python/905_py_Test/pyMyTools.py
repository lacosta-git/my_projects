import tkinter as tk
from tkinter.font import Font
from tkinter import messagebox


# Entry globals
m_entry = None


def scroll_entry_text():
    m_entry_len = len(m_entry.get())
    m_entry.xview(m_entry_len)


def insert(m_text=''):
    global m_entry
    m_entry.insert(index=len(m_entry.get()), string=m_text)
    scroll_entry_text()


def init_scroll_bar(master=''):
    entry_scroll_bar = tk.Scrollbar(master=master,
                                    orient=tk.HORIZONTAL)
    return entry_scroll_bar


def wr_insert(m_t=''):
    def wrapper(my_t=m_t):
        insert(m_text=my_t)
    return wrapper


def cmd_pressed_equal():
    m_equal = ''
    try:
        m_equal = eval(m_entry.get())
    except Exception:
        m_equal = 'ERROR'
    insert(m_text='={}'.format(m_equal))


def cmd_pressed_clear():
    m_entry_current = m_entry.get()
    m_entry.delete(first=0, last=len(m_entry_current))


def w_cmd_pressed_clear():
    cmd_pressed_clear()
    return w_cmd_pressed_clear


def init_main_frame():
    main_frame = tk.Tk()
    return main_frame


def init_display(master='', entry_scroll=''):
    m_entry = tk.Entry(master=master,
                       justify=tk.RIGHT,
                       width=19,
                       font=Font(family='Courier New', size=16),
                       xscrollcommand=entry_scroll.set)
    return m_entry


def init_buttons(master=''):
    buttons = []
    b_font = Font(family='Courier New', size=16)
    m_buttons = ['1', '2', '3', '+',
                 '4', '5', '6', '-',
                 '7', '8', '9', '*',
                 '.', '0', '/']

    for b_entry in m_buttons:
        m_button = tk.Button(master=master,
                             text=b_entry,
                             command=wr_insert('{}'.format(b_entry)),
                             font=b_font,
                             width=4)
        buttons.append(m_button)

    # button text, function suffix, width, bg color
    m_buttons = [['=', 'equal', 4, 'blue'],
                 ['Clear', 'clear', 8, 'red']]
    for b_entry in m_buttons:
        m_button = tk.Button(master=master,
                             text=b_entry[0],
                             command=eval('cmd_pressed_{}'.format(b_entry[1])),
                             font=b_font,
                             width=b_entry[2],
                             bg=b_entry[3])
        buttons.append(m_button)

    # row, column, columnspan
    b_grid = [[3, 1, 1], [3, 2, 1], [3, 3, 1], [3, 4, 1],
              [4, 1, 1], [4, 2, 1], [4, 3, 1], [4, 4, 1],
              [5, 1, 1], [5, 2, 1], [5, 3, 1], [5, 4, 1],
              [6, 1, 1], [6, 2, 1], [6, 4, 1], [6, 3, 1],
              [7, 1, 4]]
    return buttons, b_grid


def msg_about():
    about_txt = '''    This is simple calculator based on examples
form web. It has very limited error protection therefore
you must be very careful when using it.

Tomasz Szkudlarek 14.10.2018
    '''
    messagebox.showinfo(title='About...',
                        message=about_txt)


def menu_bar(master=''):
    menubar = tk.Menu(master=master)

    filemenu = tk.Menu(menubar, tearoff=0)
    filemenu.add_command(label="New", command=w_cmd_pressed_clear)
    filemenu.add_command(label="Exit", command=master.quit)
    menubar.add_cascade(label="File", menu=filemenu)

    helpmenu = tk.Menu(menubar, tearoff=0)
    helpmenu.add_command(label="About...", command=msg_about)
    menubar.add_cascade(label="Help", menu=helpmenu)

    return menubar


def main_gui():
    main_frame = init_main_frame()

    m_scroll = init_scroll_bar(master=main_frame)
    m_scroll.grid(row=2, column=1, columnspan=4, sticky=tk.E+tk.W)

    global m_entry
    m_entry = init_display(master=main_frame, entry_scroll=m_scroll)
    m_entry.grid(row=1, column=1, columnspan=4)
    m_entry.focus()
    m_scroll.config(command=m_entry.xview)

    m_buttons, b_grid = init_buttons(master=main_frame)
    for b_index in range(len(m_buttons)):
        m_buttons[b_index].grid(row=b_grid[b_index][0],
                                column=b_grid[b_index][1],
                                columnspan=b_grid[b_index][2])

    menu_line = menu_bar(master=main_frame)
    main_frame.config(menu=menu_line)
    main_frame.mainloop()


if __name__ == '__main__':
    main_gui()
