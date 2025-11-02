
import tkinter as tk
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import (FigureCanvasTkAgg, NavigationToolbar2Tk)
from matplotlib.animation import FuncAnimation
import threading
import time
import my_serial



# Global variables
# Serial Comm
close_com_to_acu_controller = False
my_serial_closed = True
sample_time = [0]
sample_voltage = [0]
sample_current = [0]

lines_voltage = None
lines_current = None
canvas = None
plot_1 = None
plot_2 = None


# ACU
relay_1 = False
relay_2 = False
relay_3 = False
relay_4 = False

# APP
temp_acu_row = 0
temp_acu_C = 0
v_acu_hi_row = 0
v_acu_hi_mV = 0
v_acu_lo_row = 0
v_acu_lo_mV = 0
i_char_row = 0
i_char_mA = 0
i_dis_hi_row = 0
i_dis_hi_mA = 0
i_dis_lo_row = 0
i_dis_lo_mA = 0

sample_voltage_label = None


def main_menu_dummy_cmd(cmd="None"):
    print(cmd)


def menu_file(menu_bar=None, menu_root=None):
    filemenu = tk.Menu(menu_bar, tearoff=0)
    filemenu.add_command(label="New", command=main_menu_dummy_cmd)
    filemenu.add_command(label="Open", command=main_menu_dummy_cmd)
    filemenu.add_command(label="Save", command=main_menu_dummy_cmd)
    filemenu.add_separator()
    filemenu.add_command(label="Exit", command=menu_root.quit)
    menu_bar.add_cascade(label="File", menu=filemenu)


def menu_help(menu_bar=None):
    helpmenu = tk.Menu(menu_bar, tearoff=0)
    helpmenu.add_command(label="Help Index", command=main_menu_dummy_cmd)
    helpmenu.add_command(label="About...", command=main_menu_dummy_cmd)
    menu_bar.add_cascade(label="Help", menu=helpmenu)


def my_plot(m_frame=None):
    global sample_time, sample_voltage, sample_current
    global lines_voltage, lines_current, canvas, plot_1, plot_2

    fig, (plot_1, plot_2) = plt.subplots(nrows=2, ncols=1, figsize=(8, 6))
    lines_voltage, = plot_1.plot(sample_time, sample_voltage, label='Voltage')
    lines_current, = plot_2.plot(sample_time, sample_current, label='Current')
    canvas = FigureCanvasTkAgg(fig, master=m_frame)

    # pack_toolbar=False will make it easier to use a layout manager later on.
    toolbar = NavigationToolbar2Tk(canvas, m_frame, pack_toolbar=False)
    toolbar.update()
    toolbar.pack(side=tk.BOTTOM, fill=tk.X)
    canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)

# --------------------------------------------------- #
# ---------------  Serial --------------------------- #
# --------------------------------------------------- #

def serial_acu_read(my_sr=None, read_cmd=b'r:v:h:c;'):
    my_sr.flushInput()
    my_sr.write(read_cmd)
    resp = my_sr.readline()
    return my_serial.substract_digit(r=resp)


def serial_update_acu_controller(my_sr):
    global relay_1, relay_2, relay_3, relay_4
    my_serial.update_relay(com=my_sr, relay_number=1, status=relay_1)
    my_serial.update_relay(com=my_sr, relay_number=2, status=relay_2)
    my_serial.update_relay(com=my_sr, relay_number=3, status=relay_3)
    my_serial.update_relay(com=my_sr, relay_number=4, status=relay_4)


def serial_update_app(my_sr):
    global temp_acu_row, temp_acu_C, v_acu_hi_row, v_acu_hi_mV
    global v_acu_lo_row, v_acu_lo_mV, i_char_row, i_char_mA
    global i_dis_hi_row, i_dis_hi_mA, i_dis_lo_row, i_dis_lo_mA
    global sample_time, sample_voltage, sample_current, lines_voltage, lines_current, canvas
    global sample_voltage_label

    temp_acu_row = serial_acu_read(my_sr=my_sr, read_cmd=b'r:t:r;')
    # temp_acu_C = serial_acu_read(my_sr=my_sr, read_cmd=b'r:t:c;')
    v_acu_hi_row = serial_acu_read(my_sr=my_sr, read_cmd=b'r:v:h:r;')
    v_acu_hi_mV = serial_acu_read(my_sr=my_sr, read_cmd=b'r:v:h:c;')
    v_acu_lo_row = serial_acu_read(my_sr=my_sr, read_cmd=b'r:v:l:r;')
    v_acu_lo_mV = serial_acu_read(my_sr=my_sr, read_cmd=b'r:v:l:c;')
    i_char_row = serial_acu_read(my_sr=my_sr, read_cmd=b'r:c:r;')
    i_char_mA = serial_acu_read(my_sr=my_sr, read_cmd=b'r:c:c;')
    i_dis_hi_row = serial_acu_read(my_sr=my_sr, read_cmd=b'r:d:h:r;')
    i_dis_hi_mA = serial_acu_read(my_sr=my_sr, read_cmd=b'r:d:h:c;')
    i_dis_lo_row = serial_acu_read(my_sr=my_sr, read_cmd=b'r:d:l:r;')
    i_dis_lo_mA = serial_acu_read(my_sr=my_sr, read_cmd=b'r:d:l:c;')

    sample_time.append(sample_time[-1] + 1)
    sample_voltage.append(v_acu_hi_mV)
    sample_current.append(i_char_mA+i_dis_hi_mA)

    plot_1.clear()
    plot_2.clear()
    lines_voltage, = plot_1.plot(sample_time, sample_voltage)
    lines_current, = plot_2.plot(sample_time, sample_current, color="red")
    canvas.draw()

    sample_voltage_label.config(text="{} mV".format(v_acu_hi_mV))

    # print("sample_time {}". format(sample_time))
    # print("Temp {} [raw]".format(temp_acu_row))
    # print("V acu hi in {} [mV]".format(v_acu_hi_mV))
    # print("V acu lo in {} [mV]".format(v_acu_lo_mV))
    # print("I char {} [mA]".format(i_char_mA))


