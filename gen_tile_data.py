import base64
import struct

struct_str = "<HHHHHH"  # 6个

width = 24
height = 24

start_id = 576

def gen_tile_data():
    id = start_id

    data = bytes()

    for row in range(height):
        for col in range(width):
            data += struct.pack(struct_str, 0, col, row, id, 0, 0)
            id += 1
    b64str = base64.b64encode(data).decode()

    text = f"""tile_map_data = PackedByteArray("{b64str}==")"""
    print(text)

if __name__ == '__main__':
    gen_tile_data()