from notations import get_engineering_notation         # Helper library


def get_ht_from_points(P1, P2, I):
    m = (P1[1] - P2[1]) / (P1[0] - P2[0])       # dy / dx
    n = P1[1] - (P1[0] * m)                     # P1[1] = m * P1[0] + n
    return m * I + n                            # f(x) = m*x


Ic = 20*10**-3                                                      # Peak Current
hT = get_ht_from_points((0.1*10**-3, 35), (150*10**-3, 100), Ic)    # DC Current Gain of the Transistor (Datasheet)
print(hT)
Vcc = 5                                                             # Supply Voltage
Vbe = 0.7                                                           # Votage drop of a typical Resistor (0.7V otherwise look up in Datasheet)
Rbdrop = 10                                                         # Voltage drop(%) which the Baseresistor should do (should be 10-15%)
Rload = 50                                                          # Load resistance

Rc = Vcc/(Ic * 2)

Ib = Ic / hT

Ie = Ic + Ib
Re = (Vcc * Rbdrop) / (Ie * 100)    # * 100 is used for Voltage drop in %
Ve = Ie * Re


R1 = (Vcc - Vbe - Ve) / (Ib * 10)
R2 = (Vbe + Ve) / (Ib * 10)

Vc = Vcc - (Rc * Ic)

print("Rc:", get_engineering_notation(Rc, "OHM"))
print("Re:", get_engineering_notation(Re, "OHM"))
print("R1:", get_engineering_notation(R1, "OHM"))
print("R2:", get_engineering_notation(R2, "OHM"))
print("Ib:", get_engineering_notation(Ib, "AMP"))

print("Vc:", get_engineering_notation(Vc, "VOLT"))
print("Ve:", get_engineering_notation(Ve, "VOLT"))
print("Vb:", get_engineering_notation((R2 / (R1 + R2)) * Vcc, "VOLT"))

