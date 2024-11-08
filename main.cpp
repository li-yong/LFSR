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
#include <algorithm>

using namespace std;

vector<int> stringToBinaryVector(const string &binaryString, bool revert = true)
{
    vector<int> result;
    for (char ch : binaryString)
    {
        // Convert each character ('0' or '1') to an integer (0 or 1)
        result.push_back(ch - '0');
    }

    if (revert)
    {
        reverse(result.begin(), result.end()); // reverse the vector
    }

    return result;
}

vector<int> intToBinaryVector(int num, int N)
{
    vector<int> binaryRepresentation;

    // Convert to binary representation
    while (num > 0)
    {
        binaryRepresentation.push_back(num % 2); // Get the least significant bit
        num /= 2;                                // Shift right by dividing by 2
    }

    // Pad with 0s to ensure the length is N
    while (binaryRepresentation.size() < N)
    {
        binaryRepresentation.push_back(0);
    }
    // Reverse to get the most significant bit on the left
    std::reverse(binaryRepresentation.begin(), binaryRepresentation.end());

    return binaryRepresentation;
}

void tpg_has_input(LFSR lfsr, vector<int> poly_vec, int d_ff_num, string inputS)
{

    int len = inputS.length();
    vector<int> input_vector = stringToBinaryVector(inputS, true);

    // Iterate from right (least significant bit) to left (most significant bit)
    for (int i = 0; i < inputS.length(); ++i)
    {
        uint32_t get32bit = lfsr.get32bit();

        vector<int> last_op = intToBinaryVector(lfsr.get32bit(), d_ff_num);

        lfsr.rightShift(0);             // shift right 1 bit, fill 0 at MSB.
        int FB = last_op[d_ff_num - 1]; // the first bit, LSB.

        /*
        d-ff: X0, X1, X2, X3, X4
        last_op: [0], 1, 2, 3, [4]
        lsfr: bit[4], bit[3], bit[2], bit[1], bit[0]
        */

        auto x0 = FB ^ input_vector[i]; // MSB.
        lfsr.setBit(d_ff_num - 1, x0);

        lfsr.setBit(0, last_op[d_ff_num - 2]); // LSB

        // Middle terms in Polynomial.
        // iterate poly_x
        for (int j = 0; j < poly_vec.size(); ++j)
        {
            auto x = FB ^ last_op[poly_vec[j] - 1];     // xor (lsb, previous_postion_bit_in_last_run)
            lfsr.setBit(d_ff_num - poly_vec[j] - 1, x); // setBit(position, value)
        }

        // bitset<32> this_op(lfsr.get32bit());
        vector<int> this_op = intToBinaryVector(lfsr.get32bit(), d_ff_num);
        cout << "loop " << i << ", input " << input_vector[i] << ", output: ";

        for (int i = 0; i < this_op.size(); ++i)
        {
            cout << this_op[i];
        }

        cout << '\n';
    }
}

void tpg_has_no_input(LFSR lfsr, vector<int> poly_vec, int d_ff_num)
{

    int len = 100;

    // Iterate from right (least significant bit) to left (most significant bit)
    for (int i = 0; i < len; ++i)
    {
        // constexpr int d_ff_num = 5;

        // cout << "Bit at position " << i << " (from right to left): " << input_bs[i] << endl;
        bitset<32> last_op(lfsr.get32bit());
        // cout << "last time output: " << output0 << '\n';

        lfsr.rightShift(0);   // shift right 1 bit, fill 0 at MSB.
        auto FB = last_op[0]; // the first bit, LSB.

        /*
        X0, X1, X2, X3, X4
        bit[4], bit[3], bit[2], bit[1], bit[0]
        */

        lfsr.setBit(d_ff_num - 1, FB); // MSB.

        lfsr.setBit(0, last_op[1]); // LSB

        // Middle terms in Polynomial.
        // iterate poly_x
        for (int j = 0; j < poly_vec.size(); ++j)
        {
            auto x = FB ^ last_op[d_ff_num - poly_vec[j]]; // xor (lsb, previous_postion_bit_in_last_run)
            lfsr.setBit(d_ff_num - poly_vec[j] - 1, x);    // setBit(position, value)
        }

        bitset<32> this_op(lfsr.get32bit());
        cout << "loop " << i << ", input " << FB << ", output: " << this_op << '\n';
    }
}

// This example will calculate the length of the sequence of a 15 bits register with (x0) xor (x1) as feedback

int main(int argc, char **argv)
{

    vector<int> poly_vec = {3, 1}; // X^5+X^3+X+1.

    // LFSR is 32-bits with equation h(x) = x32 + x22 + x2 + x1 + 1
    vector<int> poly_vec_tpg = {22, 2, 1};

    // The ORA is a 16-bit LFSR whose equation is h(x) = x16 + x15 + x13 + x4 + 1.
    vector<int> poly_vec_ora = {15, 13, 4};

    const int d_ff_num = 5;

    // Create a n bits register, by default all bits are set to 0
    LFSR lfsr(d_ff_num);

    bool has_inputS = false;

    string inputS;

    bool enable_inputS;
    enable_inputS = true;
    // enable_inputS = false;

    if (enable_inputS)
    {
        inputS = "01010001";
        bitset<8> input_bs(inputS);
    }

    string initS;
    initS = "101010101010101010101010100101010";
    initS = "00000";

    // Initialize the register with the input
    // Iterate from right (least significant bit) to left (most significant bit)

    bitset<5> init_bs(initS);

    for (int i = 0; i < init_bs.size(); ++i)
    {
        lfsr.setBit(i, init_bs.test(i));
    }

    if (enable_inputS)
    {
        inputS = "01010001";
        bitset<8> input_bs(inputS);
        tpg_has_input(lfsr, poly_vec, d_ff_num, inputS);
    }
    else
    {

        tpg_has_no_input(lfsr, poly_vec, d_ff_num);
    }

    exit(0);

    // Set the first bit to 1

    // LastBit MSB >>> 0001 <<< firstbit, LSB
    lfsr.setFirstBit(true); // LSB, rightmost bit
    lfsr.setBit(1, true);
    // lfsr.setBit(2, false);
    lfsr.setLastBit(true); // MSB, leftmost bit

    uint32_t a = lfsr.get32bit();
    bitset<8> x(a);
    cout << x << '\n';
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
        cout << x << '\n';

        counter++;
    }
    // while(!lfsr.compare(output));
    while (counter < 10);

    cout << "counter = " << counter << endl;

    delete[] output;

    return (0);
}
