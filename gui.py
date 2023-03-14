import struct
import tkinter
import customtkinter as ctk
import serial.tools.list_ports
import time
from struct import *
from time import gmtime

N_VS = 9
N_TS = 3
N_SLAVES = 1

UPDATE_FREQ = 200

MAX_TEMP = 60
MIN_TEMP = 20
MAX_VOLT = 4.2
MIN_VOLT = 3.3

# H -> half_word (2 Byte),  ? -> bool (1 Byte),  c -> char (1 Byte)
STR_FORMAT = "H" * (N_VS + N_TS) + "c?"
size_struct = struct.calcsize(STR_FORMAT)
print(size_struct)

ser = serial.Serial(timeout=0.1)


def chg_appearance(new_appearance_mode: str):
    ctk.set_appearance_mode(new_appearance_mode)


class App(ctk.CTk):

    def configuration_frame(self):
        tabview = ctk.CTkTabview(self, state="disable", segmented_button_selected_color="chartreuse4", width=240)
        tabview.grid(row=0, column=0, padx=(20, 0))
        tabview.add("CONFIGURATION")
        tabview.tab("CONFIGURATION").grid_columnconfigure(0, weight=1)

        self.switch = ctk.CTkSwitch(tabview.tab("CONFIGURATION"), text=f"OFF/ON", command=self.switch_event, switch_width=60, switch_height=30)
        self.switch.grid(row=3, column=0, padx=20, pady=(30, 10))

        self.option_COM = ctk.CTkOptionMenu(tabview.tab("CONFIGURATION"))
        self.get_COM()
        self.option_COM.grid(row=0, column=0, padx=20, pady=(15, 5))

        baud_frame = ctk.CTkFrame(tabview.tab("CONFIGURATION"))
        baud_frame.grid(row=1, column=0, padx=(20, 20), pady=(5, 0), sticky="nwse")
        baud_frame.grid_columnconfigure(0, weight=1)

        self.baud_var = tkinter.IntVar(value=115200)
        button1 = ctk.CTkRadioButton(baud_frame, variable=self.baud_var, value=115200, text="115200  bit/s")
        button1.grid(row=1, column=0, pady=(10, 5))
        button2 = ctk.CTkRadioButton(baud_frame, variable=self.baud_var, value=9600, text="9600  bit/s    ")
        button2.grid(row=2, column=0, pady=(5, 10))

        mode_frame = ctk.CTkFrame(tabview.tab("CONFIGURATION"))
        mode_frame.grid(row=2, column=0, padx=(20, 20), pady=(5, 0), sticky="nwse")
        mode_frame.grid_columnconfigure(0, weight=1)

        self.mode = tkinter.StringVar(value="Normal Mode")
        normal = ctk.CTkRadioButton(master=mode_frame, variable=self.mode, value="Normal Mode", text="Normal Mode     ", command=self.set_mode)
        normal.grid(row=1, column=0, pady=(10, 5))
        silent = ctk.CTkRadioButton(master=mode_frame, variable=self.mode, value="Sleep Mode", text="Sleep Mode        ", command=self.set_mode)
        silent.grid(row=2, column=0, pady=(5, 5))
        balancing = ctk.CTkRadioButton(master=mode_frame, variable=self.mode, value="Balancing Mode", text="Balancing Mode", command=self.set_mode)
        balancing.grid(row=3, column=0, pady=(5, 10))


        refresh = ctk.CTkButton(tabview.tab("CONFIGURATION"), fg_color="transparent", border_width=2, text_color=("gray10", "#DCE4EE"), text="Refresh Serial",
                                command=self.get_COM)
        refresh.grid(row=4, column=0, padx=(20, 20), pady=(15, 5), sticky="nwse")
        reset = ctk.CTkButton(tabview.tab("CONFIGURATION"), fg_color="transparent", border_width=2, text_color=("gray10", "#DCE4EE"), text="Reset Serial",
                              command=self.on_reset)
        reset.grid(row=5, column=0, padx=(20, 20), pady=(5, 15), sticky="nwse")

    def appearence_frame(self):
        appearence_frame = ctk.CTkFrame(self)
        appearence_frame.grid(row=2, column=0, padx=(20, 0), pady=(20, 0), sticky="nsew")
        appearence_frame.grid_columnconfigure(0, weight=1)
        appearence_frame.grid_rowconfigure(0, weight=1)

        option = ctk.CTkOptionMenu(appearence_frame, values=["System", "Light", "Dark"], command=chg_appearance)
        option.grid(row=0, column=0, pady=(8, 8))

    def information_frame(self):

        box1 = ctk.CTkFrame(master=self)
        box1.grid(row=0, column=1, padx=(10, 10), pady=(20,20), sticky="nsew", rowspan=2, ipadx=3)

        for i in range(N_VS):
            label = ctk.CTkLabel(box1, text="Cell " + str(i + 1), fg_color=("gray70", "gray25"), corner_radius=4, width=50)
            label.grid(column=0, row=i + 1, sticky="nsew", pady=(5, 5), padx=(5, 5))

        for i in range(N_VS, N_VS + N_TS):
            label = ctk.CTkLabel(box1, text="Tmp " + str(i - N_VS + 1), fg_color=("gray70", "gray25"), corner_radius=4)
            label.grid(column=0, row=i + 1, sticky="nsew", pady=(5, 5), padx=(5, 5))

        for i in range(1, N_SLAVES + 1):

            label = ctk.CTkLabel(box1, text="Slv " + str(i), fg_color=("gray70", "gray25"), corner_radius=4, width=49)
            label.grid(column=i, row=0, sticky="nsew", padx=(5, 5), pady=(20, 5))

            s = list()
            for j in range(1, N_VS + N_TS + 1):
                label = ctk.CTkLabel(box1, text="0", fg_color=("gray80", "gray15"), corner_radius=4, text_color="black", width=49)
                label.grid(column=i, row=j, sticky="nsew", padx=(2, 2), pady=(2, 2))
                s.append(label)

            self.total_pack_labels.append(s)

        # Bottom BOX with additional information
        box2 = ctk.CTkFrame(master=self)
        box2.grid(row=2, column=1, padx=(10, 10), pady=5, sticky="nsew", rowspan=2)

        # min_volt, max_volt, tot_volt, min_temp, max_temp, tot_temp

        list_num = ["MIN VOLT", "MAX VOLT", "AVG VOLT", "TOT VOLT", "MIN TEMP", "MAX TEMP", "AVG TEMP", "SoC", "TOT POWER", "CURRENT"]
        self.list_info = []

        for i in range(len(list_num)):
            box2.columnconfigure(i, weight=1)
            label = ctk.CTkLabel(box2, text=list_num[i], fg_color=("gray70", "gray25"), corner_radius=4)
            label.grid(column=i, row=0, sticky="nsew", pady=(5, 5), padx=(5, 5))

            label2 = ctk.CTkLabel(box2, text="0", fg_color=("gray80", "gray15"), corner_radius=4)
            label2.grid(column=i, row=1, sticky="nsew", pady=(5, 5), padx=(5, 5))
            self.list_info.append(label2)

    def __init__(self):
        super().__init__()

        self.total_pack_labels = list()
        self.title("BMS UI")
        self.resizable(False, False)
        self.geometry("+20+20")  # top left corner
        self.configuration_frame()

        # create textbox
        self.textbox = ctk.CTkLabel(self, height=100, fg_color=("gray70", "gray10"), corner_radius=5)
        self.textbox.grid(row=1, column=0, padx=(20, 0), pady=(10, 0), sticky="nsew")

        self.appearence_frame()
        self.information_frame()

        self.update()

    def get_COM(self):
        connected = [port.device for port in serial.tools.list_ports.comports()]
        if len(connected) == 0:
            connected = ["No Device Found"]
            self.switch.deselect()
        self.option_COM.configure(values=connected)
        self.option_COM.set(connected[0])

    def on_reset(self):
        self.switch.deselect()
        self.switch_event()

    def set_mode(self):
        if self.switch.get() == 0:
            return

        if self.mode.get() == "Normal Mode":
            mode = "N"
        elif self.mode.get() == "Sleep Mode":
            mode = "S"
        else:
            mode = "B"

        ser.write(bytes(mode, 'utf-8'))
        ser.flush()         # todo pulire buffer invio host

        mustend = time.time() + 2
        while time.time() < mustend:
            if ser.in_waiting:
                break
        else:
            self.textbox.configure(text="No ACK received")
            raise Exception

        ack = ser.readline().decode("ascii")

        if ack != mode:
            self.textbox.configure(text="Received wrong ACK")
            raise Exception

        ser.flush()
        time.sleep(UPDATE_FREQ / 1000)

    def switch_event(self):
        global ser
        if self.switch.get() == 0:
            ser.close()
        else:
            if self.option_COM.get() == "No Device Found":
                self.switch.deselect()
                self.textbox.configure(text="Select a correct Serial Port")
            else:
                try:
                    ser.baudrate = self.baud_var.get()
                    ser.port = self.option_COM.get()
                    ser.open()
                    time.sleep(0.4)
                    self.set_mode()

                except Exception as e:
                    print(e)
                    self.switch.deselect()
                    self.get_COM()
                    self.textbox.configure(text="Select a correct Serial Port")


    def update_gui_normal(self):

        try:
            ser.flush()

            while ser.in_waiting < size_struct * N_SLAVES:
                pass

            resp = ser.read(size_struct * N_SLAVES)

        except Exception:
            self.get_COM()
            # if switch is still on:
            if self.switch.get() == 1:
                self.textbox.configure(text="Error Unpacking")
            else:
                ser.close()

            return 0

        # min_volt, max_volt, avg_volt, tot_volt,  min_temp, max_temp, tot_temp
        add_infos = [0] * 7
        add_infos[0] = MAX_VOLT * 10
        add_infos[4] = MAX_TEMP * 10

        for i in range(N_SLAVES):
            cell_value = unpack(STR_FORMAT, resp[i * size_struct: (i + 1) * size_struct])

            for j in range(N_VS):
                if cell_value[N_VS+N_TS+1]:
                    self.total_pack_labels[i][j].configure(text="💀", fg_color="gray")
                else:
                    value = round(cell_value[j] / 10000, 2)
                    self.total_pack_labels[i][j].configure(text=str(value), fg_color=rgb(value, MIN_VOLT, MAX_VOLT))
                    # add_infos[0] = min(add_infos[0], value)
                    # add_infos[1] = max(add_infos[1], value)
                    # add_infos[2] += value
                    # add_infos[3] += value

            for j in range(N_VS, N_VS + N_TS):
                if cell_value[N_VS + N_TS + 1]:
                    self.total_pack_labels[i][j].configure(text="💀", fg_color="gray")
                else:
                    value = round(cell_value[j], 2)
                    self.total_pack_labels[i][j].configure(text=str(value), fg_color=rgb(value, MIN_TEMP, MAX_TEMP))
                    # add_infos[4] = min(add_infos[4], value)
                    # add_infos[5] = max(add_infos[5], value)
                    # add_infos[6] += value
        #
        # add_infos[2] /= (N_SLAVES*N_VS)
        # add_infos[6] /= (N_SLAVES*N_TS)
        #
        # for i in range(len(add_infos)):
        #     self.list_info[i].configure(text=str(round(add_infos[i],2)))
        #     if i < 3:
        #         self.list_info[i].configure(fg_color=rgb(add_infos[i], MIN_VOLT, MAX_VOLT))
        #     if 3 < i < 7:
        #         self.list_info[i].configure(fg_color=rgb(add_infos[i], MIN_TEMP, MAX_TEMP))

    def update_silent(self):
        print("silent")

    def update_bilancing(self):
        print("bilance")

    def update(self):
        if self.switch.get() == 0:
            if self.textbox.cget("text") != "Select a correct Serial Port":
                self.textbox.configure(text="Select the settings and press ON")

        if self.switch.get() == 1:
            self.textbox.configure(text="Selected port is ON and listen")
            if self.mode.get() == "Normal Mode":
                self.update_gui_normal()
            elif self.mode.get() == "Sleep Mode":
                self.update_silent()
            else:
                self.update_bilancing()

        self.after(60, self.update)


def rgb(val, min_val, max_val, a=0.6):
    x = int((val - min_val) / (max_val - min_val) * 256)
    r = x * 2 if x < 128 else 255
    g = 255 if x < 128 else 255 - (x - 128) * 2
    newColor = tuple(int(x + (1 - x) * (1 - a)) for x in [r, g, 0])
    newColor = [min(max(x, 0), 255) for x in newColor]
    return "#%s%s%s" % tuple([hex(c)[2:].rjust(2, "0") for c in newColor])
    # return "gray"


if __name__ == "__main__":
    app = App()
    app.mainloop()
