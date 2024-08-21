def functia_florina ():
    print("Hello from functia_florina")

def sixteen_bit_to_int (i, buf):
        sixteen_bit_number = (buf[i + 1] << 8) | buf[i]
        return sixteen_bit_number