import socket
import asyncio
import websockets

from Constants import ADDR


class WebSocketController:
    def __init__(self):
        pass

    def open(self):
        pass

    # send the selected mode to the host
    def set_mode(self, mode: str):
        pass

    def close(self):
        pass

    def read_packet(self) -> str:
        pass

        # return resp.decode(FORMAT)
