from ConfigurationMenu import *
from DataFrame import *


class UI(ctk.CTk):

    def __init__(self):
        super().__init__()

        self.iconbitmap("icon.ico")
        self.title("BMS UI")
        self.resizable(False, False)
        self.geometry("+20+20")  # top left corner
        self.protocol("WM_DELETE_WINDOW", self._on_closing)

        self.serial_controller = SerialController()
        self.menu = ConfigurationMenu(self, self.serial_controller)
        self.data_frame = DataFrame(self)

        self._initialize_gui()

    def _initialize_gui(self):
        self.menu.grid(row=0, column=0, padx=(20, 20), rowspan=5)
        self.data_frame.grid(row=0, column=1, padx=(10, 10), pady=(20, 20), sticky="nsew", rowspan=2, ipadx=3)

    def _on_closing(self):
        if self.menu.get_switch() == 1:
            self.serial_controller.close()
        self.destroy()


if __name__ == "__main__":
    app = UI()
    app.mainloop()
