/*
Peter Doria

                        Login Registration System


=====================
INSTRUCTIONS

1. Stores user credentials (username, password)
    - all user infos stored in csv. 
        "data, data, data, \n"
        "data, data, data, \n"
    - usernames no longer than 10 characters
    - password no longer than 15 characters
2. Registers new users with new username / password
    - verifies unique username during creation
    - communicates password strength while password is being typed
3. Login system for existing users into application
4. Logout
5. Modify user profile

Part 2:
1. Forgot password reset
    - verify username to reset

====================
PERSONAL NOTES
Attempt to write all in one file. See how it works. Consider separating into headers afterwards.
====================
*/

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "WinConsoleMethods.h"
#include <vector>

static const unsigned int maxDataMembers = 3;
class userProfile
{
    std::vector<std::string> userData;
    unsigned int _maxDataMembers = maxDataMembers;
public:
    
    userProfile(std::string _username, std::string _password, std::string _favoriteColor)
    {
        userData.push_back(_username);
        userData.push_back(_password);
        userData.push_back(_favoriteColor);
        if (userData.size() > _maxDataMembers)
        {
            assert(("User data has been given more members than can be interpreted correctly. Does not match data model.", false));
        }
    }
};

static outputController UIController;
static std::unordered_set<std::string> validLoginOrCreateChars{"L", "l", "C", "c", "Q", "q"};
static std::unordered_set<std::string> validYesOrNoChars{ "Y", "y", "n", "N"};
static std::unordered_set<std::string> invalidUsernamePasswordChars{",", " ", "\n"};
static unsigned int leftmostColumn = 30;
static unsigned int topmostColumn = 5;

bool loadInUserData(std::unordered_map<std::string, std::string>& usernamePasswordMap, std::unordered_map<std::string, userProfile*> profileMap);
std::string getUserInput();
bool checkGlobalInputCommands(std::string input); 
bool isYes(std::string yesOrNo);
std::string loginOrCreateAccountPrompt();
userProfile* login( std::unordered_map<std::string, std::string>& usernamePasswordMap, std::unordered_map<std::string, userProfile*> profileMap);
userProfile* logout();
userProfile* registerNewUser( std::unordered_map<std::string, std::string>& usernamePasswordMap, std::unordered_map<std::string, userProfile*> profileMap);
userProfile* modifyProfile(std::unordered_map<std::string, std::string>& usernamePasswordMap, std::unordered_map<std::string, userProfile*> profileMap);
void quit();

bool loadInUserData(std::unordered_map<std::string, std::string>& usernamePasswordMap, std::unordered_map<std::string, userProfile*> profileMap)
{
    std::ifstream userProfileCSV;
    userProfileCSV.open("userProfiles.csv");
    if (!userProfileCSV.is_open())
    {
        assert(("userProfile.csv failed to open", false));
        return false;
    }

    std::string username;
    std::string password;
    std::string favoriteColor;
    
    std::string extractedLine;
    std::string extractedWord;
    int i = 0;
    while (getline(userProfileCSV, extractedLine))
    {
        while (!extractedLine.empty())
        {
            size_t foundComma = extractedLine.find(',');
            if (foundComma != std::string::npos)
            {
                if (extractedWord[0] == ',')
                {
                    assert(("Unexpected to see comma at beginning of word."));
                }
                else
                {
                    extractedWord = extractedLine.substr(0, foundComma);
                    extractedLine = extractedLine.substr(foundComma + 1, extractedLine.length() - 1);
                }

                if (extractedWord.find(',') != std::string::npos)
                {
                    assert(("Should not have 2 commas by one word/sequence of chars"));
                }
            }
            else
            {
                extractedWord = extractedLine;
                extractedLine = "";
            }

            switch (i)
            {
            case 0:
                username = extractedWord;
                break;
            case 1:
                password = extractedWord;
                break;
            case 2:
                favoriteColor = extractedWord;
                break;
            default:
                assert(("Reached an undescribed case", false));
            }

            i++;

            if (foundComma == std::string::npos)
            {
                usernamePasswordMap.insert(std::make_pair(username, password));
                std::string usernamePasswordCombo = username + password;
                profileMap.insert(std::make_pair(usernamePasswordCombo, new userProfile(username, password, favoriteColor)));
                username = "";
                password = "";
                favoriteColor = "";
                i = 0;
            }
        }
    }
    
    userProfileCSV.close();
    return true;
}

std::string getUserInput()
{
    std::string input; 
    std::cin >> input;
    checkGlobalInputCommands(input);
    return input;
}

bool checkGlobalInputCommands(std::string userInput)
{
    if (userInput == "Q" || userInput == "q")
    {
        quit();
        assert(("If this runs, then we didn't quit?!", false));
    }

    return false;
}

bool isYes(std::string yesOrNo)
{
    if (yesOrNo == "y" || yesOrNo == "Y")
    {
        return true;
    }

    return false;
}


