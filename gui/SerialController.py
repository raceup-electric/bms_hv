import time
from Constants import *
import serial.tools.list_ports
import json


# static method to get all the available (connected) serial port at that moment
def get_com() -> list:
    return [port.device for port in serial.tools.list_ports.comports()]


class SerialController:
    def __init__(self):
        self.ser = serial.Serial(timeout=2)

    def open(self, baud_rate: int, selected_com: str):
        self.ser.baudrate = baud_rate
        self.ser.port = selected_com
        self.ser.open()

        time.sleep(0.4)
        self.set_mode("C")

    # send the selected mode to the host
    def set_mode(self, mode: str):
        self.ser.write(bytes(mode, FORMAT))
        self.ser.flush()

    def close(self):
        if self.ser.isOpen():
            self.set_mode("D")
            self.ser.close()

    def _check_timeout(self, time_out):
        if time.time() > time_out:
            self.close()
            raise TimeoutError

    def _readline(self) -> bytes:
        time_out = time.time() + 2
        data = self.ser.readline()
        self._check_timeout(time_out)
        return data

    def _read(self, size: int) -> bytes:
        time_out = time.time() + 2

        try:
            while self.ser.in_waiting < size and time.time() < time_out:
                pass
        except serial.SerialException:
            return bytes(size)

        self._check_timeout(time_out)
        return self.ser.read(size)

    # Read the start sequence and only then read the payload sent by the BMS. There's a time out in order to avoid
    # infinite loop on a wrong com
    def read_packet(self) -> str:
        read = False
        time_out = time.time() + 2
        bytes_buffer = bytearray(4)

        while not read and time.time() < time_out:
            bytes_buffer += self._read(1)
            bytes_buffer.pop(0)

            if bytes_buffer[-4:] == b'\xff\xff\xff\xff':
                read = True

        self._check_timeout(time_out)
        resp = self._readline().decode(FORMAT).strip()

        self.ser.reset_input_buffer()
        self.ser.reset_output_buffer()

        return resp
