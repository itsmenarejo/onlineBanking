#include <cstdlib> // For getenv function (to access environment variables)
#include <iostream> 
#include <string> 
#include <unordered_map> 
#include <fstream>      // ✅ for file streams
#include <vector>   
#include <sstream>      // ✅ if you're using stringstream


using namespace std; 

// Decodes URL-encoded strings (like converting "%20" to space)
string url_decode(const string &src) {
    string ret; // This will hold the decoded string
    char ch;    // Temporary variable to store a decoded character
    int i, ii;  // i is index for looping, ii is integer to hold hex value

    // Loop through each character in the input string
    for (i = 0; i < (int)src.length(); i++) {
        if (src[i] == '%') { 
            // If we find '%', next two chars are hex digits representing a character
            sscanf(src.substr(i + 1, 2).c_str(), "%x", &ii); // Convert hex to int
            ch = static_cast<char>(ii); // Convert int to char
            ret += ch; // Append decoded char to result
            i = i + 2; // Skip the two hex digits we just processed
        } else if (src[i] == '+') {
            // '+' in URL encoding means a space
            ret += ' ';
        } else {
            // Normal character, just add it as-is
            ret += src[i];
        }
    }
    return ret; // Return the fully decoded string
}

// Function to parse form data string like "key=value&key2=value2" into a map
unordered_map<string, string> parse_form_data(const string &data) {
    unordered_map<string, string> params; // Map to hold key-value pairs
    size_t pos = 0, amp_pos; // pos is current parsing position, amp_pos is position of '&'

    // Loop until we reach the end of the string
    while (pos < data.length()) {
        // Find the next '&' which separates key-value pairs
        amp_pos = data.find('&', pos);

        // Extract the substring between pos and next '&' (or end of string if no '&')
        string token = (amp_pos == string::npos) ? data.substr(pos) : data.substr(pos, amp_pos - pos);

        // Find the '=' which separates key and value inside the token
        size_t eq_pos = token.find('=');

        if (eq_pos != string::npos) {
            // Extract the key and value parts
            string key = url_decode(token.substr(0, eq_pos));        // URL decode key
            string value = url_decode(token.substr(eq_pos + 1));    // URL decode value

            // Store in map
            params[key] = value;
        }

        // If no more '&' found, we are done
        if (amp_pos == string::npos)
            break;
        else
            pos = amp_pos + 1; // Move past '&' for next iteration
    }

    return params; // Return the map containing all key-value pairs
}

string read_cgi_input() {
    // Get the HTTP request method (like "GET" or "POST") from environment variables
    string method = getenv("REQUEST_METHOD") ? getenv("REQUEST_METHOD") : "";

    if (method == "GET") {
        // For GET requests, data is passed in QUERY_STRING environment variable
        return getenv("QUERY_STRING") ? getenv("QUERY_STRING") : "";
    } else if (method == "POST") {
        // For POST requests, we need to read data from standard input (stdin)
        // First, get the length of the POST data from CONTENT_LENGTH environment variable
        int content_length = getenv("CONTENT_LENGTH") ? stoi(getenv("CONTENT_LENGTH")) : 0;

        string post_data; // To store the data read from stdin

        if (content_length > 0) {
            // Allocate a buffer big enough to hold the incoming POST data plus null terminator
            char *buffer = new char[content_length + 1];

            // Read exactly content_length bytes from standard input (usually the web server sends POST data here)
            cin.read(buffer, content_length);

            // Add a null terminator at the end to safely convert to string
            buffer[content_length] = '\0';

            // Convert buffer to a C++ string
            post_data = string(buffer);

            // Free the allocated memory
            delete[] buffer;
        }

        // Return the POST data string
        return post_data;
    }

    // If method is neither GET nor POST, return empty string
    return "";
}

class Customer {
public:
    string name;
    string dob;
    int accountNumber;
    double balance;
    int securityPin;

