import os
import re

folder = "/Users/phillipb/Documents/macACE/dataSources/font/menuFont"

def natural_key(filename):
    return int(re.search(r'\d+', filename).group())

files = sorted(
    [f for f in os.listdir(folder) if f.endswith(".png")],
    key=natural_key
)
offset = 1

charset = (
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    " !?@#$%^&*()_-+=[]{|}/~:;\"'<>,."
)

for i, filename in enumerate(files):
    if i >= len(charset):
        break

    char = charset[i]
    ascii_code = ord(char)

    old_path = os.path.join(folder, filename)
    new_path = os.path.join(folder, f"{ascii_code}.png")

    print(f"{char} : {ascii_code} : {filename}")

    os.rename(old_path, new_path)

print("Done.")