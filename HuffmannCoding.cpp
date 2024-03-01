
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <bitset>

using namespace std;

class BinaryTree;

// Using minHeap as a shorthand for priority_queue<BinaryTree *, vector<BinaryTree *>, greater<BinaryTree *>>
using minHeap = priority_queue<BinaryTree *, vector<BinaryTree *>, greater<BinaryTree *>>;

class HuffmanCode
{
private:
    string path;
    minHeap heap; // Use minHeap as shorthand for priority_queue<BinaryTree *, vector<BinaryTree *>, greater<BinaryTree *>>
    unordered_map<char, string> code;
    unordered_map<string, char> reverseCode;

    unordered_map<char, int> frequency_from_text(const string &text);

    void build_heap(const unordered_map<char, int> &frequency_dict);

    BinaryTree *build_binary_tree();

    void build_tree_code_helper(BinaryTree *root, const string &curr_bits);

    void build_tree_code();

    string build_encoded_text(const string &text);

    string build_padded_text(const string &encoded_text);

    vector<uint8_t> build_byte_array(const string &padded_text);

    void remove_padding(string &text);

    string decompress_text(const string &text);

public:
    HuffmanCode(const string &input_path);

    ~HuffmanCode();

    string compress();

    void decompress(const string &input_path);
};

class BinaryTree
{
public:
    char value;
    int frequ;
    BinaryTree *left;
    BinaryTree *right;

    BinaryTree(char val, int freq)
    {
        this->value = val;
        this->frequ = freq;
        left = nullptr;
        right = nullptr;
    }

    // Declare HuffmanCode as a friend class
    friend class HuffmanCode;

    bool operator<(const BinaryTree &other) const
    {
        return frequ > other.frequ;
    }

    bool operator==(const BinaryTree &other) const
    {
        return this->frequ == other.frequ;
    }
};

// Implementation of HuffmanCode member functions

unordered_map<char, int> HuffmanCode::frequency_from_text(const string &text)
{
    unordered_map<char, int> frequ_dict;
    for (char ch : text)
    {
        frequ_dict[ch]++;
    }
    return frequ_dict;
}

void HuffmanCode::build_heap(const unordered_map<char, int> &frequency_dict)
{
    for (const auto &entry : frequency_dict)
    {
        char key = entry.first;
        int frequency = entry.second;
        BinaryTree *binary_tree_node = new BinaryTree(key, frequency);
        heap.push(binary_tree_node);
    }
}

BinaryTree *HuffmanCode::build_binary_tree()
{
    // Huffman Tree is build from bottom to top.
    // The symbols with lowest frequency are at the bottom of the tree
    // that leads to longer codes for lower frequency symbols and hence
    // shorter codes for higher frequency symbol giving OPTIMAL codelength
    while (heap.size() > 1)
    // The first two trees with min. priority (i.e. frequency) are taken and
    {
        BinaryTree *binary_tree_node_1 = heap.top();
        heap.pop();
        BinaryTree *binary_tree_node_2 = heap.top();
        heap.pop();
        // a new tree is constructed taking the above trees as left and right sub-trees
        // with the frequency of root node as the sum of frequencies of left and right child.
        int sum_of_freq = binary_tree_node_1->frequ + binary_tree_node_2->frequ;
        BinaryTree *newnode = new BinaryTree('\0', sum_of_freq);
        newnode->left = binary_tree_node_1;
        newnode->right = binary_tree_node_2;
        heap.push(newnode);
        // The process continues till only one tree is left in the array of heap.
    }

    return heap.top();
}

void HuffmanCode::build_tree_code_helper(BinaryTree *root, const string &curr_bits)
{
    if (root == nullptr)
    {
        return;
    }
    if (root->value != '\0')
    {
        code[root->value] = curr_bits;
        reverseCode[curr_bits] = root->value;
        return;
    }
    build_tree_code_helper(root->left, curr_bits + '0');
    build_tree_code_helper(root->right, curr_bits + '1');
}

