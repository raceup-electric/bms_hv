import asyncio
import websocket
import rel

from Constants import ADDR


def _on_error(ws, error) -> None:
    print(error)


def _on_close(ws, close_status_code, close_msg) -> None:
    print("### closed ###")


def _on_open(ws) -> None:
    print("Opened connection")


class WebSocketController:
    def __init__(self):
        websocket.enableTrace(True)
        self.ws = websocket.WebSocketApp(ADDR, on_message=self._on_message, on_error=_on_error, on_close=_on_close, on_open=_on_open)
        self.last_message: str = ""

    def open(self) -> None:
        # set up the dispatcher for automatically reconnect in case of an error
        self.ws.run_forever(dispatcher=rel, reconnect=5)
        rel.signal(2, rel.abort)  # every two seconds
        rel.dispatch()

    # send the selected mode to the host
    def set_mode(self, mode: str) -> None:
        self.ws.send(mode)

    def close(self) -> None:
        self.set_mode("D")
        self.ws.close()

    def _on_message(self, ws, message):
        self.last_message = message

    def read_packet(self, ws, message) -> str:
        return self.last_message
