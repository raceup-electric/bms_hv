import tkinter

from serial import SerialException
import customtkinter as ctk
from UI import UI
from SerialController import *


class ConfigurationMenu(ctk.CTkTabview):

    def __init__(self, ui_frame: UI):
        super().__init__(ui_frame, state="disable", segmented_button_selected_color="chartreuse4", width=220, height=320)
        self.baud_var = tkinter.IntVar(value=115200)
        self.mode = tkinter.StringVar(value="Normal Mode")
        self.type = tkinter.StringVar(value="Serial")
        self.ui = ui_frame
        self.controller = ui_frame.serial_controller
        self._menu_setup()

    def _set_mode(self):
        if self.get_switch() == 0:
            return
        self.controller.set_mode(self.mode.get()[0])

    def _set_type(self):
        if self.get_type() == "Serial":
            self.controller = self.ui.serial_controller
        else:
            self.controller = self.ui.ws_controller

    def get_text(self):
        return self.textbox.cget("text")

    def _menu_setup(self):
        self.add("GENERAL")
        self.tab("GENERAL").grid_columnconfigure(0, weight=1)

        self._general_setup()

        self.add("SERIAL")
        self.tab("SERIAL").grid_columnconfigure(0, weight=1)
        self._serial_setup()

        self.add("WIFI")
        self.tab("WIFI").grid_columnconfigure(0, weight=1)
        self._socket_setup()

    def _general_setup(self):
        self._mode_frame_setup()
        self._type_frame_setup()
        self.switch = ctk.CTkSwitch(self.tab("GENERAL"), text=f"OFF/ON", command=self._switch_event, switch_width=60, switch_height=30)
        self.switch.grid(row=2, column=0, padx=0, pady=(30, 10))

        self.textbox = ctk.CTkLabel(self, height=100, fg_color=("gray70", "gray10"), corner_radius=5, text="Select the settings and press ON")
        self.textbox.grid(row=4, column=0, pady=(10, 0), sticky="nsew")
        self._appearance_frame_setup()

    def _serial_setup(self):
        self._baud_frame_setup()

        self.option_COM = ctk.CTkOptionMenu(self.tab("SERIAL"))
        self._get_com()
        self.option_COM.grid(row=0, column=0, padx=0, pady=(40, 5))

        buttons = (("Refresh Serial", (15, 0)), ("Reset Serial", (5, 15)))
        for button in buttons:
            tk_button = ctk.CTkButton(self.tab("SERIAL"), fg_color="transparent", border_width=2, text_color=("gray10", "#DCE4EE"), text=button[0],
                                      command=None)
            tk_button.grid(row=buttons.index(button) + 4, column=0, pady=button[1], sticky="news")

    def _socket_setup(self):
        pass

    def _baud_frame_setup(self):
        baud_frame = ctk.CTkFrame(self.tab("SERIAL"))
        baud_frame.grid(row=1, column=0, pady=(5, 0), sticky="news")

        available_baud = ((115200, "115200  bit/s", (10, 5)), (9600, "9600  bit/s    ", (5, 10)))
        for baud_tuple in available_baud:
            ctk.CTkRadioButton(baud_frame, variable=self.baud_var, value=baud_tuple[0], text=baud_tuple[1]).grid(row=available_baud.index(baud_tuple), column=0,
                                                                                                                 pady=baud_tuple[2], padx=10)

    def _mode_frame_setup(self):
        mode_frame = ctk.CTkFrame(self.tab("GENERAL"))
        mode_frame.grid(row=0, column=0, pady=(5, 0), sticky="news")

        available_mode = (("Normal Mode     ", (10, 5)), ("Sleep Mode        ", (5, 5)), ("Balancing Mode", (5, 10)))
        for mode in available_mode:
            ctk.CTkRadioButton(mode_frame, variable=self.mode, value=mode[0].strip(), text=mode[0], command=self._set_mode).grid(row=available_mode.index(mode),
                                                                                                                                 column=0, pady=mode[1],
                                                                                                                                 padx=10)

    def _type_frame_setup(self):
        type_frame = ctk.CTkFrame(self.tab("GENERAL"))
        type_frame.grid(row=1, column=0, pady=(5, 0), sticky="news")

        available_type = (("Serial", (10, 5)), ("WebSocket", (5, 5)))
        for comunication_type in available_type:
            ctk.CTkRadioButton(type_frame, variable=self.type, value=comunication_type[0].strip(), text=comunication_type[0], command=self._set_type).grid(
                row=available_type.index(comunication_type), column=0, pady=comunication_type[1], padx=10)

    def _appearance_frame_setup(self):
        appearance_frame = ctk.CTkFrame(self)
        appearance_frame.grid(row=7, column=0, pady=(6, 0), sticky="nsew")
        appearance_frame.grid_columnconfigure(0, weight=1)
        option = ctk.CTkOptionMenu(appearance_frame, values=["System", "Dark", "Light"], command=lambda app_mode: ctk.set_appearance_mode(app_mode))
        option.grid(pady=(8, 8))

    def _get_com(self):
        connected = get_com()
        if len(connected) == 0:
            connected = ["No Device Found"]
            self.switch.deselect()
        self.option_COM.configure(values=connected)
        self.option_COM.set(connected[0])

    def error(self, message: str):
        self.switch.deselect()
        self.set_text(message)
        self.controller.close()

        if self.get_type() == "Serial":
            self._get_com()
        else:
            print("-TO BE IMPLEMENTED-")  # TODO
            print(message)

    def _switch_event(self):
        if self.switch.get() == 0:
            self.controller.close()
            return

        if self.get_type() == "Serial" and self.option_COM.get() == "No Device Found":
            self.error("Select a correct Serial Port")
            return

        try:
            if self.get_type() == "Serial":
                self.controller.open(self.baud_var.get(), self.option_COM.get())
                self.set_text("Selected port is ON and listening")
            else:
                self.controller.open()
                self.set_text("WebSocket is ON and listening")

        except SerialException:
            self.error("Not possible to open that port, please select another one")

    def get_switch(self) -> int:
        return self.switch.get()

    def get_mode(self) -> str:
        return self.mode.get()

    def get_type(self) -> str:
        return self.type.get()

    def set_text(self, message: str) -> None:
        self.textbox.configure(text=message)