void HuffmanCode::build_tree_code()
{
    BinaryTree *root = build_binary_tree();
    build_tree_code_helper(root, "");
}

string HuffmanCode::build_encoded_text(const string &text)
{
    string encoded_text = "";
    for (char ch : text)
    {
        encoded_text += code[ch];
    }
    return encoded_text;
}

string HuffmanCode::build_padded_text(const string &encoded_text)
{
    string modified_encoded_text = encoded_text;

    int padding_value = 8 - (modified_encoded_text.length() % 8);
    for (int i = 0; i < padding_value; ++i)
    {
        modified_encoded_text += '0';
    }

    string padded_info = "";
    for (int i = 7; i >= 0; --i)
    {
        padded_info += ((padding_value >> i) & 1) + '0';
    }

    string padded_encoded_text = padded_info + modified_encoded_text;
    return padded_encoded_text;
}

vector<uint8_t> HuffmanCode::build_byte_array(const string &padded_text)
{
    vector<uint8_t> array;
    for (size_t i = 0; i < padded_text.length(); i += 8)
    {
        string byte = padded_text.substr(i, 8);
        array.push_back(static_cast<uint8_t>(stoi(byte, nullptr, 2)));
    }
    return array;
}

void HuffmanCode::remove_padding(string &text)
{
    string padded_info = text.substr(0, 8);
    int extra_padding = stoi(padded_info, nullptr, 2);
    text = text.substr(8, text.length() - 8 - extra_padding);
}

string HuffmanCode::decompress_text(const string &text)
{
    string decoded_text = "";
    string current_bits = "";
    for (char bit : text)
    {
        current_bits += bit;
        if (reverseCode.find(current_bits) != reverseCode.end())
        {
            char character = reverseCode[current_bits];
            decoded_text += character;
            current_bits = "";
        }
    }
    return decoded_text;
}

HuffmanCode::HuffmanCode(const string &input_path) 
{
    path=input_path;
}

HuffmanCode::~HuffmanCode()
{
    while (!heap.empty())
    {
        delete heap.top();
        heap.pop();
    }
}

string HuffmanCode::compress()
{
    ifstream file(path);
    if (!file.is_open())
    {
        cerr << "Error opening file." << endl;
        return "";
    }

    string text((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    unordered_map<char, int> frequency_dict = frequency_from_text(text);

    build_heap(frequency_dict);

    build_tree_code();

    string encoded_text = build_encoded_text(text);

    string padded_text = build_padded_text(encoded_text);

    vector<uint8_t> byte_array = build_byte_array(padded_text);

    string output_path = path.substr(0, path.find_last_of('.')) + ".bin";
    ofstream output(output_path, ios::binary);
    if (!output.is_open())
    {
        cerr << "Error creating output file." << endl;
        return "";
    }

    output.write(reinterpret_cast<const char *>(byte_array.data()), byte_array.size());
    output.close();

    cout << "Compressed" << endl;
    return output_path;
}

void HuffmanCode::decompress(const string &input_path)
{
    ifstream file(input_path, ios::binary);
    if (!file.is_open())
    {
        cerr << "Error opening file." << endl;
        return;
    }

    string bit_string;
    char byte;
    while (file.get(byte))
    {
        bit_string += bitset<8>(byte).to_string();
    }
    file.close();

    string actual_text = bit_string;
    remove_padding(actual_text);

    string decompressed_text = decompress_text(actual_text);

    string output_path = path.substr(0, path.find_last_of('.')) + "_decompressed.txt";
    ofstream output(output_path);
    if (!output.is_open())
    {
        cerr << "Error creating output file." << endl;
        return;
    }

    output << decompressed_text;
    output.close();
}

int main()
{
    string path;
    cout << "ENTER THE PATH OF YOUR FILE: ";
    getline(cin, path);

    HuffmanCode h(path);
    string output_path = h.compress();
    if (!output_path.empty())
    {
        h.decompress(output_path);
    }

    return 0;
}
