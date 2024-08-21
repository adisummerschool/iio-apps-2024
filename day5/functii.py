def functia_alex(): 
    print("Ba ce chestie!")

def sixteen_bit(i, buf):
    sixteen_bit_number = (buf[i+1] << 8) | buf[i]
    return sixteen_bit_number
