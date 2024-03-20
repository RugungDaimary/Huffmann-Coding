HUFFMANN CODING :
    1. The provided code is an implementation of Huffman Coding, a data compression algorithm.
    2. Various data structures such as priority queue (minHeap), unordered_map, and binary tree are used in the construction of the Huffman tree and generation of Huffman codes.
    3. The HuffmanCode class includes methods for various tasks such as building a frequency dictionary, constructing the Huffman tree, generating Huffman codes, encoding and padding the input text, and building a byte array. It also includes methods for decompression tasks such as removing padding and decompressing the text.
    4. The main function in the code handles the file to be compressed. It creates an instance of HuffmanCode, compresses the file, and if successful, decompresses it.
    5. Huffman coding is a lossless algorithm that assigns variable-length codes to characters based on their frequencies, with more frequent characters receiving shorter codes.
    6. The codes assigned in Huffman coding are Prefix Codes, ensuring no ambiguity during decoding.
    7. Improper assignment of codes can lead to ambiguity. For instance, if a code assigned to one character is the prefix of codes assigned to other characters, it can result in multiple possible outputs during decompression. Therefore, careful assignment of codes is essential.
