#include <iostream>
#include <vector>
#include <array>
#include <map>
#include <fstream>
using namespace std;

//global var to keep track of index in vocab
int vocab_index = 256;

string print_token(int index, vector<array<int,2>> &vocab);

//vocab -> {{key(0), l(1), r(2)}}

void build_init_vocab(vector<int> &int_str, vector<array<int, 2>> &vocab){
	for(int i : int_str){
		vocab[i][0] = i;
	}
}

void encode(vector<array<int, 2>> &vocab, vector<int> &int_str){

	map<array<int, 2>, int> freq;
	//make the frequency map
	for(int i = 0; i < int_str.size()-1; i++){
		if(freq.find({int_str[i], int_str[i+1]}) == freq.end()){
			freq.insert({{int_str[i], int_str[i+1]}, 1});
		}else{
			freq.at({int_str[i], int_str[i+1]}) += 1;
		}
	}

	//get the max freq element from the map
	array<int, 2> maxElement;
	int maxFreq = -1;
	for(auto& it : freq){
		if(it.second > maxFreq){
			maxElement = it.first;
			maxFreq = it.second;
		}
	}

	//base case
	if(maxFreq <= 1) {
		return;
	}

	//put the max element in the vocab
	vocab[vocab_index][0] = maxElement[0]; //left
	vocab[vocab_index][1] = maxElement[1]; //right
	
	//update the string
	int write = 0;
	for(int read = 0; read < int_str.size() - 1; read++){
		if(int_str[read] == maxElement[0] && int_str[read+1] == maxElement[1]){
			int_str[write] = vocab_index;
			write++;
			read++;//skip the next element
		}else{
			int_str[write] = int_str[read];
			write++;
		}
	}

	//resize the string
	int_str.resize(write);

	//update the vocab index
	vocab_index++;

	//recursive call
	encode(vocab, int_str);

}

void decode(vector<int> &int_str, vector<array<int, 2>> &vocab){
	for (int i : int_str){
		cout<<print_token(i, vocab);
	}
}

void print_text(vector<int>int_str){
	for(int i : int_str){
		if(i < 256) cout<<(char)i;
		else cout<<"["<<i<<"]";
	}
	cout<<"\n";
	
}

unordered_map<int, string> memo;
string print_token(int index, vector<array<int, 2>> &vocab){	
	if(memo.find(index)!=memo.end()){
		return memo[index];
	}

	string result;

	if(index < 256){
		result = string(1,(char)vocab[index][0]);
	}else{
	result = print_token(vocab[index][0], vocab) + print_token(vocab[index][1], vocab);
	}

	memo[index] = result;

	return result;
}

void print_tokens(vector<array<int, 2>> &vocab){
	for(int i = 0; i < vocab_index; i++){
		if(vocab[i][0] == 0 && vocab[i][1] == 0){
			//do nothing since this is not used (empty)
		}else{
			cout<<"["<<i<<"] -> ";
			cout<<print_token(i, vocab);
			cout<<"\n";
		}
	}
}


void generate_graphviz(const vector<array<int, 2>> &vocab, const string &filename) { //THIS function is AI generated
    ofstream file(filename);

    if (!file.is_open()) {
        cerr << "Failed to open file: " << filename << endl;
        return;
    }

    file << "digraph BPE_Tokens {\n";
    file << "    node [shape=ellipse, fontname=\"Helvetica\", fontsize=14, color=blue];\n";
	file << "    ratio=fill;\n";
	file << "    size=\"10,6\";\n"; // Width x Height in inches


    // Output nodes and edges for each vocab entry
    for (int i = 256; i < vocab_index; i++) {
        string token = print_token(i, const_cast<vector<array<int, 2>>&>(vocab));

        string left = print_token(vocab[i][0], const_cast<vector<array<int, 2>>&>(vocab));
        string right = print_token(vocab[i][1], const_cast<vector<array<int, 2>>&>(vocab));

        // Node for the current token
        file << "    \"" << i << " (" << token << ")\" [color=orange];\n";

        // Edges to left and right parts
        file << "    \"" << i << " (" << token << ")\" -> \"" << vocab[i][0] << " (" << left << ")\";\n";
        file << "    \"" << i << " (" << token << ")\" -> \"" << vocab[i][1] << " (" << right << ")\";\n";
    }

    file << "}\n";
    file.close();

    cout << "Graphviz file generated: " << filename << endl;
}//NOTE: this graphviz is just to visualize, generated using CHATGPT!!


int main()
{
	ifstream file("text.txt");
	if(!file){
		cerr<<"file opening error\n";
		return 1;
	}

	string init_string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

	file.close();
	
	vector<int> int_str;

	for(char c : init_string){
		int_str.push_back((int)c);
	}

	vector<array<int, 2>> vocab(2000);

	build_init_vocab(int_str, vocab);
	
	cout<<int_str.size()<<"\n";

	print_text(int_str);

	encode(vocab, int_str);

	cout<<int_str.size()<<"\n"<<vocab_index;

	print_text(int_str);

	cout<<"\n";

	print_tokens(vocab);

	decode(int_str, vocab);

	generate_graphviz(vocab, "bpe_graph.dot");
}

/*NOTE: segfault caused in the initial code due to:
			out of bound access
			i tried to access vector[i]
			without defining its size
			OR
			pushing back things
			KEEP this in mind for the next time

		other things to keep in mind
			should have changed vector<array<int, 2>>
			to vector<pair<int, int>>
			because apparently its more efficient
			but i didnt change it cause its a hassle
*/
