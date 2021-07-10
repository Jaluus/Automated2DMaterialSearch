from tkinter import *
from tkinter.messagebox import askyesno, showwarning
import sys


class parameter_picker_class:
    def __init__(self):
        self.MATERIALS = [
            "Graphene",
            "hBN",
            "WS2",
            "WSe2",
        ]

        self.CHIP_THICKNESSES = [
            "80nm",
            "90nm",
            "100nm",
            "110nm",
        ]

        # Keeps track if the user closed the application, cant raise exeption in tkinter
        self.user_closed = False

        self.scan_name = None
        self.scan_user = None
        self.scan_exfoliated_material = None
        self.chip_thickness = None
        self.entropy_threshold = None
        self.size_threshold = None
        self.sigma_threshold = None

        self.__create_parameter_picker()

    def __create_parameter_picker(self):
        """
        This is really scuffed right now, I should be doing it better
        """
        self.parameter_picker = Tk()

        self.parameter_picker.protocol("WM_DELETE_WINDOW", self.on_close)

        self.parameter_picker.title("Flake Finder")
        self.parameter_picker.grid_columnconfigure((0, 1), weight=1)
        self.parameter_picker.resizable(0, 0)

        ### Scan Parameters
        self.Scan_name_input = Entry(
            self.parameter_picker, width=50, borderwidth=5, text=""
        )
        self.Scan_user_input = Entry(
            self.parameter_picker, width=50, borderwidth=5, text=""
        )

        self.chosen_material = StringVar()
        self.chosen_material.set(self.MATERIALS[0])
        self.Scan_exfoliated_material_dropdown = OptionMenu(
            self.parameter_picker, self.chosen_material, *self.MATERIALS
        )

        self.chosen_chip_thicknesses = StringVar()
        self.chosen_chip_thicknesses.set(self.CHIP_THICKNESSES[1])
        self.chip_thickness_dropdown = OptionMenu(
            self.parameter_picker, self.chosen_chip_thicknesses, *self.CHIP_THICKNESSES
        )

        self.Scan_parameters_label = Label(
            self.parameter_picker, text="Scan Parameters"
        )

        self.Scan_name_label = Label(self.parameter_picker, text="Scan name")
        self.Scan_user_label = Label(self.parameter_picker, text="Scan user")
        self.Scan_exfoliated_material_label = Label(
            self.parameter_picker, text="Exfoliated Material"
        )
        self.chip_thickness_label = Label(self.parameter_picker, text="Chip Thickness")

        ### Filter Parameters

        self.Entropy_threshold_input = Entry(
            self.parameter_picker, width=50, borderwidth=5
        )
        self.Entropy_threshold_input.insert(0, "2.4")
        self.Size_threshold_input = Entry(
            self.parameter_picker, width=50, borderwidth=5
        )
        self.Size_threshold_input.insert(0, "200")
        self.Sigma_threshold_input = Entry(
            self.parameter_picker, width=50, borderwidth=5
        )
        self.Sigma_threshold_input.insert(0, "50")

        self.Filter_parameters_label = Label(
            self.parameter_picker, text="Filter Parameters"
        )

        self.Entropy_threshold_label = Label(
            self.parameter_picker, text="Entropy Threshold"
        )
        self.Size_threshold_label = Label(
            self.parameter_picker, text="Size Threshold in µm²"
        )
        self.Sigma_threshold_material_label = Label(
            self.parameter_picker, text="Sigma Threshold"
        )

        ### Start Button

        self.start_button = Button(
            self.parameter_picker,
            text="START SCAN",
            height=4,
            command=self.validate_input,
        )

        ### Layout
        self.Scan_parameters_label.grid(
            row=1, columnspan=2, sticky="ew", padx=10, pady=10
        )

        self.Scan_name_input.grid(row=2, column=1, sticky="ew", padx=10, pady=10)
        self.Scan_user_input.grid(row=3, column=1, sticky="ew", padx=10, pady=10)
        self.Scan_exfoliated_material_dropdown.grid(
            row=4, column=1, sticky="ew", padx=10, pady=10
        )
        self.chip_thickness_dropdown.grid(
            row=5, column=1, sticky="ew", padx=10, pady=10
        )

        self.Filter_parameters_label.grid(
            row=6, columnspan=2, sticky="ew", padx=10, pady=10
        )

        self.Entropy_threshold_input.grid(
            row=7, column=1, sticky="ew", padx=10, pady=10
        )
        self.Size_threshold_input.grid(row=8, column=1, sticky="ew", padx=10, pady=10)
        self.Sigma_threshold_input.grid(row=9, column=1, sticky="ew", padx=10, pady=10)

        self.Scan_name_label.grid(row=2, column=0)
        self.Scan_user_label.grid(row=3, column=0)
        self.Scan_exfoliated_material_label.grid(row=4, column=0)
        self.chip_thickness_label.grid(row=5, column=0)
        self.Entropy_threshold_label.grid(row=7, column=0)
        self.Size_threshold_label.grid(row=8, column=0)
        self.Sigma_threshold_material_label.grid(row=9, column=0)

        self.start_button.grid(row=10, columnspan=2, sticky="ew")

    def on_close(self):
        self.user_closed = True
        self.parameter_picker.destroy()

    def validate_input(self):
        self.scan_name = self.Scan_name_input.get().strip()
        self.scan_user = self.Scan_user_input.get().lower().strip()
        self.scan_exfoliated_material = self.chosen_material.get()
        self.chip_thickness = self.chosen_chip_thicknesses.get()

        self.entropy_threshold = self.Entropy_threshold_input.get()
        self.size_threshold = self.Size_threshold_input.get()
        self.sigma_threshold = self.Sigma_threshold_input.get()

        if self.scan_name == "" or self.scan_user == "":
            showwarning(
                title="Null Error",
                message="You need to specify a Scan User and Scan Name!",
            )
            return

        try:
            self.entropy_threshold = float(self.entropy_threshold)
            self.size_threshold = float(self.size_threshold)
            self.sigma_threshold = float(self.sigma_threshold)
        except ValueError as error:
            showwarning(
                title="Type Error",
                message="The Entropy, Size and Sigma Threshold need to be floating point numbers or integers not strings!",
            )
            return

        text_string = (
            f"Start the Scan with the current Parameters?\n"
            "--- Scan Parameters ---\n"
            f"Current User : {self.scan_user}\n"
            f"Scan Name : {self.scan_name}\n"
            f"Exfoliated Material : {self.scan_exfoliated_material}\n"
            f"Chip Thickness : {self.chip_thickness}\n"
            "\n--- Filter Parameters ---\n"
            f"Entropy Threshold : {self.entropy_threshold}\n"
            f"Size Threshold : {self.size_threshold}\n"
            f"Sigma Threshold : {self.sigma_threshold}\n"
        )

        answer = askyesno("LOL", text_string)

        if answer == 1:
            self.parameter_picker.destroy()
        else:
            return

    def take_input(self):
        """Takes the Input from the User

        Raises:
            RuntimeError: Gets raised when the user closes the Window

        Returns:
            dict: a dict with the user input
        """
        # Displays the GUI,Continues when the User has finished entering his input
        mainloop()

        if self.user_closed:
            raise RuntimeError("Closed by User")

        return {
            "scan_user": self.scan_user,
            "scan_name": self.scan_name,
            "scan_exfoliated_material": self.scan_exfoliated_material,
            "chip_thickness": self.chip_thickness,
            "entropy_threshold": self.entropy_threshold,
            "size_threshold": self.size_threshold,
            "sigma_threshold": self.sigma_threshold,
        }


if __name__ == "__main__":
    mygui = parameter_picker_class()
    try:
        input_dict = mygui.take_input()
    except RuntimeError:
        print("lol")
        sys.exit(0)
