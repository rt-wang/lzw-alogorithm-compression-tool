# LZW Compression Tool

A command-line utility that implements LZW (Lempel-Ziv-Welch) compression algorithm with customizable table sizes and pruning options.

## Features

- Encoding and decoding functionality
- Adjustable dictionary size
- Optional pruning for better compression rates
- Symbolic linking for easy usage

## Installation

Build the project using make:
```bash
make
```

This will create three executables:
- `program` (main executable)
- `encode` (symbolic link)
- `decode` (symbolic link)

## Usage

### Encoding
```bash
./encode [-m bits] [-p] < input_file > output_file
```

Options:
- `-m <bits>`: Set dictionary size (9-12, default: 12)
  - 9: Smallest table size, faster but less compression
  - 12: Largest table size, slower but better compression
- `-p`: Enable pruning for better compression rates (takes longer to process)

### Decoding
```bash
./decode < compressed_file > output_file
```

## Examples

Basic encoding:
```bash
./encode < original.txt > compressed.txt
```

Encoding with maximum compression:
```bash
./encode -m 12 -p < original.txt > compressed.txt
```

Faster encoding with smaller dictionary:
```bash
./encode -m 9 < original.txt > compressed.txt
```

Decoding:
```bash
./decode < compressed.txt > decompressed.txt
```

## Performance Considerations

1. Dictionary Size (-m option):
   - Larger values (12) provide better compression but take longer
   - Smaller values (9) are faster but offer less compression
   - Memory usage increases with larger dictionary sizes

2. Pruning (-p option):
   - Improves compression ratio
   - Significantly increases processing time
   - Recommended for archival purposes where time isn't critical

## Cleaning Up

Remove all compiled files and links:
```bash
make clean
```

## Implementation Details

The program uses the LZW compression algorithm with:
- Dynamic dictionary building
- Variable-width code output
- Optional dictionary pruning
- Efficient bit-packing for output

## Author

Ruotian Wang
