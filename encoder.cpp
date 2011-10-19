#include <iostream>
#include <map>
#include <queue>
#include <sstream>
#include <fstream>
#include <iterator>
#include <algorithm>

using namespace std;

typedef map<char, unsigned int> SymbolCount;
typedef map<char, string> HuffmanCode;

// nodo de un arbol de Huffman
struct Node {
    char symbol;
    unsigned int frequency;
    Node *left_child;
    Node *right_child;

    // constructor para nodos hoja
    Node(char s, unsigned int f) {
        symbol = s;
        frequency = f;
        left_child = NULL;
        right_child = NULL;
    }

    // constructor para nodos intermedios
    Node(Node* l, Node* r) {
        symbol = 0;
        frequency = l->frequency + r->frequency;
        left_child = l;
        right_child = r;
    }

    // un nodo es hoja cuando no tiene hijos
    bool is_leaf() const {
        return left_child == NULL && right_child == NULL;
    }

    // comparacion util para ordenar nodos por frecuencia
    bool operator <(Node &other) {
#if 0
        // con esto se obtiene una codificacion
        // casi igual a la del ejemplo
        if ((*this).frequency == other.frequency) {
            if ((*this).is_leaf() && !other.is_leaf())
                return true;
            if (!(*this).is_leaf() && other.is_leaf())
                return false;
        }
#endif
        return (*this).frequency > other.frequency;
    }
};


// funcion auxiliar para comparar nodos convenientemente a traves de punteros
// (http://www-staff.it.uts.edu.au/~ypisan/programming/stl50.html, tip 20)
struct DereferenceLess {
    template <typename T>
    bool operator ()(T& p1, T& p2) const {
        return *p1 < *p2;
    }
};


// actualizar tabla de frecuencias con los caracteres leidos desde s
void count_symbols(SymbolCount& freq_table, istream& s) {
    while (1) {
        char c(s.get());
        if (s.eof())
            break;
        freq_table[c]++;
    }
}


void print_huffman_tree(Node *node, unsigned int depth = 0) {
    if (node == NULL)
        return;
    string indent(4 * depth, ' ');
    if (node->is_leaf())
        cout << indent << node->symbol << " (" << node->frequency << ")" << endl;
    else
        cout << indent << '*' << " (" << node->frequency << ")" << endl;
    print_huffman_tree(node->left_child,  depth + 1);
    print_huffman_tree(node->right_child, depth + 1);
}




// crear arbol de huffman y retornar puntero a la raiz;
// obs:  los nodos hoja residen en el vector referenciado en el argumento,
//       (posiblemente en memoria de stack), mientras los nodos intermedios
//       son creados en el heap.
Node *create_huffman_tree(vector<Node>& leaves) {
    // cola de punteros a nodos, ordenada por frecuencia
    typedef priority_queue<Node *, vector<Node *>, DereferenceLess> NodePtrQueue;
    NodePtrQueue nodes;

    // inicializar cola con los nodos hoja disponibles
    vector<Node>::iterator p;
    for (p = leaves.begin(); p != leaves.end(); ++p)
        nodes.push(&(*p));

    // combinar nodos hasta crear el arbol;
    while (nodes.size() > 1) {
        Node *first = nodes.top();
        nodes.pop();
        Node *second = nodes.top();
        nodes.pop();
        Node *combined_node = new Node(first, second);
        nodes.push(combined_node);
    }

    return nodes.top();
}


void update_huffman_code(HuffmanCode& code, Node *node, const string& prefix) {
    if (node == NULL)
        return;
    if (node->is_leaf())
        code[node->symbol] = prefix;
    else {
        update_huffman_code(code, node->left_child,  prefix + "0");
        update_huffman_code(code, node->right_child, prefix + "1");
    }
}


void free_huffman_tree(Node *node) {
    if (node == NULL || node->is_leaf())
        return;
    free_huffman_tree(node->left_child);
    free_huffman_tree(node->right_child);
    delete node;
}


int main(int argc, char *argv[]) {
    ifstream entrada("in.txt");

    // guardar la entrada estandar
    stringstream text_stream;
    //text_stream << cin.rdbuf();
    text_stream << entrada.rdbuf();

    // crear tabla de frecuencias
    SymbolCount frequencies;
    count_symbols(frequencies, text_stream);

    // saco del map los caracteres especiales @ # *
    // para que no formen parte del arbol de huffman
    unsigned int arroba = 0, asterisco = 0, gato = 0;
    arroba = frequencies['@'];
    frequencies.erase('@');
    gato = frequencies['#'];
    frequencies.erase('#');
    asterisco = frequencies['*'];
    frequencies.erase('*');

    // crear nodos hoja con las frecuencias
    vector<Node> leaves;
    SymbolCount::iterator sym;
    for (sym = frequencies.begin(); sym != frequencies.end(); ++sym) {
        Node n(sym->first, sym->second);
        leaves.push_back(n);
    }

    // crear arbol
    Node *root = create_huffman_tree(leaves);

    // ir creando el codigo de Huffman recursivamente
    HuffmanCode code;
    update_huffman_code(code, root, "11");

    // meter al map code de regreso los 3 codigos especiales
    code['@'] = "0";
    code['*'] = "101";
    code['#'] = "1001";

    free_huffman_tree(root);

    // codificar el mensaje
    vector<unsigned char> encoded_msg;
    string text(text_stream.str());
    string::iterator symbol;
    unsigned int bit_count = 0;
    int bits_in_byte = 0;
    unsigned char c = 0;

    for (symbol = text.begin(); symbol != text.end(); ++symbol) {
        string current_code(code[*symbol]);
        string::iterator bit;

        // util para debuguear el decodificador
        cout << code[*symbol];

        for (bit = current_code.begin(); bit != current_code.end(); ++bit) {
            if (*bit == '1')
                c = (c << 1) + 1;
            else if (*bit == '0')
                c = c << 1;
            else
                cerr << "ERROR" << endl;
            bit_count++;
            bits_in_byte++;
            if (bits_in_byte == 8) {
                encoded_msg.push_back(c);
                bits_in_byte = 0;
            }
        }
    }
    // meter el ultimo byte si quedo a medio construir
    if (bits_in_byte != 0) {
        c <<= 8 - bits_in_byte;
        encoded_msg.push_back(c);
    }

    // preparar el archivo de salida
    ofstream salida("out.txt");

    // imprimir la tabla de simbolos
    HuffmanCode::iterator codeword;
    for (codeword = code.begin(); codeword != code.end(); ++codeword){
        salida << codeword->first << "-" << codeword->second << endl;
    }
    salida << "--" << endl;

    salida << bit_count << endl;
    salida << "--" << endl;

    copy(encoded_msg.begin(), encoded_msg.end(),
         ostream_iterator<unsigned char>(salida));

    //salida.seekp(0,ios_base::beg);
    salida.close();

    cout << endl;
}

