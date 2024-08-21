def functie_matei():
    print("Matei")

def sixteen_bit(i, buffer):
    x = (buffer[i + 1] << 8) | buffer[i]
    return x