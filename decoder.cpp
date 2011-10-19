#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <iterator>

using namespace std;

typedef map<string, char> HuffmanDecode;


void parse_input_file(HuffmanDecode& code_table, string& msg, istream& input) {

    // leer tabla de simbolos
    while (1) {
        char c, dash;
        string codeword;

        input.get(c);
        input.get(dash);
        getline(input, codeword);

        // terminar cuando no haya 0s ni 1s
        if (codeword.find_first_of("01") == string::npos)
            break;

        code_table[codeword] = c;
    }

    // leer largo de la cadena de bits
    unsigned int nr_bits;
    input >> nr_bits;

    // saltarse el resto de la linea actual
    // y la linea con el delimitador
    string _;
    getline(input, _);
    getline(input, _);

    // limpiar string y reservar memoria,
    // aprovechando que sabemos el largo del mensaje
    msg.reserve(nr_bits);
    msg.erase();

    char c;
    while (input >> c) {
        int i;
        for (i = 0; i < 8; ++i) {
            if ((c >> (7 - i)) & 1)
                msg.append("1");
            else
                msg.append("0");
            if (--nr_bits == 0)
                break;
        }
    }
}


void decode_msg(HuffmanDecode& code, const string& msg, ostream& output) {
    // encontrar el largo maximo de un codeword
    HuffmanDecode::iterator i;
    unsigned int max_prefix_length = 0;
    for (i = code.begin(); i != code.end(); ++i) {
        unsigned int current_length = i->first.size();
        if (current_length > max_prefix_length)
            max_prefix_length = current_length;
    }

    // decodificar
    unsigned int start = 0;
    while (start < msg.size()) {
        unsigned int length;
        bool found(false);
        for (length = 1; length <= max_prefix_length; ++length) {
            string prefix(msg.substr(start, length));
            //cout << "Buscando el prefijo " << prefix << endl;
            if (code.find(prefix) != code.end()) {
                //cout << "    encontrado: " << code[prefix] << endl;
                output << code[prefix];
                start += prefix.size();
                found = true;
                break;
            }
        }
        if (!found) {
            cerr << "No se encontro un prefijo apropiado" << endl;
            break;
        }
    }
}


int main(int argc, char *argv[]) {
    HuffmanDecode code_table;
    string encoded_msg;
    ifstream input("out.txt");
    ofstream output("out.txt.decoded");

    parse_input_file(code_table, encoded_msg, input);
    decode_msg(code_table, encoded_msg, output);

    //cout << encoded_msg << endl;
}
