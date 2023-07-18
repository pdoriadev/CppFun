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

struct validationResult
{
    bool isValid = false;
    std::string validationMessage;
    validationResult(bool _isValid, std::string _validationMessage)
    {
        validationMessage = _validationMessage;
        isValid = _isValid;
    }
};


bool loadAllUserData(std::unordered_map<std::string, std::string>& usernamePasswordMap, std::unordered_map<std::string, userProfile*>& profileMap);
bool loginOrCreateAccountPrompt();
userProfile* login(std::unordered_map<std::string, std::string>& usernamePasswordMap, std::unordered_map<std::string, userProfile*>& profileMap);
userProfile* logout();
userProfile* createNewProfile(std::unordered_map<std::string, std::string>& usernamePasswordMap, std::unordered_map<std::string, userProfile*>& profileMap);
validationResult validateUsername(std::string, std::unordered_map<std::string, std::string>& usernamePasswordMap);
validationResult validatePassword(std::string);
std::string SelectFavoriteColor();
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
static std::unordered_set<std::string> illegalUsernamePasswordChars{",", "<", "'", "(", ")"," ", ";", ":"};
static std::vector<std::string> colors{ "Red", "Yellow", "Blue", "Green", "Orange", "Purple" };
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
                        if (isBlank == false)
                        {
                            UIController.placeCursor(successPos.Y, successPos.X);
                            std::cout << "Success! Profile found!";
                            isBlank = true;
                        }
                        else
                        {
                            UIController.clearLine();
                            isBlank = false;
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
            return createNewProfile(usernamePasswordMap, profileMap);
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

userProfile * createNewProfile(std::unordered_map<std::string, std::string>& usernamePasswordMap, std::unordered_map<std::string, userProfile*>& profileMap)
{
    userProfile* profile;

    UIController.clearConsole();
    UIController.placeCursor(topmostRow, leftmostColumn);
    std::cout << "============== Create a Profile ==============";
    UIController.shiftCursorFromLastSetPos(2, 0);
    std::cout << "Username must be between " << minUsernameLength << " and " << maxUsernameLength << " characters.";
    UIController.shiftCursorFromLastSetPos(1, 0);
    std::cout << "Password must be between " << minPasswordLength << " and " << maxPasswordLength << " characters.";
    UIController.shiftCursorFromLastSetPos(1, 0);
    std::cout << "Illegal characters: ";
    for (auto i = illegalUsernamePasswordChars.begin(); i != illegalUsernamePasswordChars.end(); i++)
    {
        if (illegalUsernamePasswordChars.size() == 1)
        {
            std::cout << *i;
            break;
        }

        if (std::next(i) == illegalUsernamePasswordChars.end())
        {
            std::cout << "and " << *i;
            break;
        }
        std::cout << *i << " ";
    }

    COORD usernameInputPos;
    COORD passwordInputPos;
    COORD validationMessagePos;
    

    UIController.shiftCursorFromLastSetPos(2, 0);
    std::cout << "New Username: ";
    usernameInputPos = UIController.getCursorPosition();
    
    UIController.shiftCursorFromLastSetPos(1, 0);
    std::cout << "New Password: ";
    passwordInputPos = UIController.getCursorPosition();
    
    validationMessagePos = UIController.CalculateCOORDValueAsIfShifted(UIController.getLastSetPosition(),1, 0);

    unsigned int attempts = 0;
    std::string usernameInput;
    while (true)
    {
        attempts++;
        UIController.placeCursor(usernameInputPos.Y, usernameInputPos.X);
        UIController.clearToRightOnLine();
        usernameInput = getUserInput();
    
        validationResult validation = validateUsername(usernameInput, usernamePasswordMap);
        UIController.placeCursor(validationMessagePos.Y, validationMessagePos.X);
        UIController.clearToRightOnLine();
        std::cout << validation.validationMessage;

        if (validation.isValid)
        {
            break;
        }

        if (attempts > 6)
        {
            UIController.clearConsole();
            UIController.placeCursor(validationMessagePos.Y, validationMessagePos.X);
            UIController.clearToRightOnLine();
            std::cout << "You have exceeded maximum username input attempts. Please try creating an account later ";
            for (unsigned int i = 0; i < 1000; i++)
            {
                if (i % 200)
                {
                    std::cout << ". ";
                }
            }
            UIController.clearConsole();
            return NULL;
        }
    }

    attempts = 0;
    std::string passwordInput;
    while (true)
    {
        attempts++;
        UIController.placeCursor(passwordInputPos.Y, passwordInputPos.X);
        UIController.clearToRightOnLine();
        passwordInput = getUserInput();

        validationResult validation = validatePassword(passwordInput);
        UIController.placeCursor(validationMessagePos.Y, validationMessagePos.X);
        UIController.clearToRightOnLine();
        std::cout << validation.validationMessage;

        if (validation.isValid)
        {
            break;
        }

        if (attempts > 6)
        {
            UIController.clearConsole();
            UIController.placeCursor(validationMessagePos.Y, validationMessagePos.X);
            UIController.clearToRightOnLine();
            std::cout << "You have exceeded maximum password input attempts. Please try creating an account later ";
            for (unsigned int i = 0; i < 1000; i++)
            {
                if (i % 200)
                {
                    std::cout << ". ";
                }
            }
            UIController.clearConsole();
            return NULL;
        }
    }
    
    std::string color = SelectFavoriteColor();

    usernamePasswordMap.insert(std::make_pair(usernameInput, passwordInput));
    profile = new userProfile(usernameInput, passwordInput, color);
    profileMap.insert(std::make_pair(usernameInput + passwordInput, profile));

    writeProfileDataToUserProfilesCSV(profile);

    return profile;
}

validationResult validateUsername(std::string username, std::unordered_map<std::string, std::string>& usernamePasswordMap)
{
    bool isValid = true;
    std::string message;
    if (usernamePasswordMap.count(username) > 0)
    {
        isValid = false;
        message += "Username already exists. ";
        return validationResult(isValid, message);
    }

    for (unsigned int i = 0; i < username.length(); i++)
    {
        std::string test = "";
        test = username[i];
        if (illegalUsernamePasswordChars.count(test))
        {
            isValid = false;
            message += "Username has invalid characters";
            break;
        }
    }

    if (username.length() < minUsernameLength)
    { 
        isValid = false;
        message += "Username is too short. ";
    }
    else if (username.length() > maxUsernameLength)
    {
        isValid = false;
        message += "Username is too long. ";
    }

    if (isValid)
    {
        message += "What a lovely username <3";
    }

    return validationResult(isValid, message);
}

validationResult validatePassword(std::string password)
{
    bool valid = true;
    std::string message = "";
    for (unsigned int i = 0; i < password.length() - 1; i++)
    {
        std::string testChar = "";
        testChar += password[i];
        if (illegalUsernamePasswordChars.count(testChar) == 1)
        {
            valid = false;
            message += "Password has illegal characters. ";
        }
    }

    if (password.length() < minPasswordLength)
    {
        valid = false;
        message += "Password is too short";
    }
    else if (password.length() > maxPasswordLength)
    {
        valid = false;
        message += "Password is too long";
    }

    if (valid)
    {
        message += "Password is good!";
    }

    return validationResult(valid, message);
}

std::string SelectFavoriteColor()
{
    UIController.clearConsole();
    UIController.placeCursor(topmostRow, leftmostColumn);
    std::cout << "---------------- Color Select ---------------------";
    UIController.shiftCursorFromLastSetPos(1, 0);
    std::cout << "Input your favorite color's corresponding number: ";
    COORD inputPos = UIController.getCursorPosition();
    
    for (unsigned int i = 0; i < colors.size(); i++)
    {
        UIController.shiftCursorFromLastSetPos(1, 0);
        std::cout << i << " - " << colors[i];
    }

    UIController.shiftCursorFromLastSetPos(2, 0);
    COORD validationMessagePos = UIController.getLastSetPosition();
    while (true)
    {
        UIController.placeCursor(inputPos.Y, inputPos.X);
        UIController.clearToRightOnLine();
        std::string selection = getUserInput();

        unsigned int selectionNum = std::stoi(selection);
        if (selectionNum >= 0 && selectionNum < colors.size())
        {
            UIController.placeCursor(validationMessagePos.Y, validationMessagePos.X);
            UIController.clearToRightOnLine();
            std::cout << "What a lovely color (:";
            Sleep(750);
            UIController.clearConsole();
            return colors[selectionNum];
        }

        UIController.placeCursor(validationMessagePos.Y, validationMessagePos.X);
        UIController.clearToRightOnLine();
        std::cout << "Not a color number. Please try again";
    }
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

    return true;
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
            profile = createNewProfile(usernamePasswordMap, usernamePasswordToProfileMap);
        }              
    }
    else
    {
        profile = createNewProfile(usernamePasswordMap, usernamePasswordToProfileMap);
    }

    // Logged in. Modify profile prompt

    // Log out option
           

}

   

