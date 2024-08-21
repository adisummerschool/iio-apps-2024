def sixteen_bit(i, bytes_read):
    sixteen_bits_number = (bytes_read[i + 1] << 8) | bytes_read[i]
    return sixteen_bits_number