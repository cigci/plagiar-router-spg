import os
from pathlib import Path

# === CONFIG ===
SRC_DIRS = ["src", "lib"]        # folders to include
OUTPUT_FILE = "combined_output.md"
COMMENT_HEADER = "// T H I S - C O D E B L O C K : {}\n"

def combine_arduino_files(src_dirs, output_file):
    combined_md = []
    seen_files = set()

    for src_dir in src_dirs:
        src_path = Path(src_dir)
        if not src_path.exists():
            print(f"[!] Skipping missing folder: {src_dir}")
            continue

        # Recursively find all .cpp and .h files
        for file_path in sorted(src_path.rglob("*")):
            if file_path.suffix not in [".cpp", ".h"]:
                continue
            if file_path in seen_files:
                continue
            seen_files.add(file_path)

            # Markdown header
            combined_md.append(f"## `{file_path.name}`\n\n")
            combined_md.append("```cpp\n")
            combined_md.append(COMMENT_HEADER.format(file_path.name))
            combined_md.append("\n")

            # File content
            with open(file_path, "r", encoding="utf-8", errors="ignore") as f:
                combined_md.extend(f.readlines())

            combined_md.append("\n```\n\n")

    # Write Markdown file
    with open(output_file, "w", encoding="utf-8") as out:
        out.writelines(combined_md)

    print(f"[✓] Combined {len(seen_files)} files into {output_file}")

# === MAIN ===
if __name__ == "__main__":
    combine_arduino_files(SRC_DIRS, OUTPUT_FILE)
