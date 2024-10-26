from ConfigurationMenu import *
from DataFrame import *
from WebSocketController import WebSocketController


class UI(ctk.CTk):

    def __init__(self):
        super().__init__()

        #self.iconbitmap("icon.ico")
        self.title("BMS UIx")
        self.resizable(False, False)
        self.geometry("+20+20")  # top left corner
        self.protocol("WM_DELETE_WINDOW", self._on_closing)
        self.bind('<Control-e>', self.ctrl_e_pressed)
        self.bind('<Control-x>', self.ctrl_x_pressed)

        self.serial_controller = SerialController()
        self.ws_controller = WebSocketController()

        self.menu = ConfigurationMenu(self)
        self.data_frame = DataFrame(self)

        self._initialize_gui()

    def _initialize_gui(self):
        self.menu.grid(row=0, column=0, padx=(10, 0), rowspan=5)
        self.data_frame.grid(row=0, column=1, padx=(10, 10), pady=(20, 20), sticky="nsew", rowspan=2, ipadx=3)

    def _on_closing(self):
        if self.menu.get_switch() == 1:
            self.serial_controller.close()
        self.destroy()

    def ctrl_e_pressed(self, event):
        if self.menu.get_switch() == 1:
            self.menu.controller.set_mode("E")
            self.title("BMS UIe")

        # print("Ctrl+E was pressed!")

    def ctrl_x_pressed(self, event):
        if self.menu.get_switch() == 1:
            self.menu.controller.set_mode("X")
            self.title("BMS UIx")

        # print("Ctrl+X was pressed!")



if __name__ == "__main__":
    app = UI()
    app.mainloop()
