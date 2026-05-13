#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <limits>
#include <iomanip>
using namespace std;

const string ClientsFileName = "Clients.txt" ; 
const string UsersFileName = "Users.txt" ; 

struct stClient 
{
    string AccountNumber ; 
    string PinCode ; 
    string Name ; 
    string Phone ; 
    double AccountBalance ; 
    bool IsMarkedForDelete = false ; 
};

struct stUser 
{
    string Username ; 
    string Password ; 
    int Permissions ; 
    bool IsForDelete  = false  ;
};

stUser CurrentUser; 

enum enMainMenueOptions
{
    eClientsList = 1,
    eAddNewClient = 2,
    eDeleteClient = 3,
    eUpdateClient = 4,
    eFindClient = 5,
    eTransactions = 6,
    eManageUsers = 7,
    eExit = 8
};

enum enMainMenuePermissions
{
    eAll = -1,
    pListClients = 1,
    pAddNewClient = 2,
    pDeleteClient = 4,
    pUpdateClient = 8,
    pFindClient = 16,
    pTransactions = 32,
    pManageUsers = 64

    // we made each permission as a bit in the binary representation of the permissions number, so we can combine permissions by adding their values and check if a user has a specific permission by using bitwise AND operation.
    // each bit represents a specific permission, for example:
    // pListClients = 1 (binary 00000001)
    // pAddNewClient = 2 (binary 00000010)
    // pDeleteClient = 4 (binary 00000100)
};

enum enManageUsersMenuOptions
{
    eListUsers = 1, eAddNewUser = 2 , eDeleteUser = 3 , eUpdateUser = 4, eFindUser = 5 , eMainMenu =6
};

enMainMenueOptions ReadUserMainMenueOption()
{
    short c = 0 ; 
    cout << "\nChoose What to you want to do ? [1 to 8] " ; 
    cin >> c ;

    return enMainMenueOptions(c);
}

enum enTransactionsOptions 
{
    eDeposit = 1 , 
    eWithdraw=2 , 
    eTotalBalances=3,
    eShowMainMenue=4
} ;

bool CheckAccessPermission (enMainMenuePermissions ) ; 
vector <stUser> LoadUsersDataFromFile (string );
void ShowManageusersScreen();
void ShowMainMenu();
void Login() ; 
void GoBackToMainMenu ()
{
    cout << "\nPlease enter any key to go back to main menu ........";
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    cin.get();
    ShowMainMenu() ; 
}

vector<string> SplitString(string S1, const string &Delim)
{
    vector<string> vString;
    size_t pos = 0;
    string sWord;

    while ((pos = S1.find(Delim)) != string::npos)
    {
        sWord = S1.substr(0, pos);
        if (!sWord.empty())
        {
            vString.push_back(sWord);
        }
        S1.erase(0, pos + Delim.length());
    }

    if (!S1.empty())
    {
        vString.push_back(S1);
    }

    return vString;
}

stClient ConvertLineToRecord (string Line , string Seperator = "#//#")
{
    vector<string> vClients = SplitString(Line,Seperator) ; 

    stClient Client ;

    Client.AccountNumber = vClients[0];
    Client.PinCode = vClients[1];
    Client.Name = vClients[2];
    Client.Phone = vClients[3];
    Client.AccountBalance = stod(vClients[4]);

    return Client ; 
}

vector<stClient> LoadDataFromFile(string FileName)
{
    fstream MyFile ; 
    MyFile.open(FileName,ios::in) ; 

    stClient Client ; 
    string Line; 
    vector<stClient> vClients ;

    if (MyFile.is_open())
    {
        while (getline(MyFile,Line))
        {
            Client = ConvertLineToRecord(Line) ; 
            vClients.push_back(Client) ; 
        }

        MyFile.close()  ;

    }

    return vClients ; 

}

void PrintClientRecord (stClient Client )
{
    cout << "| " << left << setw(15) << Client.AccountNumber ; 
    cout << "| " << left << setw(10) <<  Client.PinCode;
    cout << "| " << left << setw(40) << Client.Name;
    cout << "| " << left << setw(12) << Client.Phone;
    cout << "| " << left << setw(12) << Client.AccountBalance ;
}

void ShowAccessDeniedMessage()
{
    system ("clear") ; 
    cout << "------------------------------------\n" ; 
    cout << "Access Denied,\n" ; 
    cout <<"You don't have permission to do this,\n"; 
    cout << "Please Contact Your Admin.\n";
    cout << "------------------------------------\n" ; 
    
}

