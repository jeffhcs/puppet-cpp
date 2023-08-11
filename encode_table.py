import zlib
import base64
import sys

def compress_and_encode(input_filename, output_filename):
    # Read the file in binary mode
    with open(input_filename, 'rb') as f:
        data = f.read()
    
    # Compress the data with zlib
    compressed_data = zlib.compress(data)
    
    # Base64 encode the compressed data
    encoded_data = base64.b64encode(compressed_data)
    
    # Write the base64 encoded data to a txt file
    with open(output_filename, 'wb') as f:
        f.write(encoded_data)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python script_name.py <input_filename> <output_filename>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    compress_and_encode(input_file, output_file)
    print(f"File {input_file} has been compressed and encoded to {output_file}.")

