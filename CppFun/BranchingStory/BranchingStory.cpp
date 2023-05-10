/*
By Peter Doria

---------------------------------
Requirements
---------------------------------
Console played branching story game

---
Loop
    output text
    output decision option text
    take in decision input
    process decision. go to next node
---
Story Beat file naming convention

$$2a_1a
    
    Prefix: "$$" -- determines is story beat file when reading file names in
    Previous Beat Syntax followed by arrow: "0-", "1a-", "2c-" -- name of previous beat with beat number and letter
    Beat Number (can be any positive integer): "1a, "3b", "11a"
    Choice Letter (can be any lowercase letter, starting from a, for each beat): 'a', 'b', 'd' -- represents a 
        unique decision option from a story beat
    Se
    
---
File to write all the player's choices to for debug purposes AND something to give them
*/

#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
#include <unordered_map>


const std::string storyBeatFileNamePrefix = "$=";
struct StoryNode
{
    std::string fileName;
    StoryNode* arrayOfNextStoryOptions;
    StoryNode(std::string _fileName = "", StoryNode* _arrayOfNextStoryOptions = NULL)
    {
        fileName = _fileName;
        if (_arrayOfNextStoryOptions == NULL)
            arrayOfNextStoryOptions = new StoryNode[static_cast<size_t>(4)] ;
    }

    ~StoryNode()
    {
        if (arrayOfNextStoryOptions != NULL)
            delete[] arrayOfNextStoryOptions;
    }

};

void createStoryMap(std::ifstream& storyFilesList, std::unordered_map<std::string, StoryNode*> storyMap)
{
    std::string extractedFileName;
    StoryNode* node = NULL;
    // adds all beats to map
    while (std::getline(storyFilesList, extractedFileName, storyBeatFileNamePrefix[storyBeatFileNamePrefix.length() -1]))
    {
        if (extractedFileName[extractedFileName.length() - 1] != storyBeatFileNamePrefix[storyBeatFileNamePrefix.length() - 2])
        {
            continue;
        }

        std::getline(storyFilesList, extractedFileName); // <--- gets actual file name
        extractedFileName = storyBeatFileNamePrefix + extractedFileName;

        std::size_t foundEndDelimiter = extractedFileName.find('_', 2);
        std::string storyBeatName = "";
        if (foundEndDelimiter != std::string::npos)
        {
            storyBeatName = extractedFileName.substr(2, foundEndDelimiter);
        }
        else if (extractedFileName[2] == '0')
        {

        }
        else
        {
            std::cout << "\nERROR! Invalid beat name:" << extractedFileName[2]; 
        }

        if (storyMap.count(storyBeatName) == 0)
        {
            storyMap.insert({ storyBeatName, new StoryNode(extractedFileName, NULL) });
        }
        else 
        {
            std::cout << "\nerror! File duplicates for " << storyBeatName << "?";
        }
    }
    std::cout << "\n\nNo more files to process . . . ";

    // add next story nodes to each story node's vector

    // go through file name string in story node. Take each beat name and assign a pointer 
        // to each relevant story node
    for (auto element : storyMap)
    {
        // previous beat name from file name
        std::string previousBeatNames = element.second->fileName;
        for (int i = 0; i < previousBeatNames.length(); i++)
        {
            std::size_t foundStartDelimiter = previousBeatNames.find(previousBeatNames.c_str(), static_cast<size_t>(i));
            if (foundStartDelimiter == std::string::npos)
                break;
            std::size_t foundEndDelimiter = previousBeatNames.find(previousBeatNames.c_str(), static_cast<size_t>(i + 1));
            std::string previousBeatSubString = "";
            if (foundEndDelimiter != std::string::npos) // not at last beat
            {
                previousBeatSubString = previousBeatNames.substr(foundStartDelimiter, foundEndDelimiter);
                // set i starting at last _ found
                i = foundEndDelimiter;
            }
            else // at last beat
            {
                previousBeatSubString = previousBeatNames.substr(foundStartDelimiter, previousBeatNames.length() - 1);
                // find previous beat in story map. if found, add current beat as a next beat

            }

            

        }
            // add current beat to previous beat node's set of "next" nodes.

        // $$2a_1a
        
    }
}

int main()
{
    if (storyBeatFileNamePrefix.length() <= 1)
    {
        std::cout << "\nFile name prefix must be at least 1 character long.";
        exit(1);
    }
    std::string dosCMD = "dir " + storyBeatFileNamePrefix + "*.txt > StoryBeats.txt";
    system(dosCMD.c_str());
    std::ifstream storyBeats("StoryBeats.txt");
    if (!storyBeats)
        std::cout << "\nCannot open story beats file.";
    std::unordered_map<std::string, StoryNode*> storyMap;
    createStoryMap(storyBeats, storyMap);
    
    //std::ifstream storyBeatFile;
    //StoryNode* node = storyQueue;
    //while (node)
    //{
    //    storyBeatFile.open(node->fileName);
    //    if (!storyBeatFile)
    //    {
    //        std::cout << "\nERROR opening story file.";
    //        /*
    //            dequeueing node
    //        */
    //        StoryNode* nextNode = node->nextNode;
    //        delete node;
    //        node = nextNode;
    //        continue;
    //    }

    //    std::string fullText = "";
    //    std::string text;
    //    while (std::getline(storyBeatFile, text))
    //    {
    //        fullText += '\n' + text;
    //        if (storyBeatFile.eof())
    //            break;
    //    }
    //    storyBeatFile.close();

    //    std::cout << fullText << std::endl;
    //    
    //    /*
    //        dequeueing node
    //    */
    //    StoryNode* nextNode = node->nextNode;
    //    delete node;
    //    node = nextNode;

    //}
}
