ENGINEERING_UNITS = {
    "OHM": "\u03A9",
    "AMP": "A",
    "VOLT": "V",
}

ENGINEERING_NOTATIONS = {
    12: "T",
    9: "G",
    6: "M",
    3: "k",
    0: "",
    -3: "m",
    -6: "\u03BC",
    -9: "n",
    -12: "p"
}

def get_engineering_notation(value, unit):
    multiplications = 0
    while int(value) <= 0 or multiplications % 3 != 0:
        value *= 10
        multiplications -= 1
    while int(value) >= 1000 or multiplications % 3 != 0:
        value /= 10
        multiplications += 1
    return f"{value} {ENGINEERING_NOTATIONS[multiplications]}{ENGINEERING_UNITS[unit]}"
