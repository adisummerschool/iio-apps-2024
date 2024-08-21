def skibidi_func():
    print("Skibidi toilet")
    
def two_bytes(i, buf):
    x = (buf[i+1] << 8 ) | buf[i]
    return x