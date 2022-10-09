#include <iostream>
#include <string>
#include <curl/curl.h>
#include <algorithm>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef linux
#include <unistd.h>
#endif


static size_t WriteCallback(void *contents, size_t size,size_t nmemb, void *userp){
	(static_cast<std::string*>(userp))->append((char*)contents,size * nmemb);

	return size * nmemb;
}

//DEFINING FUNCTION PARSE HERE CUZ CONTROL C CONTROL V ANNOYING
std::string parse_request(std::string buffer);

std::string query_word(){
	CURL *curl;
	CURLcode response;
	std::string readBuffer;

	curl = curl_easy_init();
	if (curl){
		curl_easy_setopt(curl, CURLOPT_URL, "https://api.api-ninjas.com/v1/randomword");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		response = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}
	return parse_request(readBuffer);

}

std::string parse_request(std::string buffer){
	std::string secret_word;
	int parse_helper = 0;
	const char* n = "\"";
	const char* g = "}";
	for (int i = 0;i < buffer.size(); i++){
		if (parse_helper > 2 && parse_helper < 4 && buffer[i] != *n && buffer[i] != *g){
			secret_word+=buffer[i];
		}
		else if ( buffer[i] == *n ){
			parse_helper += 1;
		}
	}
	std::for_each(secret_word.begin(),secret_word.end(), [](char &c){
			c = ::tolower(c);
		});
	return secret_word;
}


std::vector<std::vector<char>> actual_board = {
	{'+','-','-','-','-','-','-','-','-','-','-','+'},
	{'|',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','|'},
        {'|',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','|'},
        {'|',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','|'},
        {'|',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','|'},
        {'|',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','|'},
        {'|',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','|'},
	{'+','-','-','-','-','-','-','-','-','-','-','+'}
};


void print_Board(std::vector<std::vector<char>> board){
	for (int i = 0; i < 8 ; i++){
		for (int j = 0; j < 12; j++){
			std::cout << board[i][j];
		}
		std::cout << std::endl;
	}
}

std::vector<std::vector<int>> indexes_order = { 
	{5,4},
	{5,6},
	{5,5},
	{4,5},
	{3,5},
	{2,5},
	{1,6},
	{1,7},
	{2,7},
	{3,7},
	{3,6},
	{3,8}
};

std::vector<char> index_elem = {'_','_','|','|','|','/','_','_','o','|','-','-'};

int cls(){
	try {
		#ifdef _WIN32
		system("cls");
		#endif
		#ifdef linux
		system("reset");
		#endif
	}
	catch (...){
		std::cout << "Erreur" << std::endl;
		return -1;
	}
	return 0;
}

void update_word(std::string secret_word, std::vector<char> display_progress,char choice){
	for (int i = 0; i < secret_word.size(); i++){
	       if (choice == secret_word[i]){
		       display_progress[i] = choice;
		}
	}
}

int is_available(char letter,std::vector<char> unavailable_choices){
	for (int l = 0 ; l < unavailable_choices.size(); l++){
		if ( unavailable_choices[l] == letter ) {
			return 0;
		}
	}
	return 1;
}



int main(int argc,char* argv[]){
	std::cout << "Hey !\nWanna play a game ?" << std::endl;
	std::cout << "I mean you don't really have the choice, let's get started :)" << std::endl;
	std::cout << "Rules are basic, enter a letter, if the word contains it, it will appear, else you'll get closer to the defeat !" << std::endl;
	//print_Board(actual_board);
	std::cout << "Press enter to continue...";
	std::getchar();
	cls();
	bool is_playing = true;
	while 	(is_playing) {
		std::vector<std::string> unavailable_choices;
		std::string word = query_word();
		//std::string word = "word";
		bool is_ended = false;
		int length = word.size();
		int count = 0;
		int wrong_answers = 0;
		std::string word_letters[length];
		for (int i=0;i<length;i++){word_letters[i]='_';}
		while (!(is_ended)){
			print_Board(actual_board);
			for (int j=0;j<length;j++){
				std::cout << word_letters[j] << " ";
			}
			std::cout << std::endl;
			std::cout << "Unavailable letters: ";
			for (int k = 0; k < unavailable_choices.size();k++){std::cout << unavailable_choices[k] << " ";}
			std::cout << std::endl;
ERROR_AVOID:
			std::cout << "Chose a letter which is not from the unavailable list: ";
			std::string choice;
			std::cin >> choice;
			for (int i = 0; i < unavailable_choices.size();i++){
				if (choice == unavailable_choices[i] || choice.size()>1){
					goto ERROR_AVOID;
				}	
			}
			unavailable_choices.push_back(choice);
			std::sort(unavailable_choices.begin(),unavailable_choices.end());
			bool is_correct = false;
			for (int i = 0; i < word.size() ; i++){
				char s = choice[0];
				char w = word[i];
				if ((int)s == (int)w) {
					is_correct = true;
					count+=1;	
					word_letters[i] = choice;
				}
			}
			if (!(is_correct)){
				actual_board[indexes_order[wrong_answers][0]][indexes_order[wrong_answers][1]] = index_elem[wrong_answers];
				wrong_answers++;		
			}
			if (wrong_answers == 12){
				is_ended = true;
				is_playing = false;
				std::cout << "The word was: " << word << std::endl;
				std::cout << "Pendu !" << std::endl;
			}
			if (count == length){
				is_ended = true;
				is_playing = false;
				std::cout << "Congrats ! The word was indeed " << word << std::endl;
			}
		//int count = 0;
		//for ( int i = 0; i < length; i++){
		//	if (word_letters[i] != "_"){
		//		count++;
		//}
		//if (count == length){is_ended=true;}
		//is_ended=true;	
		}

		}
		//std::cout << "Bien joué !";	

	return 0;
}
