from Slave import *
from Constants import *
from SummaryInfo import *
import json
import flatten_json



def parse_serial_data(serial_data) -> dict:
    #print(serial_data)
    return json.loads(serial_data)


class DataFrame(ctk.CTkFrame):

    def __init__(self, ui_frame):
        super().__init__(ui_frame)

        self.slaves = list()
        self._setup_slaves_frame()
        self.ui_frame = ui_frame

        self.summary_info = SummaryInfo(self)
        self.summary_info.grid(row=1, column=0, padx=(10, 10), pady=(20, 5), sticky="nsew", columnspan=N_SLAVES + 1)

        self._update_gui()

    def _setup_slaves_frame(self) -> None:
        index_frame = get_index_frame(self)
        index_frame.grid(column=0, row=0, sticky="nsew")

        for i in range(0, N_SLAVES):
            slave = Slave(self, i)
            slave.grid(column=i + 1, row=0, padx=(5, 5 if i == N_SLAVES - 1 else 0), sticky="nsew")
            self.slaves.append(slave)

    def _get_switch(self) -> int:
        return self.ui_frame.menu.get_switch()

    def _get_mode(self) -> str:
        return self.ui_frame.menu.get_mode()

    def _get_type(self) -> str:
        return self.ui_frame.menu.get()

    def _get_text(self) -> str:
        return self.ui_frame.menu.get_text()

    def _set_text(self, message: str) -> None:
        self.ui_frame.menu.set_text(message)

    def _update_gui(self):

        try:
            switch = self._get_switch()

            if switch == 1 and self._get_mode() == "Normal Mode":
                packet = self.ui_frame.menu.controller.read_packet()
                self._update_logic(packet)

            elif (switch == 1 and self._get_mode() == "Sleep Mode") or switch == -1:  #clear all the slaves
                for i in range(N_SLAVES):
                    self.slaves[i].update_slave({}, 0, 0)
                self.summary_info.update_info({})

        except TimeoutError:
            self.ui_frame.menu.error("Device not responding, retry or select another port")
        except TypeError or json.JSONDecodeError as e:
            print(e)
            self.ui_frame.menu.error("Unable to parse JSON")
        # except json.JSONDecodeError:
        #     self.ui_frame.menu.error("Error Unpacking")  # look carefully the definition of the host's JSON

        self.after(UPDATE_FREQ, self._update_gui)

    def _update_logic(self, packet: str) -> None:
        # First packet is empty
        if packet == "":
            return
        data_dict: dict = parse_serial_data(packet)
        data_dict = flatten_json.unflatten_list(data_dict)
        for i, slave_values in enumerate(data_dict["slaves"]):
            try:
                self.slaves[i].update_slave(slave_values, data_dict["voltages"]["max"], data_dict["voltages"]["min"])
            except Exception as e:
                print(f"In update logic {e}")
                pass

        self.summary_info.update_info(data_dict)


# return the index frame of the slaves table
def get_index_frame(master: DataFrame) -> ctk.CTkFrame:
    index_frame = ctk.CTkFrame(master)
    label = ctk.CTkLabel(index_frame, text="", fg_color="transparent", corner_radius=4, width=52)
    label.grid(column=0, row=0, sticky="nsew", pady=(5, 5), padx=(5, 5))

    for i in range(0, N_VS + N_TS):
        label = ctk.CTkLabel(index_frame, text="Cell " + str(i + 1) if i < N_VS else "Tmp " + str(i - N_VS + 1), fg_color=("gray70", "gray25"), corner_radius=4,
                             width=52)
        label.grid(column=0, row=i + 1, sticky="nsew", pady=(5, 5), padx=(5, 5))

    return index_frame
