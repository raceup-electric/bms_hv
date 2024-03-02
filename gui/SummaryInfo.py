import customtkinter as ctk


class SummaryInfo(ctk.CTkFrame):

    def __init__(self, ui_frame):
        super().__init__(ui_frame)
        self.list_info = list()
        self._information_frame_setup()

    def _information_frame_setup(self):
        list_num = ["MAX VOLT", "MIN VOLT", "BALANCING", "TOT VOLT", "AVG VOLT", "MAX TEMP", "MIN TEMP", "AVG TEMP", "CURRENT", "TOT POWER"]

        for i in range(len(list_num)):
            self.columnconfigure(i, weight=1)
            info_name = ctk.CTkLabel(self, text=list_num[i], fg_color=("gray70", "gray25"), corner_radius=4)
            info_name.grid(column=i, row=0, sticky="nsew", pady=(5, 5), padx=(5, 5))

            info_value = ctk.CTkLabel(self, text="0", fg_color=("gray80", "gray15"), corner_radius=4)
            info_value.grid(column=i, row=1, sticky="nsew", pady=(5, 5), padx=(5, 5))
            self.list_info.append(info_value)

    def update_info(self, data_dict: dict) -> None:

        color = ("magenta", "cyan", "yellow", "white", "white", "white", "white", "white", "white", "white")

        if len(data_dict) == 0:
            infos = [0] * len(self.list_info)
        else:
            infos = (
                data_dict["voltages"]["max"], data_dict["voltages"]["min"], data_dict["voltages"]["max"] - data_dict["voltages"]["min"], data_dict["voltages"]["tot"], data_dict["voltages"]["avg"],
                data_dict["temps"]["max"], data_dict["temps"]["min"], data_dict["temps"]["avg"], data_dict["current"])

        for index, value in enumerate(infos):
            self.list_info[index].configure(text=str(round(value, 3)), text_color=color[index])
