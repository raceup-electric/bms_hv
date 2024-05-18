import threading
import time

import websocket
from Constants import *

import json

def _on_error(wsapp, error) -> None:
    print("- ERROR -" + error)


def _on_close(ws, close_status_code, close_msg) -> None:
    print("- HOST DISCONNECTED -")


# def _on_open(ws) -> None:
#     print("Opened connection")

class WebSocketController:
    v_max = 9000
    v_min = 0

    t_max = 255
    t_min = 0

    def __init__(self):
        # Verbose connetion output for debug
        websocket.enableTrace(False)

        # Set default timeout before an exception is thrown
        websocket.setdefaulttimeout(5)

        self.ws = None
        self.last_message: str = ""

    def open(self) -> None:
        self.ws = websocket.WebSocketApp(SERVER, on_message=self._on_message, on_error=_on_error, on_close=_on_close)
        self.ws.keep_running = True
        wst = threading.Thread(target=self.ws.run_forever)
        wst.start()
        time.sleep(0.2)

    # send the selected mode to the host
    def set_mode(self, mode: str) -> None:
        print("- SETTING MODE -")
        self.ws.send(mode)

    def close(self) -> None:
        print("- CLOSING -")
        self.set_mode("D")
        self.ws.close()

    def _on_message(self, ws, message):
        # parsed_packet = json.loads(message)

        # v_tot = 0
        # t_tot = 0

        # ws_bms = {}
        # ws_bms["slaves"] = []
        # ws_bms["voltages"] = {}
        # ws_bms["temps"] = {}
        # ws_bms["current"] = float(parsed_packet["lem"])

        # for slave_number in range(N_SLAVES): # slave num
        #     slave = {}
            
        #     slave["voltages"] = []
        #     slave["temps"] = []
        #     slave["errorCount"] = 0 # TODO: implementre gli error

        #     for cell_number in range(N_VS): # cell num
        #         temp_volt = float(parsed_packet["cell_" + str(slave_number) + "_" + str(cell_number)])
        #         v_tot += temp_volt

        #         slave["voltages"].append(temp_volt)
        #         self.v_max = temp_volt if temp_volt > self.v_max else self.v_max
        #         self.v_min = temp_volt if temp_volt < self.v_min else self.v_min

        #     for temp_number in range(N_TS): # temp num
        #         temp_temp = float(parsed_packet["temp_" + str(slave_number) + "_" + str(temp_number)])
        #         t_tot += temp_temp

        #         slave["temps"].append(temp_temp)
        #         self.t_max = temp_temp if temp_temp > self.t_max else self.t_max
        #         self.t_min = temp_temp if temp_temp < self.t_min else self.t_min

        #     ws_bms["slaves"].append(slave)
            
        # ws_bms["voltages"]["max"] = self.v_max
        # ws_bms["voltages"]["min"] = self.v_min
        # ws_bms["voltages"]["tot"] = v_tot
        # ws_bms["voltages"]["avg"] = v_tot/N_SLAVES

        # ws_bms["temps"]["max"] = self.t_max
        # ws_bms["temps"]["min"] = self.t_min
        # ws_bms["temps"]["avg"] = t_tot/N_SLAVES

        self.last_message = message

    def read_packet(self) -> str:
        return self.last_message
