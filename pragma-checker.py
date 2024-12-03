import os

def check_and_add_pragma_once(directory):
    """Recursively checks for #pragma once in header files and adds it if missing."""
    header_extensions = {".h", ".hpp", ".hh"}  # Add more extensions if needed

    for root, _, files in os.walk(directory):
        for file in files:
            file_path = os.path.join(root, file)
            file_extension = os.path.splitext(file)[1]

            if file_extension in header_extensions:
                print(f"file path: {file_path}")
                with open(file_path, "r+") as f:
                    lines = f.readlines()

                    # Check if the first line is #pragma once
                    if not lines or lines[0].strip() != "#pragma once":
                        print(f"modifiedfile : {file_path}")
                        print(f"Adding #pragma once to {file_path}")

                        # Add #pragma once to the top
                        f.seek(0)
                        f.write("#pragma once\n")
                        f.writelines(lines)


if __name__ == "__main__":
    check_dirs = ("include", "src", "lib")  # Change this to your include directory path
    for curr_dir in check_dirs:
        if os.path.isdir(curr_dir):
            check_and_add_pragma_once(curr_dir)
        else:
            print(f"Directory '{curr_dir}' does not exist.")