void ShowClientsList()
{
    if (!CheckAccessPermission(enMainMenuePermissions::pListClients))
    {
        ShowAccessDeniedMessage() ;
        // GoBackToMainMenu();
        return;
    }
   

    vector<stClient> vClients = LoadDataFromFile(ClientsFileName) ; 
    
    cout << "\t\t\tClients List (" << vClients.size() << ") Client(s).\n" ; 
    cout <<"_______________________________________________________________________________________________\n" ;
    cout << "| " << left << setw(15) << "Account Number" ; 
    cout << "| " << left << setw(10) << "Pin Code" ; 
    cout << "| " << left << setw(40) << "Client Name" ;
    cout << "| " << left << setw(12) << "Phone" ;
    cout << "| " << left << setw(12) << "Balance" ; 
    cout <<"\n_______________________________________________________________________________________________\n" ;

    if (vClients.size() == 0 )
    {
        cout << "\n\t\t\tNo Clients Available In System!" ; 
    }
    else 
    {
        for (stClient &C : vClients)
        {
            PrintClientRecord(C) ;
            cout << endl;
        }
    }
}

string ConvertRecordToLine (stClient Client , string Seperator = "#//#")
{
    string sLine = "" ; 
    
    sLine += Client.AccountNumber + Seperator ;
    sLine += Client.PinCode + Seperator; 
    sLine += Client.Name + Seperator;
    sLine += Client.Phone + Seperator ;
    sLine +=  to_string(Client.AccountBalance) ;

    return sLine ; 
    
}

bool IsAccountNumberExistInFile (string AccountNumber , string FileName)
{
   fstream MyFile ;
   MyFile.open(FileName,ios::in); 

   if (MyFile.is_open())
   {
       string Dataline;
       stClient client;


       while (getline(MyFile,Dataline))
       {
         client = ConvertLineToRecord(Dataline) ;

         if (client.AccountNumber == AccountNumber)
         {
             MyFile.close() ; 
             return true; 
         }


       }

       MyFile.close() ; 
   }

    return false ; 
}

void AddClientToFile (stClient Client , string FileName)
{
    fstream MyFile ; 
    MyFile.open(FileName,ios::out|ios::app) ; 
    if (MyFile.is_open()) 
    {
        MyFile << ConvertRecordToLine(Client) << endl ; 

        MyFile.close() ; 
    }
}

stClient ReadNewClient ()
{
    stClient Client ;

    cout << "Enter Account Number ? ";
    getline(cin >> ws, Client.AccountNumber);

    while (IsAccountNumberExistInFile(Client.AccountNumber,ClientsFileName))
    {
        cout << "\nSorry you can't add this client beacause it is already exists, Please Enter Another one ? ";
        getline(cin >> ws, Client.AccountNumber);
    }
    cout << "\nEnter Pin Code ? ";
    getline(cin, Client.PinCode);
    cout << "\nEnter Name ? ";
    getline(cin, Client.Name);
    cout << "\nEnter Phone ? ";
    getline(cin, Client.Phone);
    cout << "\nEnter Account Balance ? ";
    cin >> Client.AccountBalance;


    return Client ; 
}

void AddNewClient () 
{
    stClient Client = ReadNewClient();

    AddClientToFile(Client,ClientsFileName ) ; 
  

}

void AddNewClients ()
{

    char AddMore = 'n';

    do
    {
        AddNewClient();
        cout << "\nClient Added Succesfully , Do you want to add more clients ? [Y/N] ";
        cin >> AddMore;
        system("clear") ; 
    } while (toupper(AddMore) == 'Y');
}

void ShowAddNewClientsScreen()
{
    if (!CheckAccessPermission(enMainMenuePermissions::pAddNewClient))
    {
        ShowAccessDeniedMessage() ;
        // GoBackToMainMenu() ; 
        return ; 
    }
    cout << "\n---------------------------------------\n" ;
    cout << "\t Add New Client Screen\n" ;
    cout << "\n---------------------------------------\n";
    cout << "Adding New Client:\n\n" ; 

    AddNewClients() ; 

    
}

string ReadClientAccountNumber()
{
    string AccountNumber ; 
    cout << "\nPlease Enter Account Number? " ; 
    cin >> AccountNumber ;

    return AccountNumber ; 
}

bool FindClientByAccountNumber (vector<stClient> &vClients,string AccountNumber , stClient& Client)
{
   

    for (stClient & C : vClients)
    {
        if (C.AccountNumber == AccountNumber)
        {
            Client = C ; 
            return true; 
        }
    }


    return false ; 
}

void PrintClientCard(stClient Client)
{
    cout << "The Following are the client details : \n" ;
    cout << "____________________________________________\n";
    cout << "Account Number  : " << Client.AccountNumber << endl ; 
    cout << "Pin Code        : " << Client.PinCode << endl ; 
    cout << "Name            : " << Client.Name << endl ; 
    cout << "Phone           : " << Client.Phone << endl ; 
    cout << "Account Balance : " << Client.AccountBalance << endl ;
    cout << "____________________________________________\n";
}

