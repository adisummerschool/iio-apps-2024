def fun():
    print("This is an external function")

def sixteen_bit_i(i, buf):
    sixteen_bit_number = (buf[i+1] << 8) | buf[i]
    return sixteen_bit_number