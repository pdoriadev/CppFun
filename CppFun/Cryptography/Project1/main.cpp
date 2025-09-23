///////////////////////////////////////////////////
// By pdoria
// 
// ASCII reference: https://en.cppreference.com/w/cpp/language/ascii.html
// 
///////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include <vector>
#include <set>
#include <fstream>

using namespace std;

set<string> validWords = {"a", "and", "at", "as", "be", "but", "do", "for", "have", "he", "i", "in", "is", "it", 
						"not", "of", "on", "she", "that", "the", "to", "with", "you"};

void decrypt_shiftLettersByConstant(const string& cipher, vector<string>& plainTextOut)
{
	for (int i = 0; i < plainTextOut.size(); i++)
	{
		plainTextOut[i].resize(cipher.size(), '*');
		int letterOffset = i + 1;
		for (int j = 0; j < cipher.size(); j++)
		{

			// Is this a letter?
			int valueA = cipher[j] >= 65 && cipher[j] <= 90 ? 65 : 0;
			if (valueA == 0)
			{
				valueA = cipher[j] >= 97 && cipher[j] <= 122 ? 97 : 0;
			} 

			if (valueA == 0)
			{
 				plainTextOut[i][j] = cipher[j];
				continue;
			}

			// Transform cipher to plainText value
			char plainTextLetter;
			if (cipher[j] + letterOffset > valueA + 25)
			{
				plainTextLetter = cipher[j] + letterOffset - 26;
			}
			else
			{
				plainTextLetter = cipher[j] + letterOffset;
			}

			plainTextOut[i][j] = plainTextLetter;
		}
	}
	
}

void decrypt_shiftLettersByMinDelta(const string& cipher, string &plainTextOut)
{
	plainTextOut.resize(cipher.size(), '*');
	for (int j = 0; j < cipher.size(); j++)
	{
			
		// Is this a letter?
		std::pair<int, int> minMax = cipher[j] >= 65 && cipher[j] <= 97 ? std::make_pair(65, 97) : std::make_pair(0, 0);
		if (minMax.first == 0)
		{
			minMax = cipher[j] >= 97 && cipher[j] <= 122 ? std::make_pair(97, 122) : std::make_pair(0, 0);
		}

		if (minMax.first == 0)
		{
			plainTextOut[j] = cipher[j];
			continue;
		}
			
		// Transform cipher to plainText value
		int delta = cipher[j] - minMax.first;
		char plainTextLetter = minMax.second - delta;

		plainTextOut[j] = plainTextLetter;
	}

}

void waitForInput(string messageIn = "\nWaiting For Input. . .", string* messageOut = NULL)
{
	cout << messageIn;
	if (messageOut == NULL)
	{
		cin.ignore();
	}
	else
	{
		cin >> *messageOut;
	}
	
}

/// <summary>
/// returns decoded messages that may be valid.
/// </summary>
void findValidMessages(const vector<string>& plainTextIn, vector<int>& validIndicesOut)
{
	validIndicesOut.resize(0);
	for (int i = 0; i < plainTextIn.size(); i++)
	{
		string word = "";
		for (int j = 0; j < plainTextIn[i].size(); j++)
		{
			bool isLetter = (plainTextIn[i][j] >= 65 && plainTextIn[i][j] <= 90) ||
								(plainTextIn[i][j] >= 97 && plainTextIn[i][j] <= 122);
			if (isLetter == false)
			{
				if (validWords.count(word) > 0)
				{
					validIndicesOut.push_back(i);
					break;
				}

				word = "";
				continue;
			}

			word += tolower(plainTextIn[i][j]);
		}
	}
}

int anyValidCheck(const vector<string>& plainTextIn, const vector<int> validIndicesIn)
{
	string messageIn = "";
	int returnIndex = -1;
	waitForInput("\nTo print all potential valids without pause in-between, input 'skip'. For pauses, input anything else:", &messageIn);
	for (int i = 0; i < validIndicesIn.size(); i++)
	{
		cout << "\nVariation " << validIndicesIn[i];
		cout << "\n\"" << plainTextIn[validIndicesIn[i]] << "\"";
		if ((messageIn == "skip") == false)
		{
			waitForInput("\nInput anything to output next message. \nIf the correct message has been found, enter 'y': ", &messageIn);
		}

		if (messageIn == "y")
		{
			returnIndex = i;
			break;
		}
	}
	
	waitForInput("\nEnd of Valid Messages\n");

	return returnIndex;
}


int main()
{
	string message1 = "Mx mw zivc mqtsvxerx jsv epp gsqtyxiv wgmirxmwxw xs fi orsapihkefpi efsyx gcfivwigyvmxc tvmrmgtpiw erh tvegxmgiw.Xlmw mw csyv sttsvxyrmxc!!";
	string message2 = "zorxv dzh yvtrmmrmt gl tvg evib grivw lu hrggrmt yb svi hrhgvi lm gsv yzmp zmw lu szermt mlgsrmt gl wl lmxv li gdrxv hsv szw kvvkvw rmgl gsv \
		\n yllp svi hrhgvi dzh ivzwrmt yfg rg szw ml krxgfivh li xlmevihzgrlmh rm rg zmw dszg rh gsv fhv lu z yllp gslftsg zorxv drgslfg krxgfivh \
		\n li xlmevihzgrlm hl hsv dzh xlmhrwvirmt rm svi ldm nrmw zh dvoo zh hsv xlfow uli gsv slg wzb nzwv svi uvvo evib hovvkb zmw hgfkrw dsvgsvi \
		\n gsv kovzhfiv lu nzprmt z wzrhb xszrm dlfow yv dligs gsv gilfyov lu tvggrmt fk zmw krxprmt gsv wzrhrvh dsvm hfwwvmob z dsrgv Izyyrg drgs krmp vbvh izm xolhv yb svi";

	vector<string> plainTextVariants = {""};
	plainTextVariants.resize(26, "");
	vector<int> validMessages = {};
	
	cout << '\n' << setfill('*') << setw(30) << endl;
	decrypt_shiftLettersByConstant(message1, plainTextVariants);
	findValidMessages(plainTextVariants, validMessages);
	int decryptedIndex = anyValidCheck(plainTextVariants, validMessages);
	string outputMessage = "Message 1: " + plainTextVariants[decryptedIndex];

	cout << '\n' << setfill('*') << setw(30) << endl;
	plainTextVariants.resize(1,"");
	decrypt_shiftLettersByMinDelta(message2, plainTextVariants[0]);
	findValidMessages(plainTextVariants, validMessages);
	decryptedIndex = anyValidCheck(plainTextVariants, validMessages);
	outputMessage += "\nMessage 2: " + plainTextVariants[decryptedIndex];
	
	outputMessage += "\n******************** End of Decryption **************************";
	ofstream outputFile("DecryptedMessages.txt");
	outputFile << outputMessage;
	outputFile.close();

	return 0;
}