void MarkClientForDelete (string AccountNumber , vector<stClient>& vClients)
{
    for (stClient & C : vClients)
    {
        if (C.AccountNumber == AccountNumber)
        {
            C.IsMarkedForDelete= true; 
            break ;
        }
    }
}

void SaveVectorToFile (string FileName ,vector<stClient> &vClients)
{
    fstream MyFile ;

    MyFile.open(FileName, ios::out) ; 

    if (MyFile.is_open())
    {
        for (  stClient & C : vClients)
        {
            if (C.IsMarkedForDelete == false )
            {
                MyFile << ConvertRecordToLine(C) << endl ;
            }
        }
    }
}

void DeleteClientByAccountNumber (string AccountNumber , vector<stClient> &vClients)
{
    stClient Client ; 

    char answer = 'n' ; 

    if (FindClientByAccountNumber(vClients,AccountNumber,Client))
    {
        PrintClientCard(Client) ; 

        cout << "\nAre you sure you want delete this client ? [Y/N] " ; 
        cin >> answer ; 

        if (toupper(answer) == 'Y')
        {
            MarkClientForDelete(AccountNumber,vClients) ;
            SaveVectorToFile(ClientsFileName, vClients);
            vClients = LoadDataFromFile(ClientsFileName) ; 
            
            cout << "\n\nClient Deleted Succesfully.\n" ; 
            return ; 
        }
        else 
        {
            cout << "\nDelete Canceled.\n" ; 
            return ;
        }

    }
    else 
    {
        cout << "\nClient With Account Number ("  << AccountNumber << ") Is Not Found!\n" ; 
        

    }

}

void ShowDeleteClientScreen ()
{
    if (!CheckAccessPermission(enMainMenuePermissions::pDeleteClient))
    {
        ShowAccessDeniedMessage() ;
        // GoBackToMainMenu() ; 
        return ; 
    }
    cout << "\n---------------------------------------\n" ;
    cout << "\t Delete Client Screen\n" ;
    cout << "\n---------------------------------------\n";

    string AccountNumber = ReadClientAccountNumber();
    vector<stClient> vClients = LoadDataFromFile(ClientsFileName);

    DeleteClientByAccountNumber(AccountNumber,vClients) ; 

}

stClient ChangeClientRecord (string AccountNumber)
{
    stClient Client ;
    Client.AccountNumber = AccountNumber;

    cout << "\nEnter Pin Code ? ";
    getline(cin>>ws, Client.PinCode);
    cout << "\nEnter Name ? ";
    getline(cin, Client.Name);
    cout << "\nEnter Phone ? ";
    getline(cin, Client.Phone);
    cout << "\nEnter Account Balance ? ";
    cin >> Client.AccountBalance;

    return Client;
}

void UpdateClientByAccountNumber (string AccountNumber,vector<stClient>&vClients)
{
    stClient Client ; 
    char answer  ='n' ; 

    if (FindClientByAccountNumber(vClients,AccountNumber,Client))
    {
        PrintClientCard(Client) ; 
        cout << "\nAre you sure you want update this client? [Y/N] "; 
        cin >> answer ; 

        if (toupper(answer) == 'Y')
        {
            for (stClient & C : vClients)
            {
                if (C.AccountNumber == AccountNumber)
                {
                    C = ChangeClientRecord(AccountNumber) ; 
                    break ; 
                }
            }
            SaveVectorToFile(ClientsFileName,vClients) ; 

            cout << "\n\nClient Updated Succesfully .\n" ;
            return ; 
        }
        else 
        {
            cout << "\nUpdate Canceled.\n" ; 
        }

    }
    else 
    {
        cout << "\nClient With Account Number ("  << AccountNumber << ") Is Not Found!\n" ; 
        return ; 
    }
}

void ShowUpdateClientScreen ()
{
    if (!CheckAccessPermission(enMainMenuePermissions::pUpdateClient))
    {
        ShowAccessDeniedMessage() ;
        //GoBackToMainMenu() ; 
        return ; 
    }
    cout << "--------------------------------------\n" ;
    cout << "\tUpdate Client Screen\n";
    cout << "--------------------------------------\n";

    string AccountNumber = ReadClientAccountNumber() ; 
    vector<stClient> vClients   = LoadDataFromFile(ClientsFileName)  ;

    UpdateClientByAccountNumber(AccountNumber,vClients) ; 
}

void ShowTransactionsMenu();