    Customer(const string& name, const string& dob, int accountNumber, double balance, int securityPin)
        : name(name), dob(dob), accountNumber(accountNumber), balance(balance), securityPin(securityPin) {}
};

class TreeNode {
public:
    Customer* customer;
    TreeNode* left;
    TreeNode* right;

    TreeNode(Customer* cust) : customer(cust), left(nullptr), right(nullptr) {}
};

class BankingSystem {
private:
    TreeNode* root;

    TreeNode* insert(TreeNode* root, Customer* customer) {
        if (root == nullptr) {
            return new TreeNode(customer);
        }
        if (customer->accountNumber < root->customer->accountNumber) {
            root->left = insert(root->left, customer);
        } else if (customer->accountNumber > root->customer->accountNumber) {
            root->right = insert(root->right, customer);
        }
        return root;
    }

    TreeNode* search(TreeNode* root, int accountNumber) {
        if (root == nullptr || root->customer->accountNumber == accountNumber) {
            return root;
        }
        if (accountNumber < root->customer->accountNumber) {
            return search(root->left, accountNumber);
        }
        return search(root->right, accountNumber);
    }

    void saveInOrder(TreeNode* node, ofstream& ofs) {
        if (!node) return;
        saveInOrder(node->left, ofs);
        Customer* c = node->customer;
        ofs << c->name << "|" << c->dob << "|" << c->accountNumber << "|" << c->balance << "|" << c->securityPin << "\n";
        saveInOrder(node->right, ofs);
    }

public:
    BankingSystem() : root(nullptr) {}

    void addCustomer(Customer* customer) {
        root = insert(root, customer);
    }

    Customer* authenticate(int accountNumber, int securityPin) {
        TreeNode* node = search(root, accountNumber);
        if (node && node->customer->securityPin == securityPin) {
            return node->customer;
        }
        return nullptr;
    }

    string deposit(Customer* customer, double amount) {
    if (customer) {
        customer->balance += amount;
        return "Amount deposited successfully. Updated balance: $" + to_string(customer->balance);
    }
    return "Deposit failed.";
}

    string withdraw(Customer* customer, double amount) {
        if (customer) {
            if (customer->balance >= amount) {
                customer->balance -= amount;
                return "Amount withdrawn successfully. Updated balance: $" + to_string(customer->balance);
            } else {
                return "Insufficient balance for withdrawal.";
            }
        }
        return "Withdrawal failed.";
}

    void saveToFile(const string& filename) {
        ofstream ofs(filename);
        if (!ofs) {
            cerr << "Error opening file for saving: " << filename << endl;
            return;
        }
        saveInOrder(root, ofs);
        ofs.close();
    }

    void loadFromFile(const string& filename) {
        ifstream ifs(filename);
        if (!ifs) {
            // File does not exist or cannot open, just return
            return;
        }

        string line;
        while (getline(ifs, line)) {
            size_t pos = 0, prev = 0;
            vector<string> fields;
            while ((pos = line.find('|', prev)) != string::npos) {
                fields.push_back(line.substr(prev, pos - prev));
                prev = pos + 1;
            }
            fields.push_back(line.substr(prev));

            if (fields.size() == 5) {
                string name = fields[0];
                string dob = fields[1];
                int accountNumber = stoi(fields[2]);
                double balance = stod(fields[3]);
                int securityPin = stoi(fields[4]);

                Customer* cust = new Customer(name, dob, accountNumber, balance, securityPin);
                addCustomer(cust);
            }
        }
        ifs.close();
    }

    TreeNode* getRoot() {
        return root;
    }
};

