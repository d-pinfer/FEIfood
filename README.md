<div align="center">
  <img src="./assets/feifood-identity.jpg" alt="FEIfood visual identity" width="320">
</div>

# 🍔 FEIfood

FEIfood is a terminal-based delivery system developed in **C** as an academic project at **Centro Universitário FEI**.

The application simulates a food delivery workflow, allowing users to create accounts, search a menu, build and edit orders, choose a payment method, provide delivery information and rate completed orders.

## 🚀 Features

- User registration and login
- Food search by item name
- Order creation and item management
- Add, remove and change item quantities
- View and clear open orders
- Order subtotal and total calculation
- Delivery fee calculation
- Delivery address registration
- Payment simulation via PIX, credit card or debit card
- Order status management
- Rating system for completed orders
- Local persistence using text files

## 💻 Technologies and Concepts

- C
- Structs
- Pointers
- Dynamic memory allocation
- File handling
- String manipulation
- Input validation
- Arrays
- Functions and modularization
- Algorithms and control flow

## 🧠 Project Highlights

The project applies fundamental C programming concepts in a larger application flow.

Some examples include:

- manual memory management with `malloc` and `free`;
- reading and rewriting structured text files;
- parsing strings with `strtok`;
- custom data structures for order items and ratings;
- validation of user input;
- management of order states;
- calculation of subtotals, delivery fee and final order value.

## 📁 Project Structure

```text
FEIfood/
├── main.c
├── cardapio.txt
├── README.md
└── .gitignore
```

The application uses local text files during execution:

- `cardapio.txt` — food catalog used by the search and order flow (included in the repository);
- `cadastros.txt` — user accounts;
- `pedidos.txt` — user orders;
- `avaliacoes.txt` — order ratings.

The user-generated files are ignored by Git because they may contain personal information.


## ▶️ How to Compile and Run

### 1. Clone the repository

```bash
git clone https://github.com/d-pinfer/FEIfood.git
cd FEIfood
```

### 2. Create the local data files

Create these empty files in the project folder:

```text
cadastros.txt
pedidos.txt
avaliacoes.txt
```

### 3. Compile

Using GCC:

```bash
gcc main.c -o feifood
```

On Windows, you may use:

```bash
gcc main.c -o feifood.exe
```

### 4. Run

Linux/macOS:

```bash
./feifood
```

Windows:

```bash
feifood.exe
```

## 💳 Order Flow

```text
Account/Login
      ↓
Search Food
      ↓
Add to Order
      ↓
View / Edit Order
      ↓
Confirm Delivery
      ↓
Choose Payment
      ↓
Complete Order
      ↓
Rate Order
```

The application supports simulated PIX payments and card payment on delivery.

## 🎥 Demo

Watch a test/demo of the project on YouTube:

[▶️ FEIfood — Project Demo](https://youtu.be/yJdrEcwlVH0)

## 🔒 Security Note

This is an academic project created for learning purposes. User credentials and application data are stored locally in plain-text files and the authentication/payment flow is not intended for production use.

## 🎓 Academic Context

Project developed during the **Computer Science** program at **Centro Universitário FEI**.

## 👨‍💻 Author

**Davi Pinheiro Ferreira**