void ShowFindClientScreen()
{
    if (!CheckAccessPermission(enMainMenuePermissions::pFindClient))
    {
        ShowAccessDeniedMessage() ;
        //GoBackToMainMenu() ; 
        return ; 
    }
    cout << "----------------------------------\n" ; 
    cout << "\tFind Client Screen\n" ;
    cout << "----------------------------------\n";

    string AccountNumber = ReadClientAccountNumber() ; 
    vector<stClient> vClients = LoadDataFromFile(ClientsFileName) ;
    stClient Client ;

    if (FindClientByAccountNumber(vClients,AccountNumber,Client))
    {
        PrintClientCard(Client);
    }
    else
    {
        cout << "Client With Account Number (" << AccountNumber<< ") Is Not Found!\n" ; 
    }

    
}

void ShowEndScreen()
{
    cout << "==================================================\n" ; 
    cout << "\t\tProgram End :-)\n" ;
    cout << "==================================================\n";
     cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
     cin.get();
}

enTransactionsOptions ReadUserTransactionOption()
{
    short c ;
    cout << "\nChoose what do you want to do ? [1 to 4]? " ; 
    cin >> c ;

    return enTransactionsOptions(c) ; 
}

void GoBackToTransactionsMenu()
{
    cout << "\nPlease enter any key to go back to Transactions Screen........ " ;
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    cin.get();
    ShowTransactionsMenu() ; 
}

int ReadDepositAmount()
{
    int x ;
    cout<< "\nPlease enter deposit amount ? " ; 
    cin >> x ;
    return x ; 
}

bool DepositBalanceByAccountNumber(string AccountNumber, vector<stClient>&vClients, double DepositAmmount)
{
    char answer = 'n' ;
    cout << "\nAre you sure you want make this transaction? [Y/N] ";
    cin >> answer ;

     if (toupper(answer) == 'Y')
     {
         for (stClient &C : vClients)
         {
             if (C.AccountNumber == AccountNumber)
             {
                 C.AccountBalance += DepositAmmount;
                 SaveVectorToFile(ClientsFileName, vClients);
                 cout << "\nDone Successfully , New Balance is: " << C.AccountBalance << endl;

                 return true;
             }
         }

         return true;
     }
     return false; 


    }

void ShowDepositScreen()
{
    cout << "--------------------------------------\n" ; 
    cout << "\t  Deposit Screen\n" ;
    cout << "--------------------------------------\n";

    stClient Client ; 
    string AccountNumber = ReadClientAccountNumber() ; 
    vector<stClient> vClients = LoadDataFromFile(ClientsFileName) ; 

    while (!FindClientByAccountNumber(vClients,AccountNumber,Client))
    {
        cout << "\nClient wiht account number [" << AccountNumber<< "] Does Not Exist.\n" ; 
        AccountNumber = ReadClientAccountNumber() ;  
    }
    
    PrintClientCard(Client) ;

    double amount = 0;
    cout << "\nPlease Enter Deposit Amount? " ;
    cin >> amount;

    DepositBalanceByAccountNumber(AccountNumber, vClients, amount);
}

void ShowWithdrawScreen ()
{
    cout << "--------------------------------------\n";
    cout << "\t  Withdraw Screen\n"; 
    cout << "--------------------------------------\n" ; 

    string AccountNumber = ReadClientAccountNumber() ;
    vector<stClient> vClients = LoadDataFromFile(ClientsFileName);
    stClient Client ; 

    while (!FindClientByAccountNumber(vClients,AccountNumber,Client))
    {
        cout << "\nClient wiht account number [" << AccountNumber << "] Does Not Exist.\n";
        AccountNumber = ReadClientAccountNumber() ;  
    }

    PrintClientCard(Client) ; 

    double amount ; 
    cout << "\nPlease Enter Withdraw amount ? " ; 
    cin >> amount ; 

    for (stClient & C : vClients)
    {
        if (C.AccountNumber == AccountNumber)
        {
            while (amount > C.AccountBalance)
            {
                cout << "\nWithdraw amount Exceeds the balance, you can withdraw up to : " << C.AccountBalance << endl; 
                cout << "Please enter another withdraw amount ? " ; 
                cin >> amount ; 
            }

            break ; 

        }
    }

    DepositBalanceByAccountNumber(AccountNumber,vClients, amount * -1) ; 

    

    
}

void PrintBalanceRecord (stClient Client)
{
    cout << "| " << left << setw(15) << Client.AccountNumber;
    cout << "| " << left << setw(40) << Client.Name;
    cout << "| " << left << setw(12) << Client.AccountBalance;
}

