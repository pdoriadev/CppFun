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
#include <iterator>

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

    std::string getUsername() { return userData[0]; }

    std::string getPassword() { return userData[1]; }

    std::string getFavoriteColor() { return userData[2]; }

};


bool loadAllUserData(std::unordered_map<std::string, std::string>& usernamePasswordMap, std::unordered_map<std::string, userProfile*>& profileMap);
bool loginOrCreateAccountPrompt();
userProfile* login(std::unordered_map<std::string, std::string>& usernamePasswordMap, std::unordered_map<std::string, userProfile*>& profileMap);
userProfile* logout();
userProfile* createNewUser(std::unordered_map<std::string, std::string>& usernamePasswordMap, std::unordered_map<std::string, userProfile*>& profileMap);
bool writeProfileDataToUserProfilesCSV(userProfile* profile);
userProfile* modifyProfile(std::unordered_map<std::string, std::string>& usernamePasswordMap, std::unordered_map<std::string, userProfile*>& profileMap);
void quit();

std::string getUserInput();
bool checkGlobalInputCommands(std::string input);
bool isYes(std::string yesOrNo);

static outputController UIController;
static std::unordered_set<std::string> validLoginOrCreateChars{"L", "l", "C", "c"};
static std::unordered_set<std::string> quitCommandString{ ":q", "Q" };
static std::unordered_set<std::string> validYesOrNoChars{ "Y", "y", "n", "N"};
static std::unordered_set<std::string> invalidUsernamePasswordChars{",", "' '", ":"};
static unsigned int minUsernameLength = 4;
static unsigned int maxUsernameLength = 13;
static unsigned int minPasswordLength = 8;
static unsigned int maxPasswordLength = 23;

static unsigned int leftmostColumn = 30;
static unsigned int topmostRow = 5;