def serial_com_worker():
    print("Serial worker invoked")
    global close_com_to_acu_controller, my_serial_closed
    # init serial
    my_sr = my_serial.init_serial()
    time.sleep(0.1)

    # Main loop
    close_com_to_acu_controller = False
    while close_com_to_acu_controller is False:
        serial_update_acu_controller(my_sr=my_sr)
        serial_update_app(my_sr=my_sr)
        time.sleep(0.5)

    my_serial.reset_relays(com=my_sr)
    my_sr.close()
    print("Serial com closed")
    time.sleep(1)
    my_serial_closed = True


def connet_to_controller():
    global my_serial_closed

    if my_serial_closed is False:
        print("Com already open")
    else:
        print("Connecting...")
        try:
            serial_com_thread = threading.Thread(target=serial_com_worker)
            serial_com_thread.start()
            serial_com_thread.join(timeout=0.1)
            my_serial_closed = False

        except:
            print("Error")


def disconnect_from_controller():
    global my_serial_closed
    global close_com_to_acu_controller

    if my_serial_closed is True:
        print("Com already closed")
    else:
        print("Disconnecting...")
        try:
            close_com_to_acu_controller = True
            print("Running threads {}".format(threading.enumerate()))

        except:
            print("Error")


# --------------------------------------------------- #
# ---------------  Options -------------------------- #
# --------------------------------------------------- #
def my_options(o_frame=None, menu_root=None):
    global sample_voltage_label

    r1_status = tk.BooleanVar()
    r2_status = tk.BooleanVar()
    r3_status = tk.BooleanVar()
    r4_status = tk.BooleanVar()

    def o_frame_get_relays_state():
        global relay_1, relay_2, relay_3, relay_4

        if r1_status.get(): relay_1 = True
        else: relay_1 = False
        if r2_status.get(): relay_2 = True
        else: relay_2 = False
        if r3_status.get(): relay_3 = True
        else: relay_3 = False
        if r4_status.get(): relay_4 = True
        else: relay_4 = False

    def o_exit(e_root=menu_root):
        global close_com_to_acu_controller
        if my_serial_closed is False:
            close_com_to_acu_controller = True
            while my_serial_closed is False:
                time.sleep(0.01)
        e_root.quit()

    # creating grid
    for col in range(0, 40):
        tk.Label(o_frame, text='.', width=2).grid(column=col, row=0)

    sample_voltage_label = tk.Label(o_frame, text="   0 mV", width=10)
    sample_voltage_label.grid(column=10, columnspan=10, row=2)
    tk.Label(o_frame, text='ACU controller', width=14).grid(column=0, columnspan=7, row=3)
    tk.Button(o_frame, text='Connect', width=12, command=connet_to_controller).grid(column=1, columnspan=6, row=4)
    tk.Button(o_frame, text='Disconnect', width=12, command=disconnect_from_controller).grid(column=8, columnspan=6, row=4)
    tk.Checkbutton(o_frame, text='R1', variable=r1_status, command=o_frame_get_relays_state).grid(column=0, columnspan=2, row=5, sticky="w")
    tk.Checkbutton(o_frame, text='R2', variable=r2_status, command=o_frame_get_relays_state).grid(column=3, columnspan=2, row=5, sticky="w")
    tk.Checkbutton(o_frame, text='R3', variable=r3_status, command=o_frame_get_relays_state).grid(column=6, columnspan=2, row=5, sticky="w")
    tk.Checkbutton(o_frame, text='R4', variable=r4_status, command=o_frame_get_relays_state).grid(column=9, columnspan=2, row=5, sticky="w")
    tk.Button(o_frame, text='Exit', width=12, command=o_exit).grid(column=30, columnspan=6, row=6)


def main_window():
    root = tk.Tk()
    root.title("ACU tester")
    # Menu
    menubar = tk.Menu(root)
    menu_file(menu_bar=menubar, menu_root=root)
    menu_help(menu_bar=menubar)
    root.config(menu=menubar)

    # Options frame
    options_frame = tk.Frame(root)
    options_frame.pack(side="top", fill=tk.BOTH, expand=True)
    my_options(o_frame=options_frame, menu_root=root)

    # Plot main frame
    plot_frame = tk.Frame(root)
    plot_frame.pack(side="bottom", fill=tk.BOTH, expand=True)
    # Ploting
    my_plot(m_frame=plot_frame)

    # Main loop
    root.mainloop()


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    main_window()
