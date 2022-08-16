#!/usr/bin/python3

import argparse
import functools
import re

BITMAP_PATTERN = re.compile(r'([.*@]+)')
HEX_PATTERN = re.compile(r'0x([0-9a-f]+)')

def compile(src: str, width: int, height: int) -> bytes:
    src = src.lstrip()
    result = []
    current_codepoint = None
    row_count = None
    for line in src.splitlines():
        m = BITMAP_PATTERN.match(line)
        n = HEX_PATTERN.match(line)
        if m:
            row_count += 1
            bits = [(0 if x == '.' else 1) for x in m.group(1)]
            if len(bits) != width:
                raise Exception(
                    "Row {row_count} of codepoint 0x{codepoint:04x} has width {actual_width} (width {expected_width} is expected)"
                    .format(
                        row_count=row_count, 
                        codepoint=current_codepoint, 
                        expected_width=width, 
                        actual_width=len(bits)
                    )
                )
            bits_int = functools.reduce(lambda a, b: 2*a + b, bits)
            byte_width = -(-width // 8) # divides by 8, but rounds up
            result.append(bits_int.to_bytes(byte_width, byteorder='little'))
        elif n:
            if row_count != None and row_count != height:
                raise Exception(
                    "The glyph for codepoint 0x{codepoint:04x} has {actual_height} rows ({expected_height} rows expected)."
                    .format(
                        codepoint=current_codepoint, 
                        expected_height=height, 
                        actual_height=row_count
                    )
                )
            row_count = 0
            current_codepoint = int(line, 0) # `print(int('0x10', 0))` gives 16
            result.append(current_codepoint.to_bytes(4, byteorder='little'))
        else:
            continue
    return b''.join(result)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('font', help='path to a font file')
    parser.add_argument('--width', help='width of the character, in pixel')
    # not yet explicitly used, but should validate the input to check that it really holds
    parser.add_argument('--height', help='height of the character, in pixel') 
    parser.add_argument('-o', help='path to an output file', default='font.out')
    ns = parser.parse_args()

    with open(ns.o, 'wb') as out, open(ns.font) as font:
        src = font.read()
        out.write(compile(src, int(ns.width), int(ns.height)))


if __name__ == '__main__':
    main()