void ShowTotalBalances()
{
    vector<stClient> vClients = LoadDataFromFile(ClientsFileName) ;
    cout << "\t\t\tBalances List (" << vClients.size() << ") Client(s)\n";
    cout << "________________________________________________________________________________\n\n";
    cout << "| " << left << setw(15) << "Account Number";
    cout << "| " << left << setw(40) << "Client Name";
    cout << "| " << left << setw(12) << "Balance";
    cout << "\n________________________________________________________________________________\n\n";

    if (vClients.size() == 0 )
    {
        cout << "\n\t\t\tNo Clients Available in System !\n" ; 
    }

    double TotalBalances = 0 ; 

    for (stClient &C : vClients)
    {
        PrintBalanceRecord(C);
        TotalBalances += C.AccountBalance;
        cout << endl;
    }
    cout << "________________________________________________________________________________\n";

    cout << "\n\t\t\tTotal Balances = " << TotalBalances << endl;
}

void ShowTotalBalancesScreen()
{
    ShowTotalBalances();
}

void PerformTransactionOptions (enTransactionsOptions TransactionOption)
{
    switch (TransactionOption)
    {
        case enTransactionsOptions::eDeposit:
        {
            system("clear") ; 
            ShowDepositScreen() ; 
            GoBackToTransactionsMenu() ; 
            break ;
        }

        case enTransactionsOptions::eWithdraw:
        {
            system("clear");
            ShowWithdrawScreen() ;
            GoBackToTransactionsMenu();
            break;
        }

        case enTransactionsOptions::eTotalBalances:
        {
            system("clear"); 
            ShowTotalBalancesScreen() ; 
            GoBackToTransactionsMenu();
            break;
        }

        case enTransactionsOptions::eShowMainMenue:
        {
            system("clear");
            ShowMainMenu() ;
            break;
        }
        }
}

void ShowTransactionsMenu()
{

    if (!CheckAccessPermission(enMainMenuePermissions::pTransactions))
    {
        ShowAccessDeniedMessage() ;
         GoBackToMainMenu() ; 
        return ; 
    }
    system("clear") ; 

    cout << "===============================================\n" ; 
    cout << "\t  Transactions Menu Screen\n" ;
    cout << "===============================================\n";

    cout << "\t[1] Deposit.\n" ;
    cout << "\t[2] Withdraw.\n" ;
    cout << "\t[3] Total Balances.\n" ;
    cout << "\t[4] Main Menu.\n" ;
    cout << "===============================================\n";

    PerformTransactionOptions(ReadUserTransactionOption()) ;
}

void GoBackToManageUsersScreen()
{
    cout << "\nPlease enter any key to go back to Manage Users Menu ........";
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    cin.get();
    ShowManageusersScreen();
}

enManageUsersMenuOptions ReadManageUsersMenuOption()
{
    short c = 0 ; 
    cout << "Choose what do you want to do ? [1 to 6]? " ; 
    cin >> c ;

    return enManageUsersMenuOptions(c) ; 
}

void PrintUserInfo(stUser User)
{
    cout << "| " << left << setw(30) << User.Username ; 
    cout << "| " << left << setw(10) << User.Password ; 
    cout << "| " << left << setw(5) << User.Permissions ; 
}

void ShowAllUsers()
{
    vector<stUser> vUsers = LoadUsersDataFromFile(UsersFileName) ; 

    cout <<  "\t\t\tUsers List (" << vUsers.size() << ") User(s).\n";
    cout << "________________________________________________________________________________________________\n"; 
    cout << "| " << left << setw(30) << "User Name" ; 
    cout << "| " << left << setw(10) << "Password" ; 
    cout << "| " << left << setw(5) <<  "Permissions" ;
    cout << "\n________________________________________________________________________________________________\n";

    if (vUsers.size() == 0)
    {
        cout <<  "\nThere is no users in system!\n" ; 
        return ; 
    }

    for (stUser & User : vUsers)
    {
        PrintUserInfo(User);
        cout <<  "\n" ; 
    }

    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;



     
}

void ShowListUsersScreen()
{
    ShowAllUsers();

}

string ReadUsername ()
{
    string Username ; 
    cout << "Enter Username? " ; 
    getline(cin>>ws,Username) ; 
    return Username ;
}

string ReadPassword ()
{
    string Password ;
    cout << "Enter Password? " ; 
    getline(cin>>ws , Password);
    return Password ; 
}

