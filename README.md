# onlineBanking
A simple banking system using DSA (C++). 

-------------------------------------------------------------------------------------------------------------------------------

## ⚙️ Requirements

- Apache Server (with CGI enabled)
- C++ compiler (like `g++`)
- Linux OS (Ubuntu/Debian recommended)

-------------------------------------------------------------------------------------------------------------------------------

## 🧱 Setup Instructions

### 1. **Enable CGI in Apache**
```bash
sudo a2enmod cgi
sudo systemctl restart apache2

### 2. **Compile the C++ Script**

g++ banking.cpp -o banking.cgi
chmod 755 banking.cgi

