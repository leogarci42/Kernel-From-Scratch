import subprocess
import sys

def generate_symtab(elf_path, output_path):
    # run nm
    result = subprocess.check_output(
        ["nm", "-n", elf_path],
        text=True
    )

    symbols = []

    for line in result.splitlines():
        parts = line.split()

        if len(parts) < 3:
            continue

        addr, type_, name = parts

        if type_ != "T" and type_ != "t":
            continue

        symbols.append((addr, name))

    with open(output_path, "w") as f:
        f.write("#include <stdint.h>\n")
        f.write("#include \"symtab.h\"\n\n")

        f.write("symbol_t symtab[] = {\n")

        for addr, name in symbols:
            f.write(f'    {{0x{addr}, "{name}"}},\n')

        f.write("};\n\n")
        f.write(f"size_t symtab_size = {len(symbols)};\n")

if __name__ == "__main__":
    elf_path = sys.argv[1] if len(sys.argv) > 1 else "kfs"
    output_path = sys.argv[2] if len(sys.argv) > 2 else "kernel-srcs/helpers/symtab.c"
    generate_symtab(elf_path, output_path)