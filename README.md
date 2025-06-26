# onlineBanking
A simple banking system using DSA (C++). 

---

## ⚙️ Requirements

- Apache Server with CGI enabled
- C++ compiler (e.g., `g++`)
- Linux OS (Ubuntu/Debian recommended) or WSL on Windows
- Optional: XAMPP for Windows users

---

## 🧱 Setup Instructions

### 1. ✅ Enable CGI in Apache (Linux)
```
sudo a2enmod cgi
sudo systemctl restart apache2
```
### 2. 🛠️ Compile the C++ CGI Script
Navigate to your project directory and run:
```
g++ banking.cpp -o banking.cgi
chmod 755 banking.cgi
```
### 3. 🚚 Move the CGI Script to Apache’s CGI Directory
```
sudo mv banking.cgi /usr/lib/cgi-bin/
```
### 4. 🧾 Move HTML and Assets to Web Root
```
sudo mv login.html /var/www/html/
sudo mv about.html /var/www/html/
sudo cp -r images /var/www/html/
```
### 5. 🔐 Set Proper Permissions
```
sudo chmod -R 755 /usr/lib/cgi-bin/banking.cgi
sudo chmod -R 755 /var/www/html/
```
### 6. 🚀 Start or Restart Apache Server
```
sudo systemctl start apache2     # Start if it's not running
sudo systemctl restart apache2   # Restart if already running
```
### 🌐 Running the Project
Open your browser and go to:
Login Page → http://localhost/login.html
