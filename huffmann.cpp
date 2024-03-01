
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <memory>

using namespace std;

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
        left = NULL;
        right = NULL;
    }

    bool operator<(const BinaryTree &other) const
    {
        return frequ > other.frequ;
    }

    bool operator==(const BinaryTree &other) const
    {
        return this->frequ == other.frequ;
    }
};

// Symbols with their frequencies are stored in the leaf nodes.
// The path from the root to the leaf node is the code of the symbol
// By convention, '0' is for left sub-tree and '1' for right sub-tree.
class HuffmanCode
{
private:
    string path;
    priority_queue<BinaryTree *, vector<BinaryTree *>, greater<BinaryTree *>> heap; // Change made here
    unordered_map<char, string> code;
    unordered_map<string, char> reverseCode;

    unordered_map<char, int> frequency_from_text(const string &text)
    {
        unordered_map<char, int> frequ_dict;
        for (char ch : text)
        {
            frequ_dict[ch]++;
        }
        return frequ_dict;
    }

    void build_heap(const unordered_map<char, int> &frequency_dict)
    {
        for (const auto &entry : frequency_dict)
        {
            char key = entry.first;
            int frequency = entry.second;
            BinaryTree *binary_tree_node = new BinaryTree(key, frequency); // Allocate on the heap
            heap.push(binary_tree_node);
        }
    }

    BinaryTree *build_binary_tree()
    {
        while (heap.size() > 1)
        {
            BinaryTree *binary_tree_node_1 = heap.top();
            heap.pop();
            BinaryTree *binary_tree_node_2 = heap.top();
            heap.pop();

            int sum_of_freq = binary_tree_node_1->frequ + binary_tree_node_2->frequ;
            BinaryTree *newnode = new BinaryTree('\0', sum_of_freq);
            newnode->left = binary_tree_node_1;
            newnode->right = binary_tree_node_2;
            heap.push(newnode);
        }
        return heap.top();
    }

    void build_tree_code_helper(BinaryTree *root, const string &curr_bits)
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

    void build_tree_code()
    {
        BinaryTree *root = build_binary_tree();
        build_tree_code_helper(root, "");
    }

    string build_encoded_text(const string &text)
    {
        string encoded_text = "";
        for (char ch : text)
        {
            encoded_text += code[ch];
        }
        return encoded_text;
    }

    string build_padded_text(const string &encoded_text)
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

    vector<uint8_t> build_byte_array(const string &padded_text)
    {
        vector<uint8_t> array;
        for (size_t i = 0; i < padded_text.length(); i += 8)
        {
            string byte = padded_text.substr(i, 8);
            array.push_back(static_cast<uint8_t>(stoi(byte, nullptr, 2)));
        }
        return array;
    }

    void remove_padding(string &text)
    {
        string padded_info = text.substr(0, 8);
        int extra_padding = stoi(padded_info, nullptr, 2);
        text = text.substr(8, text.length() - 8 - extra_padding);
    }

    string decompress_text(const string &text)
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

public:
    // HuffmanCode(const string &input_path) : path(input_path) {}
    HuffmanCode(const string &input_path)  {
        path=input_path;
    }
    ~HuffmanCode()
    {
        while (!heap.empty())
        {
            delete heap.top();
            heap.pop();
        }
    }
    string compress()
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

    void decompress(const string &input_path)
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
};

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