int ReadPermissionsToSet()
{
    int Permissions = 0 ; 

    cout << "Do you want to give full access? y/n? " ; 
    char answer ; 
    cin >> answer ;

    if (toupper(answer) == 'Y')
        return -1;

    cout <<  "\n\nDo you want to give access to : \n\n" ; 

    cout << "Show Client List? y/n? " ; 
    cin >> answer ; 

    if (toupper(answer) == 'Y')
        Permissions += enMainMenuePermissions::pListClients; 

    cout << "Add New Client? y/n? " ; 
    cin >> answer ;

    if (toupper(answer) == 'Y')
        Permissions += enMainMenuePermissions::pAddNewClient;

    cout << "Delete Client? y/n? " ; 
    cin >> answer ;

    if (toupper(answer) == 'Y')
        Permissions += enMainMenuePermissions::pDeleteClient; 

    cout << "Update Client? y/n? " ; 
    cin >> answer ;

    if (toupper(answer) == 'Y')
        Permissions+= enMainMenuePermissions::pUpdateClient; 

    cout << "Find Client ? y/n? " ; 
    cin >> answer ;

    if (toupper(answer) == 'Y')
        Permissions += enMainMenuePermissions::pFindClient;

    cout <<  "Transactions? y/n? " ; 
    cin >> answer ; 

    if (toupper(answer) == 'Y')
        Permissions += enMainMenuePermissions::pTransactions; 

    cout << "Manage Users? y/n? " ; 
    cin >>  answer ;

    if (toupper(answer) == 'Y')
        Permissions += enMainMenuePermissions::pManageUsers; 

    return Permissions;
}

bool UserExistsByUsername (string Username )
{
    vector<stUser> vUsers = LoadUsersDataFromFile(UsersFileName) ;

    for (stUser & User : vUsers)
    {
        if (User.Username == Username)
            return true;
    }

    return false ; 
}

string ConvertUserRecordToLine (stUser User , string Seperator = "#//#")
{
    string sUser = "" ; 

    sUser += User.Username + Seperator ;
    sUser += User.Password + Seperator ; 
    sUser += to_string(User.Permissions)  ; 

    return sUser ; 
}

void AddDataLineToFile (string Line , string FileName)
{
    fstream Myfile ;
    
    Myfile.open(FileName , ios ::out | ios::app) ;
    if (Myfile.is_open())
    {
        Myfile << Line << endl  ;
    }
}

stUser ReadNewUser ()
{
    stUser User ; 

    User.Username = ReadUsername() ;

    while (UserExistsByUsername(User.Username))
    {
        cout << "\nUser with [" << User.Username << "] already exists , Enter another username? ";
        getline(cin >> ws, User.Username);
    }

    User.Password = ReadPassword() ; 
    User.Permissions = ReadPermissionsToSet() ; 

    return User ; 
}

void AddNewUser()
{
    stUser User ;
    User = ReadNewUser() ; 
    AddDataLineToFile(ConvertUserRecordToLine(User),UsersFileName);

}

void AddNewUsers()
{
    char AddMore = 'y' ; 
    do 
    {
        AddNewUser() ; 
        cout <<  "\nUser Added Successfully, do you want to add more Users? Y/N? " ; 
        cin >> AddMore ; 
        
    }while (toupper(AddMore) == 'Y') ; 
}

void ShowAddNewUsersScreen()
{
    cout << "-----------------------------------\n"; 
    cout << "\t  Add New User Screen\n";
    cout << "-----------------------------------\n" ; 

    AddNewUsers() ;
}

bool FindUserByUsername (string Username , vector<stUser> vUsers , stUser & User )
{
    for (stUser & U : vUsers)
    {
        if (U.Username == Username)
        {
            User = U ; 

            return true ; 
        }
    }

    return false ; 
}

void MarkUserForDelete (string Username , vector<stUser> &vUsers)
{
    for (stUser & U : vUsers)
    {
        if (U.Username == Username)
        {
            U.IsForDelete = true ;
            break ;
        }
    }
}

void SaveUsersVectorToFile (vector <stUser> vUsers, string FileName)
{
    fstream Myfile ; 

    Myfile.open(FileName, ios ::out);

    if (Myfile.is_open())
    {
        for (stUser & U : vUsers)
        {
            if (!U.IsForDelete )
                Myfile << ConvertUserRecordToLine(U) << endl;
        }

        Myfile.close() ; 
    }
    
}

void PrintUserCard(stUser User)
{
    cout << "\n\nThe following are the user details:\n" ; 
    cout << "-----------------------------------\n" ;
    cout << "Username    : " << User.Username << endl ;
    cout << "Password    : " << User.Password << endl ;
    cout << "Permissions : " << User.Permissions << endl ;
    cout << "-----------------------------------\n" ;
    
}

void DeleteUserByUsername(string Username, vector<stUser> &vUsers)
{
    char answer  = 'y' ;
    stUser User ;

    if (Username == "Admin")
    {
        cout << "\n\nSorry You Can't Delete This User!\n\n";
        return;
    }

    if (FindUserByUsername(Username, vUsers,User))
    {
        PrintUserCard(User) ;

        cout << "\nAre you sure you want delete this user ? y/n? ";
        cin >> answer;

        if (toupper(answer) == 'Y')
        {
            MarkUserForDelete(Username, vUsers);
            SaveUsersVectorToFile(vUsers, UsersFileName);
            vUsers = LoadUsersDataFromFile(UsersFileName);

            cout << "User Deleted Successfully.\n";
            return ; 
        }

        else
            cout << "\n\nDelete Cancelled!\n\n";
    }
    else
    {
        cout << "\n\nUser with username [" << Username << "] is Not found!\n";
    }
}

