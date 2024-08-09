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
        self.last_message = message

    def read_packet(self) -> str:
        return self.last_message
