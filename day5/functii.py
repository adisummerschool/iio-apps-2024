def functia_roby():
    print("Roby")

def sixteen_bit(i, buf):
    sixteen_bit_number = (buf[i+1] << 8) | buf[i]
    return sixteen_bit_number