def functie_radu_maris():
    print("Radu Maris")

def sixteen_bit(i ,buf):
    sixteen_bit_nr = (buf[i+1] << 8) | buf[i]
    return sixteen_bit_nr
       