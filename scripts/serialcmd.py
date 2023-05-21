import serial
import crcmod.predefined
# pip install pycryptodomex
from Cryptodome.Cipher import AES
from Cryptodome.Random import get_random_bytes
from Cryptodome.Util.Padding import pad
import tkinter as tk
import tkinter.messagebox as messagebox


# Serial port configuration
serial_port = "COM3"  # Replace with your actual serial port
baud_rate = 115200

# Encryption parameters
key = get_random_bytes(16)  # Generate a random 128-bit encryption key
print(key)

# Create the AES cipher
cipher = AES.new(key, AES.MODE_ECB)

# Create the CRC checksum function
crc_func = crcmod.predefined.mkPredefinedCrcFun("crc-16")

def process_data(ser):
    print("Monitoring Serial")
    if ser.is_open:
        print("COM is open successfully")
        ser.reset_input_buffer()
        try:
            while True:
                line = ser.readline()
                print(line)
                if (line):
                    output = line.decode('utf-8').replace('\r\n', '')
                    print(output)
                    pass
        except KeyboardInterrupt:
            pass
    else:
        print("COM is open failed")
        pass



class EntryFrame(tk.Frame):
    def __init__(self, master=None, **kwargs):
        super().__init__(master, **kwargs)
        self.configure(width=500, height=300)
        self.create_widgets()

    def create_widgets(self):
        # Radio Button Variables
        self.direction_var = tk.StringVar()
        self.direction_var.set("clockwise")  # Default selection

        # Radio Buttons
        clockwise_radio = tk.Radiobutton(
            self,
            text="Clockwise",
            variable=self.direction_var,
            value="clockwise"
        )
        clockwise_radio.grid(row=0, column=0)
        counterclockwise_radio = tk.Radiobutton(
            self,
            text="Counterclockwise",
            variable=self.direction_var,
            value="counterclockwise"
        )
        counterclockwise_radio.grid(row=0, column=1)

        # Number Entry Fields
        RPM_label = tk.Label(self, text="RPM (1-600):")
        RPM_label.grid(row=1, column=0, padx=10, pady=10)

        self.RPM_entry = tk.Entry(self)
        self.RPM_entry.insert(0, "600")
        self.RPM_entry.grid(row=1, column=1, padx=10, pady=10)

       # Num Entry Fields
        step_label = tk.Label(self, text="Steps (0-1000):")
        step_label.grid(row=2, column=0, padx=10, pady=10)

        self.step_entry = tk.Entry(self)
        self.step_entry.insert(0, "20")
        self.step_entry.grid(row=2, column=1, padx=10, pady=10)

        framebutton = tk.Frame(self)
        framebutton.grid(row=3, column=0, columnspan=2)

        # Create a button to send the message
        self.start_button = tk.Button(framebutton, text="Start", width="7", command=lambda: self.send_message("start"))
        self.start_button.pack(side="left", padx=30, pady=20)

        # Create a button to send the message
        self.stop_button = tk.Button(framebutton, text="Stop", width="7", command=lambda: self.send_message("stop"))
        self.stop_button.pack(side="left", padx=30, pady=20)

        # Create a button to send the message
        self.finish_button = tk.Button(framebutton, text="Finish", width="7", command=self.finishcmd)
        self.finish_button.pack(side="left", padx=30, pady=20)
    def send_message(self, message_type):
        if message_type == "start":
            self.start_button.configure(state="disabled")
            self.stop_button.configure(state="normal")
            # Get the message from the input field
            direction = self.direction_var.get()
            rpms = self.RPM_entry.get()
            try:
                rpms_num = int(rpms)
                print(rpms_num)
            except ValueError:
                messagebox.showerror("Error", "RPM integer range 1 to 600")
                self.RPM_entry.delete(0, tk.END)
                return
            if rpms_num > 600:
                messagebox.showerror("Error", "RPM integer range 1 to 600")
                self.RPM_entry.delete(0, tk.END)
                return
            elif rpms_num < 1:
                self.RPM_entry.delete(0, tk.END)
                messagebox.showerror("Error", "RPM integer range 1 to 600")
                return

            steps = self.step_entry.get()
            try:
                steps_num = int(steps)
            except ValueError:
                self.step_entry.delete(0, tk.END)
                messagebox.showerror("Error", "STEP integer range 0 to 1000")
                return
            if steps_num >= 1000:
                self.step_entry.delete(0, tk.END)
                self.step_entry.insert(0, "1000")
                steps = str(1000)

            message = message_type + ':' + '(' + "dir=" + direction + ',' \
                                         + "rpm=" + rpms + ',' \
                                         + "step=" + steps + ')'
            print(message)

        elif message_type == "stop":
            self.start_button.configure(state="normal")
            self.stop_button.configure(state="disabled")
            message = message_type + ':' + "(ok)"
            print(message)

        # Pad the message to match the block size
        padded_message = pad(message.encode(), AES.block_size)

        # Encrypt the message
        encrypted_message = cipher.encrypt(padded_message)

        # Calculate the CRC checksum of the message
        crc_checksum = crc_func(message.encode())

        # Create the RS232 packet
        # rs232_packet = key + message.encode('utf-8') + crc_checksum.to_bytes(2, byteorder='big') + endline
        rs232_packet = (message + "\r\n").encode('utf-8')

        print(rs232_packet)

        # Open the serial port
        try:
            ser = serial.Serial(serial_port, baud_rate, timeout=0.1)
        except:
            messagebox.showerror("ERROR", "Can't open COM3")
            self.start_button.configure(state="normal")
            self.stop_button.configure(state="normal")

        # Send the RS232 packet
        ser.write(rs232_packet)


        # process_data(ser)

        # Close the serial port
        ser.close()

    def finishcmd(self):
        self.start_button.configure(state="normal")
        self.stop_button.configure(state="normal")

if __name__ == '__main__':
    # Create the main window
    window = tk.Tk()
    window.geometry("500x200")
    window.title("Transmitter")

    # Create an instance of the EntryFrame class
    entry_frame = EntryFrame(window)
    entry_frame.configure()
    entry_frame.pack()


    # Start the Tkinter event loop
    window.mainloop()

