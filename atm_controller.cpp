#include <iostream>
#include <cstdio>
#include <map>
#include <unordered_map>
#include <assert.h>

using namespace std;

// sample Bank and CardInfo classes, just for purpose of demoing the ATMController class
struct CardInfo {
    string pin;
    map<string, int> accounts; // this would be pretty small (typically, not many
                               // accounts per card)
};

class Bank {
    // this would potentially be very large, so
    // we need good time complexity for queries
    // (bank will have many customers in general)
    unordered_map<int, CardInfo> cards_;

    public:
    
    Bank(const unordered_map<int, CardInfo>& cards) : cards_(cards) {};

    bool cardExists(int card_num) {
        return cards_.count(card_num);
    }

    bool isPinCorrect(int card_num, string pin, CardInfo*& active_card) {
        if (cardExists(card_num)) {
            active_card = &(cards_.at(card_num));
            return active_card->pin == pin;
        }
        return false;
    }
};

class ATMController {
    Bank* bank_ = nullptr;
    CardInfo* active_card_ = nullptr;
    int* account_balance_ = nullptr;
    int card_num_ = -1;
    string pin_;
    
    bool has_card_ = false;
    bool is_pin_valid_ = false;
    bool account_exists_ = false; 

    public:
        ATMController(Bank* bank) {
            if (bank != nullptr) {
                bank_ = bank;
            }
        }

        void insertCard(int card_num) {
            if (bank_->cardExists(card_num)) {
                card_num_ = card_num;
                has_card_ = true;
            } else {
                cout << "ERROR: card number does not exist in bank database.\n";
            }
        }

        void removeCard() {
            if (has_card_) {
                has_card_ = false;
                is_pin_valid_ = false;
                account_exists_ = false;
                account_balance_ = nullptr;
                active_card_ = nullptr;
                card_num_ = -1;
                cout << "card removed successfully\n";
            }
        }

        void enterPin(const string& pin) {
            if (has_card_) {
                if (bank_->isPinCorrect(card_num_, pin, active_card_)) {
                    pin_ = pin;
                    is_pin_valid_ = true;
                } else {
                    cout << "ERROR: PIN entered is not correct.\n";
                } 
            }
        }

        void selectAccount(const string& account_name) {
            if (is_pin_valid_) {
                auto it = active_card_->accounts.find(account_name);
                if (it != active_card_->accounts.end()) {
                    account_exists_ = true;
                    account_balance_ = &(it->second);
                    cout << "selected account with name \"" << it->first << "\" and balance " << account_balance_ << "\n";
                } else {
                    cout << "ERROR: account does not exist.\n";
                }
            } else {
                cout << "ERROR: must enter PIN before accessing the accounts\n";
            }
        }

        int seeBalance() {
            if (account_exists_) {
                return *account_balance_;
            }
            cout << "ERROR: no account has been selected\n";
            return -1;
        }

        void withdraw(int amount, int& returned_amount) {
            if (account_exists_) {
                if (*account_balance_ < amount) {
                    cout << "ERROR: insufficient funds, enter amount smaller than current balance\n";
                    return;
                }
                *account_balance_ -= amount;
                returned_amount = amount;
                return;
            }
            cout << "ERROR: no account has been selected\n";
        }

        void deposit(int amount) {
            if (account_exists_) {
                *account_balance_ += amount;
                return;
            }
            cout << "ERROR: no account has been selected\n";
        }

};

int main() {
    // First, create various CardInfo objects. These
    // represent cards containing one or more accounts with
    // different balances. Each CardInfo is protected by a PIN.
    CardInfo c1 = {"1234", {{"main", 2000}, {"second", 150}}};
    CardInfo c2 = {"9999", {{"main", 10000}}};

    // Next, we create Bank object with key-value pairs, where
    // key is the card number and value is the CardInfo object.
    // For this demo, the bank only has 2 customers.
    unordered_map<int, CardInfo> customers{{123123123, c1}, {123456789, c2}};
    Bank* bank = new Bank(customers);

    // Now we can create our ATMController object, which takes a pointer
    // to a Bank object as input.
    ATMController controller_test(bank);

    // Finally, write some tests to show how the API works.
    // Let's assume we want to withdraw money from the account
    // "second" of the credit card 123123123. We'll first attempt
    // to withdraw more money than there is, which should result in
    // an error. Then we'll try to withdraw a valid amount, which should work.
    int card_num1 = 123123123;
    controller_test.insertCard(card_num1);

    controller_test.enterPin("0000"); // PIN is incorrect, should not work
    controller_test.enterPin("1234"); // ok
    
    controller_test.selectAccount("wrong_name"); // should not work, account name not valid
    controller_test.selectAccount("second"); // should work, valid name
    
    cout << "current balance: " << controller_test.seeBalance() << "\n";

    int returned_amount = 0;
    controller_test.withdraw(200, returned_amount); // try to withdraw more money than there is
    assert(returned_amount == 0); // test that no amount is returned
    
    int amount_to_deposit = 50;
    controller_test.deposit(amount_to_deposit);
    cout << "deposited " << amount_to_deposit << " dollars\n";
    cout << "new balance: " << controller_test.seeBalance() << "\n";

    controller_test.withdraw(200, returned_amount); // ok
    cout << "withdrew " << 200 << " dollars\n";
    cout << "new balance: " << controller_test.seeBalance() << "\n";

    controller_test.removeCard();
    return 0;
}