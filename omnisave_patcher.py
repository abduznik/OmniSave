import tkinter as tk
from tkinter import filedialog, messagebox
import os
import shutil
import sys

class OmniSavePatcher:
    def __init__(self, root):
        self.root = root
        self.root.title("OmniSave Patcher")
        self.root.geometry("500x350")
        self.root.resizable(False, False)

        # Game Executable
        tk.Label(root, text="1. Select Game Executable (.exe):", font=("Helvetica", 10, "bold")).pack(anchor="w", padx=20, pady=(20, 5))
        self.exe_frame = tk.Frame(root)
        self.exe_frame.pack(fill="x", padx=20)
        self.exe_path_var = tk.StringVar()
        tk.Entry(self.exe_frame, textvariable=self.exe_path_var, width=40).pack(side="left", fill="x", expand=True)
        tk.Button(self.exe_frame, text="Browse", command=self.browse_exe).pack(side="left", padx=(10, 0))

        # Save Directory
        tk.Label(root, text="2. Game Save Directory (e.g. %APPDATA%\\GameName):", font=("Helvetica", 10, "bold")).pack(anchor="w", padx=20, pady=(20, 5))
        self.save_frame = tk.Frame(root)
        self.save_frame.pack(fill="x", padx=20)
        self.save_path_var = tk.StringVar()
        tk.Entry(self.save_frame, textvariable=self.save_path_var, width=40).pack(side="left", fill="x", expand=True)

        # Portable Save Path
        tk.Label(root, text="3. Portable USB Save Folder:", font=("Helvetica", 10, "bold")).pack(anchor="w", padx=20, pady=(20, 5))
        self.usb_frame = tk.Frame(root)
        self.usb_frame.pack(fill="x", padx=20)
        self.usb_path_var = tk.StringVar(value="./portable_saves/GameName")
        tk.Entry(self.usb_frame, textvariable=self.usb_path_var, width=40).pack(side="left", fill="x", expand=True)

        # Patch Button
        tk.Button(root, text="Apply OmniSave Patch", font=("Helvetica", 12, "bold"), bg="#4CAF50", fg="black", command=self.apply_patch, pady=10).pack(fill="x", padx=50, pady=30)

    def browse_exe(self):
        filepath = filedialog.askopenfilename(filetypes=[("Executables", "*.exe"), ("All Files", "*.*")])
        if filepath:
            self.exe_path_var.set(filepath)

    def apply_patch(self):
        exe_path = self.exe_path_var.get()
        host_save = self.save_path_var.get()
        portable_save = self.usb_path_var.get()

        if not exe_path or not host_save or not portable_save:
            messagebox.showerror("Error", "Please fill in all fields.")
            return

        if not os.path.exists(exe_path):
            messagebox.showerror("Error", "The selected executable does not exist.")
            return

        game_dir = os.path.dirname(exe_path)
        base_name = os.path.basename(exe_path)
        name_no_ext, ext = os.path.splitext(base_name)
        
        original_exe = os.path.join(game_dir, f"{name_no_ext}_original{ext}")
        
        # Check if we already patched this
        if os.path.exists(original_exe):
            response = messagebox.askyesno("Already Patched", f"{base_name} seems to be already patched. Repatch?")
            if not response:
                return
        else:
            # Rename the original executable
            try:
                os.rename(exe_path, original_exe)
            except Exception as e:
                messagebox.showerror("Error", f"Failed to rename executable: {e}")
                return

        # Assuming the compiled OmniSave binary is named "OmniSave" or "OmniSave.exe" next to this script
        source_omnisave = "OmniSave.exe" if sys.platform == "win32" else "OmniSave"
        if not os.path.exists(source_omnisave):
            messagebox.showerror("Error", f"Could not find '{source_omnisave}' in the current directory. Please ensure it is compiled and placed next to this patcher.")
            # Rollback
            if not os.path.exists(exe_path) and os.path.exists(original_exe):
                os.rename(original_exe, exe_path)
            return

        # Copy OmniSave to take the place of the original executable
        try:
            shutil.copy2(source_omnisave, exe_path)
        except Exception as e:
            messagebox.showerror("Error", f"Failed to copy OmniSave: {e}")
            return

        # Generate omnisave.ini in the game directory
        ini_path = os.path.join(game_dir, "omnisave.ini")
        try:
            with open(ini_path, "w") as f:
                f.write(f"[OmniSave]\n")
                f.write(f"Launch_Command=\"{name_no_ext}_original{ext}\"\n")
                f.write(f"Host_Save_Path_1={host_save}\n")
                f.write(f"Portable_Save_Path_1={portable_save}\n")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to write omnisave.ini: {e}")
            return

        messagebox.showinfo("Success!", f"Successfully patched {base_name}!\n\nWhen Steam or CrossOver launches the game, OmniSave will silently sync the files and then launch {name_no_ext}_original{ext}.")

if __name__ == "__main__":
    root = tk.Tk()
    app = OmniSavePatcher(root)
    root.mainloop()