std::string loginOrCreateAccountPrompt()
{
    UIController.placeCursor(5, 30);
    std::cout << "============== Login / Create Account ==============";

    UIController.shiftCursorFromLastSetPos(2, 0);
    std::cout << "Press 'Q' to quit the application at any time.";
    Sleep(5);

    UIController.shiftCursorFromLastSetPos(2, 0);
    std::cout << "To Login, press 'L', then press 'Enter'\n";
    Sleep(5);

    UIController.shiftCursorFromLastSetPos(2, 0);
    std::cout << "To Create a new account, press 'C', then press 'Enter'";
    Sleep(5);

    UIController.shiftCursorFromLastSetPos(2, 0);
    std::string loginOrCreateInput;
    loginOrCreateInput = getUserInput();
    while (validLoginOrCreateChars.count(loginOrCreateInput) == 0)
    {
        checkGlobalInputCommands(loginOrCreateInput);
        UIController.shiftCursorFromLastSetPos(2, 0);
        std::cout << loginOrCreateInput + " is not an option.";
        Sleep(1000);
        std::cout << " Please Try Again .";
        Sleep(500);
        for (int i = 0; i < 5; i++)
        {
            Sleep(500);
            std::cout << " . ";
        }
        UIController.clearLine();
        UIController.shiftCursorFromLastSetPos(-2, 0);
        UIController.clearLine();
        loginOrCreateInput = "";
        loginOrCreateInput = getUserInput();
    }
    return loginOrCreateInput;
}

userProfile * login(std::unordered_map<std::string, std::string>& usernamePasswordMap, std::unordered_map<std::string, userProfile*> profileMap)
{
    std::string usernameInput = "";
    std::string passwordInput = "";
    COORD usernamePos;
    COORD passwordPos;
    int maxLoginAttempts = 3;
    int loginAttempts = 0;

    UIController.clearConsole();
    std::cout << std::endl;
    UIController.placeCursor(5, 35);
    std::cout << "===== LOGIN MENU ======";

    while (loginAttempts < maxLoginAttempts)
    {
        loginAttempts++;
        UIController.placeCursor(7, 35);
        std::cout << "Username: ";
        usernamePos = UIController.getCursorPosition();
        usernameInput = getUserInput();


        UIController.placeCursor(9, 35);
        std::cout << "\nPassword: ";
        passwordPos = UIController.getCursorPosition();
        passwordInput = getUserInput();

        if (usernamePasswordMap.count(usernameInput) == 1 && usernamePasswordMap.at(usernameInput) == passwordInput)
        {
            if (profileMap.count(usernameInput + passwordInput) == 1)
            {
                return profileMap.at(usernameInput + passwordInput);
            }
            assert(("Username password combo is valid but could not find matching profile for username password combo", false));
        }

        std::string createAccountOrEnterAgain = "";
        std::cout << "Username password combo not found. Would you like to create a new account?\n"; 
        while (validYesOrNoChars.count(createAccountOrEnterAgain) == 0)
        {
            UIController.placeCursor(10, 35);
            UIController.clearLine();
            std::cout << "Enter 'y' to create a new account. Enter 'n' to enter a username, again.\n";
            UIController.clearLine();
            std::cout << "'y' or 'n': ";
            std::cin >> createAccountOrEnterAgain;
        }

        if (isYes(createAccountOrEnterAgain))
        {
            return registerNewUser(usernamePasswordMap, profileMap);
        }        
    }

    if (loginAttempts >= maxLoginAttempts)
    {
        UIController.clearConsole();
        UIController.placeCursor(5, 35);
        std::cout << "Username and password combination not found";
        UIController.placeCursor(7, 35);
        std::cout << "Sorry. You have exceeded your maximum login attempts. Please try again later.";
        Sleep(10);
        UIController.clearConsole();
        return NULL;
    }

    assert(("Missed case. Did not think this would get called.", false));
    return NULL;

}

userProfile* logout()
{
    return NULL;
}

userProfile * registerNewUser(std::unordered_map<std::string, std::string>& usernamePasswordMap, std::unordered_map<std::string, userProfile*> profileMap)
{
    userProfile* profile = new userProfile("user", "password", "blue");
    return profile;
}

userProfile* modifyProfile( std::unordered_map<std::string, std::string>& usernamePasswordMap, std::unordered_map<std::string, userProfile*> profileMap)
{
    userProfile* profile = new userProfile("user", "password", "blue");
    return profile;
}

void quit()
{
    UIController.clearConsole();
    UIController.placeCursor(15, 30);
    logout();
    std::cout << "Bye ";
    Sleep(200);
    std::cout << "bye ";
    for (int i = 0; i < 5; i++)
    {
        Sleep(200);
        std::cout << ". ";
    }
    exit(0);
}

int main()
{
    std::unordered_map<std::string, std::string> usernamePasswordMap;
    std::unordered_map<std::string, userProfile*> usernamePasswordToProfileMap;
    loadInUserData(usernamePasswordMap, usernamePasswordToProfileMap);

    UIController = *(new outputController());

    std::string userChoice = loginOrCreateAccountPrompt();

    userProfile * profile;
    if (userChoice == "L" || userChoice == "l")
    {
        profile = login(usernamePasswordMap, usernamePasswordToProfileMap);
        if (profile == NULL)
        {
            profile = registerNewUser(usernamePasswordMap, usernamePasswordToProfileMap);
        }              
    }
    else
    {
        profile = registerNewUser(usernamePasswordMap, usernamePasswordToProfileMap);
    }



    // Logged in. Modify profile prompt

    // Log out option
           

}

   