void ShowDeleteUsersScreen()
{
    cout << "-----------------------------------\n" ; 
    cout << "\t  Delete Users Screen\n" ; 
    cout << "-----------------------------------\n" ; 

    string Username = ReadUsername() ; 
    vector <stUser> vUsers = LoadUsersDataFromFile(UsersFileName) ; 
    DeleteUserByUsername(Username,vUsers);
}

void UpdateUserRecord (string Username , vector <stUser>& vUsers)
{
    for (stUser & U : vUsers)
    {
        if (U.Username == Username)
        {
            U.Username = Username ;
            U.Password = ReadPassword() ; 
            U.Permissions = ReadPermissionsToSet() ;
            break ;
        }
    }
}

void UpdateUserByUsername (string Usernname , vector<stUser> & vUsers)
{
    stUser User ; 

    if (FindUserByUsername(Usernname,vUsers,User))
    {
        PrintUserCard(User); 
        
        cout << "\n\nAre you sure you want update this User? y/n? " ; 
        char answer ; 
        cin >> answer ;

        if (toupper(answer) == 'Y')
        {
            UpdateUserRecord(Usernname,vUsers) ; 
            SaveUsersVectorToFile(vUsers,UsersFileName) ;

            cout << "\n\nUser Updated Successfully.\n\n" ; 
            return; 

        }
        else
            cout << "\n\nUpdate Cancelled.\n\n";
    }
    else 
    {
        cout << "\n\nUser with username [" <<  Usernname << "] is not found!\n\n" ; 

    }
    
}

void ShowUpdateUserScreen()
{
    cout << "-----------------------------------\n";
    cout << "\t  Update Users Screen\n" ; 
    cout << "-----------------------------------\n" ;

    string Username = ReadUsername();
    vector<stUser> vUsers = LoadUsersDataFromFile(UsersFileName);
    UpdateUserByUsername(Username,vUsers);
}

void ShowFindUserScreen()
{
    cout << "-----------------------------------\n" ; 
    cout << "\t  Find User Screen\n" ; 
    cout << "-----------------------------------\n\n" ; 

    string Username = ReadUsername() ;
    stUser User ; 
    vector<stUser> vUsers = LoadUsersDataFromFile(UsersFileName) ;

    if (FindUserByUsername(Username,vUsers,User))

        PrintUserCard(User);

    else
        cout << "\n\nUser with username [" << Username << "] is not found!\n\n";
}

void PerformManageUsersMenuOption(enManageUsersMenuOptions UserOption)
{
    switch (UserOption)
    {
        case enManageUsersMenuOptions::eListUsers: 

        system("clear");
        ShowListUsersScreen() ; 
        GoBackToManageUsersScreen();
        break ; 

        case enManageUsersMenuOptions::eAddNewUser: 
        system("clear");
        ShowAddNewUsersScreen() ;
        GoBackToManageUsersScreen();
        break;

        case enManageUsersMenuOptions::eDeleteUser: 
        system("clear");
        ShowDeleteUsersScreen() ;
        GoBackToManageUsersScreen();
        break;

        case enManageUsersMenuOptions::eUpdateUser :
        system ("clear");
        ShowUpdateUserScreen() ;
        GoBackToManageUsersScreen();
        break;

        case enManageUsersMenuOptions::eFindUser : 
        system("clear");
        ShowFindUserScreen() ; 
        GoBackToManageUsersScreen();
        break;
        
        case enManageUsersMenuOptions::eMainMenu:
        ShowMainMenu() ; 
        break;
    }
}

void ShowManageusersScreen()
{
    if (!CheckAccessPermission(enMainMenuePermissions::pManageUsers))
    {
        ShowAccessDeniedMessage() ;
         GoBackToMainMenu();
        return ; 
    }
    system("clear"); 
    
    cout << "===========================================\n" ; 
    cout << "\t   Manage Users Menue Screen\n" ; 
    cout << "===========================================\n" ; 

    cout << "\t[1] List Users.\n" ; 
    cout << "\t[2] Add New User.\n";
    cout << "\t[3] Delete User.\n";
    cout << "\t[4] Update User.\n";
    cout << "\t[5] Find User.\n";
    cout << "\t[6] Main Menu.\n";
    cout << "===========================================\n" ; 
    
    PerformManageUsersMenuOption(ReadManageUsersMenuOption()); 

    
}

