/*
Peter Doria

                        Login Registration System


=====================
INSTRUCTIONS

1. Stores user credentials (username, password)
    - all user infos stored in csv. Example:
        "data, data, data, \n"
        "data, data, data, \n"
    - usernames min 4 characters, max 16 characters
    - password min 8 characters, max 32 characters
2. Registers new users with new username / password
    - verifies unique username during creation
    - communicates password strength while password is being typed *** TODO
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

struct result
{
    bool isValid = false;
    std::string validationMessage;
    result(bool _isValid, std::string _validationMessage)
    {
        validationMessage = _validationMessage;
        isValid = _isValid;
    }
};

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
    std::string getLastProfileItem() { return userData[userData.size() - 1]; }

    result setUsername(std::string pWord, std::string newName)
    {
        if (pWord != getPassword())
        {
            return result(false, "Failed to reset username. Incorrect Password.");
        }

        userData[0] = newName;
        return result(true, "Username reset successful.");
    }
    result setPassword(std::string uName, std::string newPword)
    {
        if (uName != getUsername())
        {
            return result(false, "Failed to reset password. Incorrect Username key.");
        }

        userData[1] = newPword;
        return result(true, "Password reset successful");
    }
    result setFavoriteColor(std::string pWord, std::string newColor)
    {
        if (pWord != getPassword())
        {
            return result(false, "Failed to set new color. Incorrect password key");
        }

        userData[2] = newColor;
        return result(true, "Set new favorite color successfully.");
    }
};

bool loadAllUserData(std::unordered_map<std::string, std::string>& usernamePasswordMap, std::unordered_map<std::string, userProfile*>& profileMap);
bool loginOrCreateAccountPrompt();
userProfile* login(std::unordered_map<std::string, std::string>& usernamePasswordMap, std::unordered_map<std::string, userProfile*>& profileMap);
userProfile* logout();
userProfile* createNewProfile(std::unordered_map<std::string, std::string>& usernamePasswordMap, std::unordered_map<std::string, userProfile*>& profileMap);
result validateUsername(std::string, std::unordered_map<std::string, std::string>& usernamePasswordMap);
result validatePassword(std::string);
std::string SelectFavoriteColor();
result writeNewProfileToCSV(userProfile* profile);
result writeModifiedProfileToCSV(userProfile* p, userProfile* newP);
void modifyProfile(userProfile* p, std::unordered_map<std::string, std::string>& usernamePasswordMap, std::unordered_map<std::string, userProfile*>& profileMap);
void loggedInMenu(userProfile* p, std::unordered_map<std::string, std::string> usernamePasswordMap, std::unordered_map<std::string, userProfile*> usernamePasswordToProfileMap);
void quit();

std::string getUserInput();
bool checkGlobalInputCommands(std::string input);
bool isYes(std::string yesOrNo);

static outputController UI;
static std::unordered_set<std::string> validLoginOrCreateStrings{"L", "l", "C", "c"};
static std::unordered_set<std::string> quitCommandString{ ":q", "Q" };
static std::unordered_set<std::string> validYesOrNoStrings{ "Y", "y", "n", "N"};
static std::unordered_set<std::string> validReturnToPreviousMenuStrings{"B", "b"};
static std::unordered_set<std::string> illegalUsernamePasswordChars{",", "<", "'", "(", ")"," ", ";", ":"};
static std::vector<std::string> colors{ "Red", "Yellow", "Blue", "Green", "Orange", "Purple" };
static unsigned int minUsernameLength = 4;
static unsigned int maxUsernameLength = 16;
static unsigned int minPasswordLength = 8;
static unsigned int maxPasswordLength = 32;

static unsigned int leftmostColumn = 30;
static unsigned int topmostRow = 5;
static COORD menuTop;

bool loadAllUserData(std::unordered_map<std::string, std::string>& usernamePasswordMap, std::unordered_map<std::string, userProfile*>& profileMap)
{
    std::ifstream userProfileCSV;
    userProfileCSV.open("userProfiles.csv");
    if (!userProfileCSV.is_open())
    {
        assert(("Failed to open CSV", false));
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
    UI.placeCursor(5, 30);
    std::cout << "============== Login / Create Account ==============";

    UI.shiftCursorFromLastSetPos(2, 0);
    std::cout << "Enter ':q' to quit the application at any time.";
    Sleep(5);

    UI.shiftCursorFromLastSetPos(2, 0);
    std::cout << "To Login, press 'L', then press 'Enter'\n";
    Sleep(5);

    UI.shiftCursorFromLastSetPos(2, 0);
    std::cout << "To Create a new account, press 'C', then press 'Enter'";
    Sleep(5);

    UI.shiftCursorFromLastSetPos(2, 0);
    std::string loginOrCreateInput;
    loginOrCreateInput = getUserInput();
    while (validLoginOrCreateStrings.count(loginOrCreateInput) == 0)
    {
        checkGlobalInputCommands(loginOrCreateInput);
        UI.shiftCursorFromLastSetPos(2, 0);
        std::cout << loginOrCreateInput + " is not an option.";
        Sleep(1000);
        std::cout << " Please Try Again .";
        Sleep(500);
        for (int i = 0; i < 5; i++)
        {
            Sleep(500);
            std::cout << " . ";
        }
        UI.clearLine();
        UI.shiftCursorFromLastSetPos(-2, 0);
        UI.clearLine();
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
    COORD messagePos;
    int maxLoginAttempts = 3;
    int loginAttempts = 0;

    UI.clearConsole();
    std::cout << std::endl;
    UI.placeCursor(topmostRow, leftmostColumn);
    int LoginMenuTop = UI.getCursorPosition().Y;
    std::cout << "=========== LOGIN MENU ===========";
    UI.shiftCursorFromLastSetPos(2, 0);
    std::cout << "Username: ";
    usernamePos = UI.getCursorPosition();
    UI.shiftCursorFromLastSetPos(1, 0);
    std::cout << "Password: ";
    passwordPos = UI.getCursorPosition();
    messagePos = UI.shiftCursorFromLastSetPos(2,0);

    while (loginAttempts < maxLoginAttempts)
    {
        loginAttempts++;
        UI.placeCursor(usernamePos.Y, usernamePos.X);
        usernameInput = getUserInput();
        UI.placeCursor(passwordPos.Y, passwordPos.X);
        passwordInput = getUserInput();

        if (usernamePasswordMap.count(usernameInput) == 1 && usernamePasswordMap.at(usernameInput) == passwordInput)
        {
            if (profileMap.count(usernameInput + passwordInput) == 1)
            {
                UI.shiftCursorFromLastSetPos(2,0);
                COORD successPos = UI.getCursorPosition();
                bool isBlank = false;
                for (unsigned int i = 0; i < 200; i++)
                {
                    if (i % 30 == 0)
                    {
                        if (isBlank == false)
                        {
                            UI.placeCursor(successPos.Y, successPos.X);
                            std::cout << "Success! Profile found!";
                            isBlank = true;
                        }
                        else
                        {
                            UI.clearLine();
                            isBlank = false;
                        }
                    }
                    Sleep(1);
                }
                UI.clearConsole();

                return profileMap.at(usernameInput + passwordInput);
            }
            assert(("Username password combo is valid but could not find matching profile for username password combo", false));
        }

        std::string createAccountOrEnterAgain = "";
        UI.placeCursor(messagePos.Y, messagePos.X);

        std::cout << "Username or Password not found. Attempt to login again or create a new account?"; 
        while (validYesOrNoStrings.count(createAccountOrEnterAgain) == 0)
        {
            UI.shiftCursorFromLastSetPos(2, 0);
            UI.clearLine();
            std::cout << "Enter 'y' to create a new account. Enter 'n' to enter a username/password, again.";
            
            UI.shiftCursorFromLastSetPos(1, 0);
            UI.clearLine();
            std::cout << "'y' or 'n': ";
            createAccountOrEnterAgain = getUserInput() ;
            
            UI.shiftCursorFromLastSetPos(-3, 0);
        }

        if (isYes(createAccountOrEnterAgain))
        {
            return createNewProfile(usernamePasswordMap, profileMap);
        }        
        else
        {
            UI.clearToRightOnLineFromPos(usernamePos);
            UI.clearToRightOnLineFromPos(passwordPos);
        }
    }

    if (loginAttempts >= maxLoginAttempts)
    {
        UI.clearConsole();
        UI.placeCursor(7, 30);
        std::cout << "Username and password combination not found.";
        UI.shiftCursorFromLastSetPos(2, 0);
        std::cout << "You have exceeded your maximum login attempts. Please try again later, or create a new account. ";
        Sleep(10);
        UI.clearConsole();
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

    UI.clearConsole();
    UI.placeCursor(topmostRow, leftmostColumn);
    std::cout << "============== Create a Profile ==============";
    UI.shiftCursorFromLastSetPos(2, 0);
    std::cout << "Username must be between " << minUsernameLength << " and " << maxUsernameLength << " characters.";
    UI.shiftCursorFromLastSetPos(1, 0);
    std::cout << "Password must be between " << minPasswordLength << " and " << maxPasswordLength << " characters.";
    UI.shiftCursorFromLastSetPos(1, 0);
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
    

    UI.shiftCursorFromLastSetPos(2, 0);
    std::cout << "New Username: ";
    usernameInputPos = UI.getCursorPosition();
    
    UI.shiftCursorFromLastSetPos(1, 0);
    std::cout << "New Password: ";
    passwordInputPos = UI.getCursorPosition();
    
    validationMessagePos = UI.CalculateCOORDValueAsIfShifted(UI.getLastSetPosition(),1, 0);

    unsigned int attempts = 0;
    std::string usernameInput;
    while (true)
    {
        attempts++;
        UI.placeCursor(usernameInputPos.Y, usernameInputPos.X);
        UI.clearToRightOnLine();
        usernameInput = getUserInput();
    
        result validation = validateUsername(usernameInput, usernamePasswordMap);
        UI.placeCursor(validationMessagePos.Y, validationMessagePos.X);
        UI.clearToRightOnLine();
        std::cout << validation.validationMessage;

        if (validation.isValid)
        {
            break;
        }

        if (attempts > 6)
        {
            UI.clearConsole();
            UI.placeCursor(validationMessagePos.Y, validationMessagePos.X);
            UI.clearToRightOnLine();
            std::cout << "You have exceeded maximum username input attempts. Please try creating an account later ";
            for (unsigned int i = 0; i < 1000; i++)
            {
                if (i % 200)
                {
                    std::cout << ". ";
                }
            }
            UI.clearConsole();
            return NULL;
        }
    }

    attempts = 0;
    std::string passwordInput;
    while (true)
    {
        attempts++;
        UI.placeCursor(passwordInputPos.Y, passwordInputPos.X);
        UI.clearToRightOnLine();
        passwordInput = getUserInput();

        result validation = validatePassword(passwordInput);
        UI.placeCursor(validationMessagePos.Y, validationMessagePos.X);
        UI.clearToRightOnLine();
        std::cout << validation.validationMessage;

        if (validation.isValid)
        {
            break;
        }

        if (attempts > 6)
        {
            UI.clearConsole();
            UI.placeCursor(validationMessagePos.Y, validationMessagePos.X);
            UI.clearToRightOnLine();
            std::cout << "You have exceeded maximum password input attempts. Please try creating an account later ";
            for (unsigned int i = 0; i < 1000; i++)
            {
                if (i % 200)
                {
                    std::cout << ". ";
                }
            }
            UI.clearConsole();
            return NULL;
        }
    }
    
    std::string color = SelectFavoriteColor();

    usernamePasswordMap.insert(std::make_pair(usernameInput, passwordInput));
    profile = new userProfile(usernameInput, passwordInput, color);
    profileMap.insert(std::make_pair(usernameInput + passwordInput, profile));

    writeNewProfileToCSV(profile);

    return profile;
}

result validateUsername(std::string username, std::unordered_map<std::string, std::string>& usernamePasswordMap)
{
    bool isValid = true;
    std::string message;
    if (usernamePasswordMap.count(username) > 0)
    {
        isValid = false;
        message += "Username already exists. ";
        return result(isValid, message);
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

    return result(isValid, message);
}

result validatePassword(std::string password)
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

    return result(valid, message);
}

std::string SelectFavoriteColor()
{
    UI.clearConsole();
    UI.placeCursor(topmostRow, leftmostColumn);
    std::cout << "---------------- Color Select ---------------------";
    UI.shiftCursorFromLastSetPos(1, 0);
    std::cout << "Input your favorite color's corresponding number: ";
    COORD inputPos = UI.getCursorPosition();
    
    for (unsigned int i = 0; i < colors.size(); i++)
    {
        UI.shiftCursorFromLastSetPos(1, 0);
        std::cout << i << " - " << colors[i];
    }

    UI.shiftCursorFromLastSetPos(2, 0);
    COORD validationMessagePos = UI.getLastSetPosition();
    while (true)
    {
        UI.placeCursor(inputPos.Y, inputPos.X);
        UI.clearToRightOnLine();
        std::string selection = getUserInput();

        unsigned int selectionNum = std::stoi(selection);
        if (selectionNum >= 0 && selectionNum < colors.size())
        {
            UI.placeCursor(validationMessagePos.Y, validationMessagePos.X);
            UI.clearToRightOnLine();
            std::cout << "What a lovely color (:";
            Sleep(750);
            UI.clearConsole();
            return colors[selectionNum];
        }

        UI.placeCursor(validationMessagePos.Y, validationMessagePos.X);
        UI.clearToRightOnLine();
        std::cout << "Not a color number. Please try again";
    }
}

result writeModifiedProfileToCSV(userProfile* p, userProfile* modP)
{
    std::ifstream in("userProfiles.csv");
    if (!in.is_open())
    {
        assert(("CSV failed to open", false));
        return result(false, "CSV failed to open");
    }
    std::string s = "";
    std::string firstPartition = "";
    std::string secondPartition = "";
    std::streampos startPos;
    while (in >> s)
    {
        if (s != p->getUsername())
        {
            firstPartition += s;
            continue;
        }
        break;
    }
    for (char c : s)
    {
        in.unget();
    }
    startPos = in.tellg();
    
    while (in >> s)
    {
        if (s == p->getLastProfileItem())
        {
            break;
        }
    }
    while (in >> s)
    {
        secondPartition += s;
    }
    in.close();
    
    std::ofstream out("userProfiles.csv");
    if (!out.is_open())
    {
        assert(("CSV failed to open", false));
        return result(false, "CSV");
    }
    out.seekp(startPos);
    out << firstPartition;
    out << modP->getUsername() << ", "
        << modP->getPassword() << ", "
        << modP->getFavoriteColor() << '\n';
    out << secondPartition;

    out.close();
    return result(true, "Success editing profile");
}

result writeNewProfileToCSV(userProfile* profile)
{
    std::ofstream outputFile("userProfiles.csv", std::ios::app);
    
    if (!outputFile.is_open())
    {
        assert(("Failed to open CSV file", false));
        return result (false, "Failed to open CSV file");
    }

    outputFile << 
        profile->getUsername() << "," << 
        profile->getPassword() << "," << 
        profile->getFavoriteColor() << '\n';

    outputFile.close();

    return result (true, "Success");
}

void quit()
{
    UI.clearConsole();
    UI.placeCursor(15, 30);
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

void loggedInMenu(userProfile * p, std::unordered_map<std::string, std::string> usernamePasswordMap, std::unordered_map<std::string, userProfile*> usernamePasswordToProfileMap)
{
    UI.clearConsole();
    UI.placeCursor(topmostRow, leftmostColumn);
    std::cout << "================= MAIN MENU ====================";
    UI.shiftCursorFromLastSetPos(1, 0);
    std::cout << "Edit Profile - 0";
    UI.shiftCursorFromLastSetPos(1, 0);
    std::cout << "Logout - 1";
    UI.shiftCursorFromLastSetPos(1, 0);
    std::cout << "Quit - :q";

    UI.shiftCursorFromLastSetPos(2, 0);
    std::cout << "Input your desired option's corresponding number-: ";
    std::string input = getUserInput();
    if (input == "0")
    {
        modifyProfile(p, usernamePasswordMap, usernamePasswordToProfileMap);
    }
    else if (input == "1")
    {
        UI.clearConsole();
        return;
    }
}

void modifyProfile(userProfile *p, std::unordered_map<std::string, std::string>& usernamePasswordMap, std::unordered_map<std::string, userProfile*>& profileMap)
{
    userProfile* newP = new userProfile(p->getUsername(), p->getPassword(), p->getFavoriteColor());
    UI.clearConsole();

    UI.placeCursor(topmostRow, leftmostColumn);
    std::cout << "========== Edit Profile Menu ==========";
    UI.shiftCursorFromLastSetPos(1, 0);
    std::cout << "Change username - 0";
    UI.shiftCursorFromLastSetPos(1, 0);
    std::cout << "Change password - 1";
    UI.shiftCursorFromLastSetPos(1, 0);
    std::cout << "Change favorite color - 2";
    UI.shiftCursorFromLastSetPos(1, 0);
    std::cout << "Return to Logged-In Menu - ";
    for (auto i = validReturnToPreviousMenuStrings.begin(); i != validReturnToPreviousMenuStrings.end(); i++)
    {
        std::cout << *i;
        if (std::next(i) != validReturnToPreviousMenuStrings.end())
        {
            std::cout << " or ";
        }
    }
    UI.shiftCursorFromLastSetPos(2, 0);
    std::cout << "Input the corresponding number or character for your desired option: ";
    
    COORD editChoicePos = UI.getCursorPosition();
    COORD modifyItemPos = UI.CalculateCOORDValueAsIfShifted(UI.getLastSetPosition(), 3, 0);
    COORD validationMessagePos = UI.CalculateCOORDValueAsIfShifted(UI.getLastSetPosition(), 1, 0);
    
    UI.shiftCursorFromLastSetPos(8, 0); 
    std::cout << "-----------------------------------------------------------------------";

    
    while (true)
    {
        UI.placeCursor(editChoicePos.Y, editChoicePos.X);
        std::string choice;
        std::cin >> choice;

        if (choice == "0")
        {
            unsigned int attempts = 0;
            std::string usernameInput; 
            while (true)
            {
                attempts++;
                UI.placeCursor(modifyItemPos.Y, modifyItemPos.X);
                std::cout << "Input new username: ";
                UI.clearToRightOnLine();
                usernameInput = getUserInput();

                result validation = validateUsername(usernameInput, usernamePasswordMap);
                UI.placeCursor(validationMessagePos.Y, validationMessagePos.X);
                UI.clearToRightOnLine();
                std::cout << validation.validationMessage;

                if (validation.isValid)
                {
                    result result = newP->setUsername(newP->getPassword(), usernameInput);
                    std::cout << ". " << result.validationMessage;
                    if (result.isValid == false)
                    {
                        assert((result.validationMessage, false));
                    }
                    break;
                }

                if (attempts > 6)
                {
                    UI.clearConsole();
                    UI.placeCursor(validationMessagePos.Y, validationMessagePos.X);
                    UI.clearToRightOnLine();
                    std::cout << "You have exceeded maximum username edit attempts. Please try again later.";
                    for (unsigned int i = 0; i < 1000; i++)
                    {
                        if (i % 200)
                        {
                            std::cout << ". ";
                        }
                    }
                    UI.clearConsole();
                    break;
                }
            }
        }
        else if (choice == "1")
        {
            int attempts = 0;
            std::string passwordInput;
            while (true)
            {
                attempts++;
                UI.placeCursor(modifyItemPos.Y, modifyItemPos.X);
                UI.clearToRightOnLine();
                std::cout << "New Password: ";
                passwordInput = getUserInput();

                result validation = validatePassword(passwordInput);
                UI.placeCursor(validationMessagePos.Y, validationMessagePos.X);
                UI.clearToRightOnLine();
                std::cout << validation.validationMessage;

                if (validation.isValid)
                {
                    result result = newP->setPassword(newP->getUsername(), passwordInput);
                    std::cout << ". " << result.validationMessage;
                    if (result.isValid == false)
                    {
                        assert((result.validationMessage, false));
                    }
                    break;
                }

                if (attempts > 6)
                {
                    UI.clearConsole();
                    UI.placeCursor(validationMessagePos.Y, validationMessagePos.X);
                    UI.clearToRightOnLine();
                    std::cout << "You have exceeded maximum password input attempts. Please try creating an account later ";
                    for (unsigned int i = 0; i < 1000; i++)
                    {
                        if (i % 200)
                        {
                            std::cout << ". ";
                        }
                    }
                    UI.clearConsole();
                    break;
                }
            }
        }
        else if (choice == "2")
        {
            std::string c = SelectFavoriteColor();
            result v = newP->setFavoriteColor(newP->getPassword(), c);
            UI.placeCursor(validationMessagePos.Y, validationMessagePos.X);
            UI.clearToRightOnLine();
            std::cout << v.validationMessage;
            if (v.isValid == false)
            {
                assert((v.validationMessage, false));
            }
            break;
        }
        else if (validReturnToPreviousMenuStrings.count(choice) > 0)
        {
            break;
        }
        else 
        {
            UI.placeCursor(validationMessagePos.Y, validationMessagePos.X);
            std::cout << "Invalid input is not an option";
        }

        UI.clearToRightOnLineFromPos(editChoicePos);
        UI.clearToRightOnLineFromPos(modifyItemPos);
    }

    writeModifiedProfileToCSV(p, newP);
    p = newP;
    
    return;
}


int main()
{
    std::unordered_map<std::string, std::string> usernamePasswordMap;
    std::unordered_map<std::string, userProfile*> usernamePasswordToProfileMap;
    loadAllUserData(usernamePasswordMap, usernamePasswordToProfileMap);

    while (true)
    {
        bool choseToLogin = loginOrCreateAccountPrompt();
        userProfile* p;
        if (choseToLogin)
        {
            p = login(usernamePasswordMap, usernamePasswordToProfileMap);
            if (p == NULL)
            {
                p = createNewProfile(usernamePasswordMap, usernamePasswordToProfileMap);
            }
        }
        else
        {
            p = createNewProfile(usernamePasswordMap, usernamePasswordToProfileMap);
        }

        loggedInMenu(p, usernamePasswordMap, usernamePasswordToProfileMap);
        
    }

    // Log out option
           

}

   

