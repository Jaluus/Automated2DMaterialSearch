from tkinter import *
from tkinter import filedialog
from tkinter.messagebox import askyesno, showwarning
import sys


class parameter_picker_class:
    def __init__(self):
        self.MAGNIFICATIONS = [
            20,
            50,
        ]

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

        self.image_dir = None
        self.serverURL = "http://134.61.6.112:5000/upload"
        self.scan_name = None
        self.scan_user = None
        self.scan_exfoliated_material = self.MATERIALS[0]
        self.scan_exfoliation_method = "unspecified"
        self.scan_magnification = self.MAGNIFICATIONS[1]
        self.chip_thickness = self.CHIP_THICKNESSES[1]
        self.entropy_threshold = "2.4"
        self.size_threshold = "200"
        self.sigma_threshold = "50"

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
        self.chosen_material.set(self.scan_exfoliated_material)
        self.Scan_exfoliated_material_dropdown = OptionMenu(
            self.parameter_picker,
            self.chosen_material,
            *self.MATERIALS,
        )

        self.chosen_chip_thicknesses = StringVar()
        self.chosen_chip_thicknesses.set(self.chip_thickness)
        self.chip_thickness_dropdown = OptionMenu(
            self.parameter_picker,
            self.chosen_chip_thicknesses,
            *self.CHIP_THICKNESSES,
        )

        self.chosen_scan_magnification = StringVar()
        self.chosen_scan_magnification.set(self.scan_magnification)
        self.scan_magnification_dropdown = OptionMenu(
            self.parameter_picker,
            self.chosen_scan_magnification,
            *self.MAGNIFICATIONS,
        )

        self.Scan_parameters_label = Label(
            self.parameter_picker, text="--- Scan Parameters ---"
        )

        self.Scan_name_label = Label(self.parameter_picker, text="Scan name")
        self.Scan_user_label = Label(self.parameter_picker, text="Scan user")
        self.Scan_magnification_label = Label(
            self.parameter_picker, text="Scan magnification"
        )
        self.Scan_exfoliated_material_label = Label(
            self.parameter_picker, text="Exfoliated Material"
        )
        self.chip_thickness_label = Label(self.parameter_picker, text="Chip Thickness")

        ### Filter Parameters

        self.Entropy_threshold_input = Entry(
            self.parameter_picker, width=50, borderwidth=5
        )
        self.Entropy_threshold_input.insert(0, self.entropy_threshold)
        self.Size_threshold_input = Entry(
            self.parameter_picker, width=50, borderwidth=5
        )
        self.Size_threshold_input.insert(0, self.size_threshold)
        self.Sigma_threshold_input = Entry(
            self.parameter_picker, width=50, borderwidth=5
        )
        self.Sigma_threshold_input.insert(0, self.sigma_threshold)

        self.Filter_parameters_label = Label(
            self.parameter_picker, text="--- Filter Parameters ---"
        )

        self.Entropy_threshold_label = Label(
            self.parameter_picker, text="Entropy Threshold"
        )
        self.Size_threshold_label = Label(
            self.parameter_picker, text="Size Threshold in ??m??"
        )
        self.Sigma_threshold_material_label = Label(
            self.parameter_picker, text="Sigma Threshold"
        )

        ### Etc Params

        self.etc_parameters_label = Label(
            self.parameter_picker, text="--- etc Parameters ---"
        )

        self.Directory_label = Label(self.parameter_picker, text="File Directory")
        self.Directory_button = Button(
            self.parameter_picker,
            text="Choose Directory",
            command=self.choose_directory,
        )

        self.ServerURL_label = Label(self.parameter_picker, text="Server URL")
        self.ServerURL_input = Entry(self.parameter_picker, width=50, borderwidth=5)
        self.ServerURL_input.insert(0, self.serverURL)

        self.Exfoliation_method_label = Label(
            self.parameter_picker, text="Exfoliation Method"
        )
        self.Exfoliation_method_input = Entry(
            self.parameter_picker, width=50, borderwidth=5
        )
        self.Exfoliation_method_input.insert(0, self.scan_exfoliation_method)

        ### Start Button

        self.start_button = Button(
            self.parameter_picker,
            text="START SCAN",
            height=4,
            command=self.validate_input,
        )

        ### Layout
        # i is a counter for the grid layout
        i = 1

        self.Scan_parameters_label.grid(
            row=i, columnspan=2, sticky="ew", padx=10, pady=10
        )
        i += 1

        self.Scan_name_label.grid(row=i, column=0, padx=10)
        self.Scan_name_input.grid(row=i, column=1, sticky="ew", padx=10)
        i += 1

        self.Scan_user_label.grid(row=i, column=0, padx=10)
        self.Scan_user_input.grid(row=i, column=1, sticky="ew", padx=10)
        i += 1

        self.Scan_exfoliated_material_label.grid(row=i, column=0)
        self.Scan_exfoliated_material_dropdown.grid(
            row=i, column=1, sticky="ew", padx=10
        )
        i += 1

        self.chip_thickness_label.grid(row=i, column=0, padx=10)
        self.chip_thickness_dropdown.grid(row=i, column=1, sticky="ew", padx=10)
        i += 1

        self.Scan_magnification_label.grid(row=i, column=0, padx=10)
        self.scan_magnification_dropdown.grid(row=i, column=1, sticky="ew", padx=10)
        i += 1

        self.Filter_parameters_label.grid(
            row=i, columnspan=2, sticky="ew", padx=10, pady=10
        )
        i += 1

        self.Entropy_threshold_label.grid(row=i, column=0, padx=10)
        self.Entropy_threshold_input.grid(row=i, column=1, sticky="ew", padx=10)
        i += 1

        self.Size_threshold_label.grid(row=i, column=0, padx=10)
        self.Size_threshold_input.grid(row=i, column=1, sticky="ew", padx=10)
        i += 1

        self.Sigma_threshold_material_label.grid(row=i, column=0, padx=10)
        self.Sigma_threshold_input.grid(row=i, column=1, sticky="ew", padx=10)
        i += 1

        self.etc_parameters_label.grid(
            row=i, columnspan=2, sticky="ew", padx=10, pady=10
        )
        i += 1

        self.Directory_label.grid(row=i, column=0, padx=10)
        self.Directory_button.grid(row=i, column=1, sticky="ew", padx=10)
        i += 1

        self.ServerURL_label.grid(row=i, column=0, padx=10)
        self.ServerURL_input.grid(row=i, column=1, padx=10)
        i += 1

        self.Exfoliation_method_label.grid(row=i, column=0, padx=10)
        self.Exfoliation_method_input.grid(row=i, column=1, padx=10)
        i += 1

        self.start_button.grid(row=i, columnspan=2, sticky="ew", padx=10, pady=10)

    def on_close(self):
        self.user_closed = True
        self.parameter_picker.destroy()

    def choose_directory(self):
        self.image_dir = filedialog.askdirectory()

    def validate_input(self):
        self.scan_name = self.Scan_name_input.get().strip()
        self.scan_user = self.Scan_user_input.get().lower().strip()
        self.scan_exfoliated_material = self.chosen_material.get()
        self.chip_thickness = self.chosen_chip_thicknesses.get()
        self.scan_magnification = self.chosen_scan_magnification.get()

        self.entropy_threshold = self.Entropy_threshold_input.get()
        self.size_threshold = self.Size_threshold_input.get()
        self.sigma_threshold = self.Sigma_threshold_input.get()

        self.scan_exfoliation_method = (
            self.Exfoliation_method_input.get().strip().lower()
        )

        self.serverURL = self.ServerURL_input.get()

        if self.scan_name == "" or self.scan_user == "":
            showwarning(
                title="Null Error",
                message="You need to specify a Scan User and Scan Name!",
            )
            return

        if self.image_dir is None or self.image_dir == "":
            showwarning(
                title="Null Error",
                message="You need to pick an Image Directory where to save the Scan!",
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
            f"Start the Scan with the following Parameters?\n"
            "\n--- Scan Parameters ---\n"
            f"Current User : {self.scan_user}\n"
            f"Scan Name : {self.scan_name}\n"
            f"Scan Magnification : {self.scan_magnification}\n"
            f"Exfoliated Material : {self.scan_exfoliated_material}\n"
            f"Chip Thickness : {self.chip_thickness}\n"
            "\n--- Filter Parameters ---\n"
            f"Entropy Threshold : {self.entropy_threshold}\n"
            f"Size Threshold : {self.size_threshold}\n"
            f"Sigma Threshold : {self.sigma_threshold}\n"
            "\n--- Etc Parameters ---\n"
            f"Image Directory : {self.image_dir}\n"
            f"Server URL : {self.serverURL}\n"
            f"Exfoliation Method : {self.scan_exfoliation_method}\n"
        )

        answer = askyesno("Validation", text_string)

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

            Keys:
            - scan_user
            - scan_name
            - scan_exfoliated_material
            - scan_exfoliation_method
            - scan_magnification
            - chip_thickness
            - entropy_threshold
            - size_threshold
            - sigma_threshold
            - image_directory
            - server_url
        """
        # Displays the GUI,Continues when the User has finished entering his input
        mainloop()

        if self.user_closed:
            raise RuntimeError("Closed by User")

        return {
            "image_directory": self.image_dir,
            "server_url": self.serverURL,
            "scan_user": self.scan_user,
            "scan_name": self.scan_name,
            "scan_exfoliated_material": self.scan_exfoliated_material,
            "scan_exfoliation_method": self.scan_exfoliation_method,
            "chip_thickness": self.chip_thickness,
            "entropy_threshold": self.entropy_threshold,
            "size_threshold": self.size_threshold,
            "sigma_threshold": self.sigma_threshold,
            "scan_magnification": int(self.scan_magnification),
        }


if __name__ == "__main__":
    mygui = parameter_picker_class()
    try:
        input_dict = mygui.take_input()
    except RuntimeError:
        sys.exit(0)

    for item in input_dict.items():
        print(item)
