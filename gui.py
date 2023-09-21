import json
import tkinter
import customtkinter as ctk
import serial.tools.list_ports
import time
import random

N_VS = 9
N_TS = 3
N_SLAVES = 16
MIN_ERR = 10
SIZE_PAYLOAD: int = 99
UPDATE_FREQ = 300

ser = serial.Serial(timeout=0.1)


def chg_appearance(new_appearance_mode: str):
    ctk.set_appearance_mode(new_appearance_mode)


class App(ctk.CTk):

    def configuration_frame(self):
        self.tabview = ctk.CTkTabview(self, state="disable", segmented_button_selected_color="chartreuse4", width=240)
        self.tabview.grid(row=0, column=0, padx=(20, 0))
        self.tabview.add("CONFIGURATION")
        self.tabview.tab("CONFIGURATION").grid_columnconfigure(0, weight=1)

        self.switch = ctk.CTkSwitch(self.tabview.tab("CONFIGURATION"), text=f"OFF/ON", command=self.switch_event, switch_width=60, switch_height=30)
        self.switch.grid(row=3, column=0, padx=20, pady=(30, 10))

        self.option_COM = ctk.CTkOptionMenu(self.tabview.tab("CONFIGURATION"))
        self.get_COM()
        self.option_COM.grid(row=0, column=0, padx=20, pady=(15, 5))

        baud_frame = ctk.CTkFrame(self.tabview.tab("CONFIGURATION"))
        baud_frame.grid(row=1, column=0, padx=(20, 20), pady=(5, 0), sticky="nwse")
        baud_frame.grid_columnconfigure(0, weight=1)

        self.baud_var = tkinter.IntVar(value=115200)
        button1 = ctk.CTkRadioButton(baud_frame, variable=self.baud_var, value=115200, text="115200  bit/s")
        button1.grid(row=1, column=0, pady=(10, 5))
        button2 = ctk.CTkRadioButton(baud_frame, variable=self.baud_var, value=9600, text="9600  bit/s    ")
        button2.grid(row=2, column=0, pady=(5, 10))

        mode_frame = ctk.CTkFrame(self.tabview.tab("CONFIGURATION"))
        mode_frame.grid(row=2, column=0, padx=(20, 20), pady=(5, 0), sticky="nwse")
        mode_frame.grid_columnconfigure(0, weight=1)

        self.mode = tkinter.StringVar(value="Normal Mode")
        normal = ctk.CTkRadioButton(master=mode_frame, variable=self.mode, value="Normal Mode", text="Normal Mode     ", command=self.set_mode)
        normal.grid(row=1, column=0, pady=(10, 5))
        silent = ctk.CTkRadioButton(master=mode_frame, variable=self.mode, value="Sleep Mode", text="Sleep Mode        ", command=self.set_mode)
        silent.grid(row=2, column=0, pady=(5, 5))
        balancing = ctk.CTkRadioButton(master=mode_frame, variable=self.mode, value="Balancing Mode", text="Balancing Mode", command=self.set_mode)
        balancing.grid(row=3, column=0, pady=(5, 10))

        refresh = ctk.CTkButton(self.tabview.tab("CONFIGURATION"), fg_color="transparent", border_width=2, text_color=("gray10", "#DCE4EE"),
                                text="Refresh Serial",
                                command=self.get_COM)
        refresh.grid(row=4, column=0, padx=(20, 20), pady=(15, 5), sticky="nwse")
        reset = ctk.CTkButton(self.tabview.tab("CONFIGURATION"), fg_color="transparent", border_width=2, text_color=("gray10", "#DCE4EE"), text="Reset Serial",
                              command=self.on_reset)
        reset.grid(row=5, column=0, padx=(20, 20), pady=(5, 15), sticky="nwse")

    def appearence_frame(self):
        appearence_frame = ctk.CTkFrame(self)
        appearence_frame.grid(row=2, column=0, padx=(20, 0), pady=(20, 0), sticky="nsew")
        appearence_frame.grid_columnconfigure(0, weight=1)
        appearence_frame.grid_rowconfigure(0, weight=1)

        option = ctk.CTkOptionMenu(appearence_frame, values=["System", "Dark", "Light"], command=chg_appearance)
        option.grid(row=0, column=0, pady=(8, 8))

    def information_frame(self):

        box1 = ctk.CTkFrame(master=self)
        box1.grid(row=0, column=1, padx=(10, 10), pady=(20, 20), sticky="nsew", rowspan=2, ipadx=3)

        for i in range(N_VS):
            label = ctk.CTkLabel(box1, text="Cell " + str(i + 1), fg_color=("gray70", "gray25"), corner_radius=4, width=52)
            label.grid(column=0, row=i + 1, sticky="nsew", pady=(5, 5), padx=(5, 5))

        for i in range(N_VS, N_VS + N_TS):
            label = ctk.CTkLabel(box1, text="Tmp " + str(i - N_VS + 1), fg_color=("gray70", "gray25"), corner_radius=4)
            label.grid(column=0, row=i + 1, sticky="nsew", pady=(5, 5), padx=(5, 5))

        for i in range(0, N_SLAVES):

            label = ctk.CTkCheckBox(box1, text="Slv " + str(i), fg_color=("gray70", "gray25"), corner_radius=4, width=52, command=self.deselect_slave)
            label.configure(command=lambda button=label: self.deselect_slave(button))

            label.select()
            label.grid(column=i + 1, row=0, sticky="nsew", padx=(5, 5), pady=(20, 5))

            s = list()
            for j in range(1, N_VS + N_TS + 1):
                label = ctk.CTkLabel(box1, text="0", fg_color=("gray80", "gray15"), corner_radius=4, text_color="black", width=52)
                label.grid(column=i + 1, row=j, sticky="nsew", padx=(2, 2), pady=(2, 2))
                s.append(label)

            self.total_pack_labels.append(s)

        # Bottom BOX with additional information
        box2 = ctk.CTkFrame(master=self)
        box2.grid(row=2, column=1, padx=(10, 10), pady=5, sticky="nsew", rowspan=2)

        # min_volt, max_volt, tot_volt, min_temp, max_temp, tot_temp

        list_num = ["MAX VOLT", "MIN VOLT", "BALANCING", "TOT VOLT", "AVG VOLT", "MAX TEMP", "MIN TEMP", "AVG TEMP", "CURRENT", "TOT POWER"]  # "SoC"
        self.list_info = []

        for i in range(len(list_num)):
            box2.columnconfigure(i, weight=1)
            label = ctk.CTkLabel(box2, text=list_num[i], fg_color=("gray70", "gray25"), corner_radius=4)
            label.grid(column=i, row=0, sticky="nsew", pady=(5, 5), padx=(5, 5))

            label2 = ctk.CTkLabel(box2, text="0", fg_color=("gray80", "gray15"), corner_radius=4)
            label2.grid(column=i, row=1, sticky="nsew", pady=(5, 5), padx=(5, 5))
            self.list_info.append(label2)

    def deselect_slave(self, button):

        text = button.cget("text")[4:]

        if button.get() == 0:
            for e in self.total_pack_labels[int(text)]:
                e.grid_forget()
        else:
            for i, e in enumerate(self.total_pack_labels[int(text)]):
                e.grid(column=int(text) + 1, row=i + 1, sticky="nsew", padx=(2, 2), pady=(2, 2))

    def on_off_faking(self, event):

        self.faking = not self.faking

        if self.faking:
            self.tabview.configure(segmented_button_selected_color="orange")
        else:
            self.tabview.configure(segmented_button_selected_color="chartreuse4")

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
        self.bind('<Control-t>', self.on_off_faking)
        self.faking = False

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
        self.clear_values()
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
        ser.flush()

    def switch_event(self):
        global ser
        if self.switch.get() == 0:
            ser.write(bytes("D", 'utf-8'))
            self.clear_values()
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
                    ser.write(bytes("C", 'utf-8'))
                    ser.flush()

                except Exception as e:
                    print(e)
                    self.switch.deselect()
                    self.clear_values()
                    self.get_COM()
                    self.textbox.configure(text="Select a correct Serial Port")

    def update_gui_normal(self):

        try:
            read = False

            byte1 = 0
            byte2 = 0
            byte3 = 0
            byte4 = 0

            mustend = time.time() + 2

            while not read and time.time() < mustend:

                byte1 = byte2
                byte2 = byte3
                byte3 = byte4

                while ser.in_waiting < 1 and time.time() < mustend:
                    pass

                byte4 = ser.read(1)

                if byte1 == b'\xff' and byte2 == b'\xff' and byte3 == b'\xff' and byte4 == b'\xff':
                    read = True

            if time.time() > mustend:
                self.textbox.configure(text="Device not responding")
                self.switch.deselect()
                self.clear_values()
                ser.close()
                return

            while ser.in_waiting < SIZE_PAYLOAD:
                pass

            serial_data = ser.read(SIZE_PAYLOAD).decode('utf-8')

            ser.reset_input_buffer()
            ser.reset_output_buffer()

        except Exception:
            self.get_COM()
            self.textbox.configure(text="USB Disconnected!")
            self.switch.deselect()
            self.update_silent()
            self.clear_values()
            ser.close()
            return 0

        data_dict = parse_serial_data(serial_data)

        for i, slave in enumerate(data_dict["slaves"]):

            if slave["error_count"] == 0:  # healthy

                for j, volt in enumerate(slave["voltages"]):

                    if volt == data_dict["voltages"]["max"]:
                        font = ("sans-serif", 14, "bold")
                        color = "magenta"
                    elif volt == data_dict["voltages"]["min"]:
                        font = ("sans-serif", 14, "bold")
                        color = "cyan"
                    else:
                        font = ("sans-serif", 14, "normal")
                        color = "black"

                    value = round(volt / 10000, 3)
                    self.total_pack_labels[i][j].configure(text=str(value), fg_color=rgb(value, "volt"), text_color=color, font=font)

                for j, temp in enumerate(slave["temps"]):
                    font = ("sans-serif", 14, "normal")
                    color = "black"

                    value = round(temp, 2)
                    self.total_pack_labels[i][j].configure(text=str(value), fg_color=rgb(value, "temp"), text_color=color, font=font)

            elif data_dict["slaves"]["error_count"] > MIN_ERR:  # dead
                for j in range(N_VS + N_TS):
                    self.total_pack_labels[i][j].configure(text="DEAD", fg_color="black", text_color="white", font=("sans-serif", 14, "normal"))

            else:  # error
                for j in range(N_VS + N_TS):
                    self.total_pack_labels[i][j].configure(text="ERR", fg_color="gray", text_color="black", font=("sans-serif", 14, "normal"))

        color = ("magenta", "cyan", "yellow", "white", "white", "white", "white", "white", "white", "white")
        # list_info: ["MAX VOLT", "MIN VOLT", "BALANCING","TOT VOLT", "AVG VOLT", "MAX TEMP", "MIN TEMP", "AVG TEMP", "CURRENT", "TOT POWER"]
        infos = (data_dict["voltages"]["max"], data_dict["voltages"]["min"], data_dict["voltages"]["max"] - data_dict["voltages"]["min"], data_dict["voltages"]["tot"], data_dict["voltages"]["avg"],
                 data_dict["temps"][
            "max"], data_dict["temps"]["min"], data_dict["temps"]["avg"], data_dict["current"])

        for index, value in enumerate(infos):
            self.list_info[index].configure(text=str(round(value, 3)), text_color=color[index])

    def update_silent(self):
        for i in range(N_SLAVES):
            for j in range(N_VS):
                self.total_pack_labels[i][j].configure(text="0", fg_color="gray")
            for j in range(N_VS, N_VS + N_TS):
                self.total_pack_labels[i][j].configure(text="0", fg_color="gray")
        for j in range(len(self.list_info)):
            self.list_info[j].configure(text="0", fg_color="gray")

    def update_bilancing(self):
        self.update_gui_normal()

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

        self.after(UPDATE_FREQ, self.update)

    def clear_values(self):
        for i in range(N_SLAVES):
            font = ("sans-serif", 14, "normal")
            color = "black"
            value = 0
            for j in range(N_VS):
                self.total_pack_labels[i][j].configure(text=str(value), fg_color=("gray80", "gray15"), text_color=color, font=font)
            for j in range(N_VS, N_VS + N_TS):
                self.total_pack_labels[i][j].configure(text=str(value), fg_color=("gray80", "gray15"), text_color=color, font=font)
            # list_info: ["MAX VOLT", "MIN VOLT", "BALANCING","TOT VOLT", "AVG VOLT", "MAX TEMP", "MIN TEMP", "AVG TEMP", "CURRENT", "TOT POWER"]
        for i in range(10):
            self.list_info[i].configure(text=str(0), text_color="black")

    def on_closing(self):
        if self.switch.get() == 1:
            ser.write(bytes("D", 'utf-8'))
            ser.close()
        self.destroy()


def rgb(value, type):
    if type == "temp":
        return rgb_temp(value)
    if type == "volt":
        return rgb_volt(value)


def rgb_temp(value):
    if 0 <= value < 20:
        return "blue"
    if 20 <= value < 50:
        return "green"
    elif 50 <= value < 60:
        return "yellow"
    else:
        return "red"


def rgb_volt(value):
    if 3.5 <= value < 4.1:
        return "green"
    elif 3.3 <= value < 3.5 or 4.1 <= value < 4.2:
        return "yellow"
    else:
        return "red"


def parse_serial_data(serial_data):
    try:
        data_dict = json.loads(serial_data)
        return data_dict
    except json.JSONDecodeError as e:
        print(f"Error decoding JSON: {e}")
        return None


if __name__ == "__main__":
    app = App()
    # app.geometry("1800x800")

    app.protocol("WM_DELETE_WINDOW", app.on_closing)

    app.mainloop()
