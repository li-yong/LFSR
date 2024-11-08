/*
MIT License

Copyright (c) 2017 https://github.com/nmrr

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <iostream>
#include "LFSR.h"
#include <bitset>
#include <vector>

using namespace std;

// This example will calculate the length of the sequence of a 15 bits register with (x0) xor (x1) as feedback

int main(int argc, char **argv)
{

    vector<int> poly_vec = {3, 1}; // X^5+X^3+X+1.

    // LFSR is 32-bits with equation h(x) = x32 + x22 + x2 + x1 + 1
    vector<int> poly_vec_tpg = {22, 2, 1};


    //The ORA is a 16-bit LFSR whose equation is h(x) = x16 + x15 + x13 + x4 + 1.
     vector<int> poly_vec_ora = {15, 13, 4};


    const int d_ff_num = 5;

    // Create a n bits register, by default all bits are set to 0
    LFSR lfsr(d_ff_num);

    string inputS = "01010001";
    // string inputS = "101010101010101010101010100101010";

    std::bitset<sizeof(inputS)> input_bs(inputS);

    // Process the bitmap
    std::cout << "Bitmap: " << input_bs << std::endl;
    // Iterate from right (least significant bit) to left (most significant bit)
    for (int i = 0; i < input_bs.size(); ++i)
    {
        // std::cout << "Bit at position " << i << " (from right to left): " << input_bs[i] << std::endl;
        std::bitset<5> last_op(lfsr.get32bit());
        // std::cout << "last time output: " << output0 << '\n';

        lfsr.rightShift(0); // shift right 1 bit, fill 0 at MSB.
        auto FB = last_op[0]; // the first bit, LSB.

        /*
        X0, X1, X2, X3, X4
        bit[4], bit[3], bit[2], bit[1], bit[0]
        */

        auto x0 = FB ^ input_bs[i]; // MSB.
        lfsr.setBit(4, x0);

        auto x5 = last_op[1]; // LSB
        lfsr.setBit(0, x5);

        // Middle terms in Polynomial.
        // iterate poly_x
        for (int j = 0; j < poly_vec.size(); ++j)
        {
            auto x = FB ^ last_op[d_ff_num - poly_vec[j]]; //xor (lsb, previous_postion_bit_in_last_run)
            lfsr.setBit(d_ff_num-poly_vec[j]-1, x); //setBit(position, value)
        }


        std::bitset<d_ff_num> this_op(lfsr.get32bit());
        std::cout << "loop " << i << ", input " << input_bs[i] << ", output: " << this_op << '\n';
    }

    // Set the first bit to 1

    // LastBit MSB >>> 0001 <<< firstbit, LSB
    lfsr.setFirstBit(true); // LSB, rightmost bit
    lfsr.setBit(1, true);
    // lfsr.setBit(2, false);
    lfsr.setLastBit(true); // MSB, leftmost bit

    uint32_t a = lfsr.get32bit();
    std::bitset<8> x(a);
    std::cout << x << '\n';
    string b = x.to_string();

    // Save the register
    uint32_t *output;
    lfsr.save(output);

    uint64_t counter = 0;

    // Iterate while the output value is not equal to the initial state
    do
    {
        // auto equition = lfsr.getFirstBit() xor lfsr.getBit(1);
        auto equition = lfsr.getFirstBit() xor lfsr.getBit(1) xor lfsr.getBit(4);

        lfsr.rightShift(equition);

        a = lfsr.get32bit();
        bitset<8> x(a);
        std::cout << x << '\n';

        counter++;
    }
    // while(!lfsr.compare(output));
    while (counter < 10);

    cout << "counter = " << counter << endl;

    delete[] output;

    return (0);
}