int main() {
    cout << "Content-Type: text/html\n\n";

    BankingSystem bank;

    const string customersFile = "/var/www/html/customers.txt";

    bank.loadFromFile(customersFile);  // Load saved customers from file

    // If no customers found, add default demo customers
    if (!bank.getRoot()) {
        bank.addCustomer(new Customer("Alice", "01-01-1990", 1001, 5000, 1234));
        bank.addCustomer(new Customer("Bob", "02-02-1985", 1002, 3000, 5678));
        bank.addCustomer(new Customer("Charlie", "03-03-1992", 1003, 7000, 2468));
        bank.addCustomer(new Customer("David", "04-04-1988", 1004, 2000, 1357));
        bank.addCustomer(new Customer("Eva", "05-05-1995", 1005, 10000, 7890));
    }

    string cgi_input = read_cgi_input();
    unordered_map<string, string> params = parse_form_data(cgi_input);

    int accountNumber = 0;
    int securityPin = 0;
    if (params.find("accountNumber") != params.end())
        accountNumber = stoi(params["accountNumber"]);
    if (params.find("securityPin") != params.end())
        securityPin = stoi(params["securityPin"]);

    Customer* customer = bank.authenticate(accountNumber, securityPin);

    if (!customer) {
    cout << "<html><head><style>"
            "body {"
            "  display: flex;"
            "  justify-content: center;"
            "  align-items: center;"
            "  height: 100vh;"
            "  margin: 0;"
            "  background-color: #f8d7da;"  
            "  font-family: Arial, sans-serif;"
            "}"
            "div.container {"
            "  background: white;"
            "  padding: 30px 40px;"
            "  border: 2px solid #f5c2c7;"
            "  border-radius: 10px;"
            "  box-shadow: 0 0 10px rgba(255, 0, 0, 0.2);"
            "  text-align: center;"
            "  color: #721c24;"
            "}"
            "</style></head><body>";
    cout << "<div class='container'>";
    cout << "<h2>Authentication failed.<br>Invalid account number or security pin.</h2>";
    cout << "<p>Redirecting to login page...</p>";
    cout << "</div>";
    cout << "<script>setTimeout(function() { window.location = '/login.html'; }, 3000);</script>";
    cout << "</body></html>";
    return 0;
}

    string operationMessage = "";

    // Check if deposit or withdraw operation requested
    if (params.find("operation") != params.end()) {
        string op = params["operation"];
        double amount = 0;
        if (params.find("amount") != params.end())
            amount = stod(params["amount"]);

        if (op == "deposit") {
            operationMessage = bank.deposit(customer, amount);
        } else if (op == "withdraw") {
            operationMessage = bank.withdraw(customer, amount);
        }
    }

    // Save customers after any operation
    if (!operationMessage.empty()) {
        bank.saveToFile(customersFile);
    }

    // Display the customer dashboard
        cout << "<html><head><style>"
        "body {"
        "  font-family: Arial, sans-serif;"
        "  background: linear-gradient(135deg, #d7eafc, #ffffff);"
        "  display: flex;"
        "  justify-content: center;"
        "  align-items: center;"
        "  height: 100vh;"
        "  margin: 0;"
        "}"
        ".dashboard-container {"
        "  background-color: #ffffff;"
        "  border-radius: 10px;"
        "  text-align: center;"
        "  padding: 2rem 3rem;"
        "  box-shadow: 0 0 15px rgba(0, 102, 255, 0.5);"
        "  width: 420px;"
        "  animation: fadeIn 0.8s ease;"
        "  transition: transform 0.3s ease;"
        "}"
        ".dashboard-container:hover {"
        "  box-shadow: 0 0 25px 8px rgba(0, 102, 255, 0.7);"
        "  transform: translateY(-10px);"
        "}"
        "h1, h3 {"
        "  color: #0056b3;"
        "  margin-bottom: 0.5rem;"
        "}"
        "p {"
        "  color: #333333;"
        "  margin: 0.2rem 0 1rem 0;"
        "}"
        "form {"
        "  margin-bottom: 1.5rem;"
        "}"
        "input[type='number'] {"
        "  width: 70%;"
        "  padding: 0.6rem;"
        "  margin: 0.5rem 0 1rem 0;"
        "  border: 1px solid #ccc;"
        "  border-radius: 6px;"
        "  font-size: 1rem;"
        "  transition: border-color 0.3s ease, box-shadow 0.3s ease;"
        "}"
        "input[type='number']:focus {"
        "  border-color: #0056b3;"
        "  box-shadow: 0 0 6px rgba(0, 102, 255, 0.5);"
        "  outline: none;"
        "}"
        "input[type='submit'] {"
        "  width: 50%;"
        "  padding: 0.6rem;"
        "  background-color: #d7eafc;"
        "  color: #0056b3;"
        "  font-weight: bold;"
        "  border: 1px solid #0056b3;"
        "  border-radius: 6px;"
        "  font-size: 1rem;"
        "  cursor: pointer;"
        "  transition: all 0.3s ease;"
        "}"
        "input[type='submit']:hover {"
        "  background-color: #0056b3;"
        "  color: white;"
        "  box-shadow: 0 0 12px rgba(0, 102, 255, 0.8);"
        "  transform: scale(1.05);"
        "}"
        "a.logout-link {"
        "  display: inline-block;"
        "  margin-top: 1rem;"
        "  color: #0056b3;"
        "  font-weight: bold;"
        "  text-decoration: none;"
        "  border: 1px solid #0056b3;"
        "  padding: 0.3rem 0.8rem;"
        "  border-radius: 6px;"
        "  transition: background-color 0.3s ease, color 0.3s ease;"
        "}"
        "a.logout-link:hover {"
        "  background-color: #0056b3;"
        "  color: white;"
        "  box-shadow: 0 0 10px rgba(0, 102, 255, 0.7);"
        "}"
        "@keyframes fadeIn {"
        "  from { opacity: 0; transform: translateY(20px); }"
        "  to { opacity: 1; transform: translateY(0); }"
        "}"
        "</style></head><body>";

    cout << "<div class='dashboard-container'>";
    cout << "<h1>Welcome, " << customer->name << "</h1>";
    cout << "<p>Date of Birth: " << customer->dob << "</p>";
    cout << "<p>Account Number: " << customer->accountNumber << "</p>";
    cout << "<p>Balance: $" << customer->balance << "</p>";

    cout << "<h3>Deposit Money</h3>";
    cout << "<form method='POST' action='/cgi-bin/banking.cgi'>";
    cout << "<input type='hidden' name='accountNumber' value='" << customer->accountNumber << "'>";
    cout << "<input type='hidden' name='securityPin' value='" << customer->securityPin << "'>";
    cout << "<input type='hidden' name='operation' value='deposit'>";
    cout << "<input type='number' step='0.01' name='amount' placeholder='Amount' required>";
    cout << "<br>";
    cout << "<input type='submit' value='Deposit'>";
    cout << "</form>";

    // Show message for deposit or withdraw after respective form
    if (!operationMessage.empty() && params["operation"] == "deposit") {
        cout << "<p style='color: green; font-weight: bold; margin-top: 0.5rem;'>" << operationMessage << "</p>";
        bank.saveToFile("customers.txt");
    }

    cout << "<h3>Withdraw Money</h3>";
    cout << "<form method='POST' action='/cgi-bin/banking.cgi'>";
    cout << "<input type='hidden' name='accountNumber' value='" << customer->accountNumber << "'>";
    cout << "<input type='hidden' name='securityPin' value='" << customer->securityPin << "'>";
    cout << "<input type='hidden' name='operation' value='withdraw'>";
    cout << "<input type='number' step='0.01' name='amount' placeholder='Amount' required>";
    cout << "<br>";
    cout << "<input type='submit' value='Withdraw'>";
    cout << "</form>";

    if (!operationMessage.empty() && params["operation"] == "withdraw") {
        cout << "<p style='color: green; font-weight: bold; margin-top: 0.5rem;'>" << operationMessage << "</p>";
        bank.saveToFile("customers.txt");
    }


    cout << "<a href='/login.html' class='logout-link'>Logout</a>";

    cout << "</div></body></html>";

    return 0;

}