bool CheckAccessPermission (enMainMenuePermissions Permission)
{
    if (CurrentUser.Permissions == enMainMenuePermissions::eAll)
        return true;

    return ((Permission & CurrentUser.Permissions) == Permission) ; 

    // bitwise AND gives the common permissions between the required permission and the user permissions, if the result is equal to the required permission, it means the user has that permission.
    // example : 
    // required permission = 0010 (binary for 2)
    // user permissions = 0110 (binary for 6)
    // bitwise AND = 0010 (binary for 2)
    // since the result (0010) is equal to the required permission (0010), the user has that permission.
}

void PerformMainMenuOption (enMainMenueOptions MainMenuOption)
{
    
    switch (MainMenuOption)
    {
        case enMainMenueOptions::eClientsList : 
        {
            system("clear") ; 
            ShowClientsList() ; 
            GoBackToMainMenu() ; 
            break ;
        }

        case enMainMenueOptions::eAddNewClient:
        {
            system("clear");
            ShowAddNewClientsScreen() ;
            GoBackToMainMenu();
            break;
        }

        case enMainMenueOptions::eDeleteClient : 
        {
            system("clear");
            ShowDeleteClientScreen() ;
            GoBackToMainMenu() ; 
            break;    
        }
        
        case enMainMenueOptions::eUpdateClient:
        {
            system("clear") ; 
            ShowUpdateClientScreen() ;
            GoBackToMainMenu() ; 
            break ;
        }

        case enMainMenueOptions::eFindClient:
        {
            system("clear"); 
            ShowFindClientScreen() ; 
            GoBackToMainMenu() ; 
            break ; 
        }

        case enMainMenueOptions::eTransactions:
        {
            //system("clear") ; 
            ShowTransactionsMenu() ; 
            break ; 

        }

        case enMainMenueOptions::eManageUsers: 
        {
            ShowManageusersScreen() ; 
            break ;

        }

        case enMainMenueOptions::eExit:
        {
            system("clear") ; 
            Login() ; 
            break ; 
        }

    }

}

void ShowMainMenu()
{
    system("clear") ; 
    cout << "===============================================\n" ; 
    cout << "\t\tMain Menu Screen \n" ;
    cout << "===============================================\n";
    cout << "\t[1] Show Client List.\n";
    cout << "\t[2] Add New Client.\n";
    cout << "\t[3] Delete Client.\n" ;
    cout << "\t[4] Update Client.\n" ; 
    cout << "\t[5] Find Client.\n";
    cout << "\t[6] Transactions.\n" ;
    cout << "\t[7] Manage Users.\n";
    cout << "\t[8] Logut.\n" ;
    cout << "===============================================\n";

    PerformMainMenuOption(ReadUserMainMenueOption()) ; 
}

stUser ConvertUserLineToRecord (string Line , string seperator = "#//#")
{
    vector<string> stringUser = SplitString(Line,seperator);

    stUser User ;
    User.Username = stringUser[0];
    User.Password = stringUser[1];
    User.Permissions = stoi(stringUser[2]) ;
    return User;
}

vector <stUser> LoadUsersDataFromFile (string FileName)
{
    vector<stUser> vUsers ; 

    fstream MyFile ;
    MyFile.open(FileName,ios::in) ; 

    string Line ; 
    stUser User ; 

    if (MyFile.is_open())
    {
        while (getline(MyFile,Line))
        {
            vUsers.push_back(ConvertUserLineToRecord(Line)) ; 
        }
        MyFile.close() ;
    }

    return vUsers ; 
}

bool FindUserByUsernameAndPassword (string Username , string Password , stUser& User )
{
    vector<stUser> vUsers = LoadUsersDataFromFile(UsersFileName) ; 

    for (stUser & U : vUsers)
    {
        if (U.Username == Username && U.Password == Password)
        {
            User = U ;
            return true;
        }
    }


    return false ;
}

bool LoadUserInfo (string Username , string Password, stUser & User)
{
    if (FindUserByUsernameAndPassword(Username, Password, User))
        return true; 
    else
        return false;
}


void Login ()
{
    bool LoginFailed = false ; 
    string Username, Password; 

    do 
    {
        system("clear");
        cout << "---------------------------------\n";
        cout << "\t  Login Screen\n" ;
        cout << "---------------------------------\n";

        if (LoginFailed)
        {
            cout <<  "Invlaid Username/Password!\n"; 
        }

        Username = ReadUsername() ; 

        Password = ReadPassword() ; 

        LoginFailed = !LoadUserInfo(Username,Password,CurrentUser);

    }while(LoginFailed) ; 

    ShowMainMenu() ;

}

int main()
{
    Login(); 

    return 0;
}