bool loadAllUserData(std::unordered_map<std::string, std::string>& usernamePasswordMap, std::unordered_map<std::string, userProfile*>& profileMap)
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
                profileMap.insert(std::make_pair(username + password, new userProfile(username, password, favoriteColor)));

                username = "";
                password = "";
                favoriteColor = "";
                extractedWord = "";
                extractedLine = "";
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
    if (quitCommandString.count(userInput) > 0)
    {
        quit();
        assert(("If this assert executes, then we didn't quit?!", false));
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


bool loginOrCreateAccountPrompt()
{
    UIController.placeCursor(5, 30);
    std::cout << "============== Login / Create Account ==============";

    UIController.shiftCursorFromLastSetPos(2, 0);
    std::cout << "Enter ':q' to quit the application at any time.";
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

    if (loginOrCreateInput == "L" || loginOrCreateInput == "l")
    {
        return true;
    }
    return false;
}

userProfile * login(std::unordered_map<std::string, std::string>& usernamePasswordMap, std::unordered_map<std::string, userProfile*>& profileMap)
{
    std::string usernameInput = "";
    std::string passwordInput = "";
    COORD usernamePos;
    COORD passwordPos;
    int maxLoginAttempts = 3;
    int loginAttempts = 0;

    UIController.clearConsole();
    std::cout << std::endl;
    UIController.placeCursor(5, 30);
    int LoginMenuTop = UIController.getCursorPosition().Y;
    std::cout << "=========== LOGIN MENU ===========";
    UIController.shiftCursorFromLastSetPos(1, 0);

    while (loginAttempts < maxLoginAttempts)
    {
        loginAttempts++;
        UIController.clearBelowGivenLine(LoginMenuTop);
        if (loginAttempts == 1)
        {
            UIController.shiftCursorFromLastSetPos(2, 0);
            usernamePos = UIController.getCursorPosition();
        }
        else
        {
            UIController.placeCursor(usernamePos.Y, usernamePos.X);
        }
        std::cout << "Username: ";
        usernameInput = getUserInput();

        if (loginAttempts == 1)
        {
            UIController.shiftCursorFromLastSetPos(2, 0);
            passwordPos = UIController.getCursorPosition();
        }
        else
        {
            UIController.placeCursor(passwordPos.Y, passwordPos.X);
        }
        std::cout << "Password: ";
        passwordInput = getUserInput();

        if (usernamePasswordMap.count(usernameInput) == 1 && usernamePasswordMap.at(usernameInput) == passwordInput)
        {
            if (profileMap.count(usernameInput + passwordInput) == 1)
            {
                UIController.shiftCursorFromLastSetPos(2,0);
                COORD successPos = UIController.getCursorPosition();
                bool isBlank = false;
                for (unsigned int i = 0; i < 200; i++)
                {
                    if (i % 30 == 0)
                    {
                        if (isBlank)
                        {
                            UIController.placeCursor(successPos.Y, successPos.X);
                            std::cout << "Success! Profile found!";
                        }
                        else
                        {
                            UIController.clearLine();
                            isBlank == false;
                        }
                    }
                    Sleep(1);
                }
                UIController.clearConsole();

                return profileMap.at(usernameInput + passwordInput);
            }
            assert(("Username password combo is valid but could not find matching profile for username password combo", false));
        }

        std::string createAccountOrEnterAgain = "";
        UIController.shiftCursorFromLastSetPos(1, 0);

        std::cout << "Username or Password not found. Attempt to login again or create a new account?"; 
        while (validYesOrNoChars.count(createAccountOrEnterAgain) == 0)
        {
            UIController.shiftCursorFromLastSetPos(2, 0);
            UIController.clearLine();
            std::cout << "Enter 'y' to create a new account. Enter 'n' to enter a username/password, again.";
            
            UIController.shiftCursorFromLastSetPos(1, 0);
            UIController.clearLine();
            std::cout << "'y' or 'n': ";
            createAccountOrEnterAgain = getUserInput() ;
            
            UIController.shiftCursorFromLastSetPos(-3, 0);
        }

        if (isYes(createAccountOrEnterAgain))
        {
            return createNewUser(usernamePasswordMap, profileMap);
        }        
        else
        {
            UIController.clearToRightOnLineFromPos(usernamePos);
            UIController.clearToRightOnLineFromPos(passwordPos);
            UIController.clearBelowGivenLine(passwordPos.Y);
        }
    }

    if (loginAttempts >= maxLoginAttempts)
    {
        UIController.clearConsole();
        UIController.placeCursor(7, 30);
        std::cout << "Username and password combination not found.";
        UIController.shiftCursorFromLastSetPos(2, 0);
        std::cout << "You have exceeded your maximum login attempts. Please try again later, or create a new account. ";
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

userProfile * createNewUser(std::unordered_map<std::string, std::string>& usernamePasswordMap, std::unordered_map<std::string, userProfile*>& profileMap)
{
    userProfile* profile = NULL;

    UIController.clearConsole();
    UIController.placeCursor(topmostRow, leftmostColumn);
    std::cout << "============== Create a Profile ==============";
    UIController.shiftCursorFromLastSetPos(2, 0);
    std::cout << "Username must be between " << minUsernameLength << " and " << maxUsernameLength << " characters.";
    UIController.shiftCursorFromLastSetPos(1, 0);
    std::cout << "Password must be between " << minPasswordLength << " and " << maxPasswordLength << " characters.";
    UIController.shiftCursorFromLastSetPos(1, 0);
    std::cout << "Illegal characters: ";
    for (auto i = invalidUsernamePasswordChars.begin(); i != invalidUsernamePasswordChars.end(); i++)
    {
        if (invalidUsernamePasswordChars.size() == 1)
        {
            std::cout << *i;
            break;
        }

        if (std::next(i) == invalidUsernamePasswordChars.end())
        {
            std::cout << "and " << *i;
            break;
        }
        std::cout << *i << ", ";
    }

    COORD usernamePos;
    COORD passwordPos;
    COORD validMessagePos;

    UIController.shiftCursorFromLastSetPos(2, 0);
    std::cout << "New Username: ";
    usernamePos = UIController.getCursorPosition();
    
    UIController.shiftCursorFromLastSetPos(2, 0);
    std::cout << "New Password: ";
    passwordPos = UIController.getCursorPosition();
    
    UIController.placeCursor(usernamePos.Y, usernamePos.X);
    validMessagePos = UIController.CalculateCOORDValueAsIfShifted(passwordPos,3, 0);

    std::string input;

    std::string usernameInput;
    bool completedUsernameInput = false;
    while (!completedUsernameInput)
    {
        Sleep(10);
        UIController.placeCursor(usernamePos.Y, usernamePos.X);
        input = "";
        std::cin >> input;

        checkGlobalInputCommands(input);

        if (usernameInput.length() < minUsernameLength)
        {
            completedUsernameInput = false;
            UIController.placeCursor(validMessagePos.Y, validMessagePos.X);
            UIController.clearToRightOnLine();
            std::cout << "Username is too short.";
        }
        else if (usernameInput.length() > maxUsernameLength)
        {
            completedUsernameInput = false;
            UIController.placeCursor(validMessagePos.Y, validMessagePos.X);
            UIController.clearToRightOnLine();
            std::cout << "Username is too long.";
        }

        if (usernamePasswordMap.count(usernameInput) > 0)
        {
            completedUsernameInput = false;
            UIController.placeCursor(validMessagePos.Y, validMessagePos.X);
            UIController.clearToRightOnLine();
            std::cout << "Username already exists.";
        }
    }

    std::string passwordInput;
    bool completedPasswordInput = false;
    while (!completedPasswordInput)
    {
        Sleep(10);
        UIController.placeCursor(passwordPos.X, passwordPos.Y);
        input = "";
        std::cin >> input;

        checkGlobalInputCommands(input);

        for (unsigned int i = 0; i < input.length() - 1; i++)
        {
            std::string test = "";
            test += input[i];
            if (invalidUsernamePasswordChars.count(test) == 0)
            {
                usernameInput += input[i];
            }
            if (input[i] == '\r' || input[i] == '\r')
            {
                completedUsernameInput = true;
            }
        }

        if (passwordInput.length() < minPasswordLength)
        {
            completedPasswordInput = false;
            UIController.placeCursor(validMessagePos.Y, validMessagePos.X);
            UIController.clearToRightOnLine();
            std::cout << "Passord is too short.";
        }
        else if (passwordInput.length() > maxPasswordLength)
        {
            completedPasswordInput = false;
            UIController.placeCursor(validMessagePos.Y, validMessagePos.X);
            UIController.clearToRightOnLine();
            std::cout << "Password is too long.";
        }
    }
    
    // favorite color?

    usernamePasswordMap.insert(std::make_pair(usernameInput, passwordInput));
    profile = new userProfile(usernameInput, passwordInput, "");
    profileMap.insert(std::make_pair(usernameInput + passwordInput, profile));

    writeProfileDataToUserProfilesCSV(profile);

    return profile;
}

bool isValidPassword(std::string password)
{
    
}

bool writeProfileDataToUserProfilesCSV(userProfile* profile)
{
    std::ofstream outputFile("userProfiles.csv", std::ios::app);
    
    if (!outputFile.is_open())
    {
        assert(("Failed to open output file"));
        return false;
    }

    outputFile << 
        profile->getUsername() << "," << 
        profile->getPassword() << "," << 
        profile->getFavoriteColor() << '\n';

    outputFile.close();
}

userProfile* modifyProfile( std::unordered_map<std::string, std::string>& usernamePasswordMap, std::unordered_map<std::string, userProfile*>& profileMap)
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
    loadAllUserData(usernamePasswordMap, usernamePasswordToProfileMap);


    bool choseToLogin = loginOrCreateAccountPrompt();
    userProfile * profile;
    if (choseToLogin)
    {
        profile = login(usernamePasswordMap, usernamePasswordToProfileMap);
        if (profile == NULL)
        {
            profile = createNewUser(usernamePasswordMap, usernamePasswordToProfileMap);
        }              
    }
    else
    {
        profile = createNewUser(usernamePasswordMap, usernamePasswordToProfileMap);
    }



    // Logged in. Modify profile prompt

    // Log out option
           

}

   

