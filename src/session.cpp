#include "session.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>

Session::Session(){
  admin_ = false;
  logged_ = false;
  read_accounts();
}

void Session::login() {
  //check if logged in
  std::string name;
  std::map<int,Account> account_map;
  if(logged_){
    std::cout << "Transaction denied. Already logged in" << std::endl;
  }else{
    std::string input = "";
    std::cout << "Please select the following:\nadmin\nuser" << std::endl;
    getline(std::cin, input);
    // check for admin, user, or invalid
    if(input == "admin"){
      admin_ = true;
      write_file(10,"",0,0,"A");
    }else if(input == "user"){
      std::cout << "Please enter your name:" << std::endl;
      getline(std::cin,name);
      try{
        account_map = accounts_.at(name);
      }catch(const std::out_of_range& err){
        std::cout << "The account holder's name is invalid" << std::endl;
        return;
      }
      name_ = name;
      admin_ = false;
      write_file(10,name_,0,0,"S");
    }else{
      std::cout << "Login failed, you must specify either admin or user" << std::endl;
      return;
    }
    // check for account name
    logged_ = true;
  }
}

void Session::read_accounts() {
  std::string filename = "CurrentAccount.txt";
  std::ifstream cafile(filename);

  std::string line;
  std::string token;
  int id;
  std::string name;
  int balance;
  bool enabled;
  bool student;

  while (!cafile.eof()) {
    getline(cafile, line);
    token = line.substr(0, 5);  // extract account id
    id = atoi(token.c_str());   // convert account id to integer

    token = line.substr(6, 20);  // extract account name

    name = trim(token);

    token = line.substr(27, 1);  // extract account enabled flag
    if (token == "A") {
      enabled = true;
    } else {
      enabled = false;
    }

    token = line.substr(29, 8);     // extract account balance
    balance = atof(token.c_str())*100;  // convert account balance to int

    token = line.substr(38, 1);  // extract plan status
    if (token == "N") {
      student = false;
    } else {
      student = true;
    }

    // printf("%d %s %f\n", id, name.c_str(), balance);

    Account curr = Account(id, name, balance, enabled, student);
    if (!accounts_.count(name)) {
      std::map<int, Account> temp;
      accounts_[name] = temp;
    }
    accounts_[name][id] = curr;
  }
}

void Session::write_file(int trans_num, std::string name, int account_id, int value, std::string misc){
  // write transaction file
  char out_file [40];
  float value_f;
  value_f = value / 100;
  sprintf(out_file, "%02d %20s %05d %00008.2f %s", trans_num, name.c_str(), account_id, value_f, misc.c_str());
  std::string current_transaction(out_file);
  if(trans_num == 00){
    std::ofstream trans_file("transactions.trf");
    for(uint i = 0; i < transactions_.size();i++){
      trans_file << transactions_[i] << std::endl;
    }
  }else{
    transactions_.push_back(current_transaction);
  }
}

void Session::logout() {
  // check if logged out
  if(!logged_){
    std::cout << "Transaction denied. Not logged in" << std::endl;
  }else{
  // set logged flag on session
    std::cout << "You have successfully logged out";
    logged_ = false;
    if(admin_){
      write_file(00,"",0,0,"A");
    }else{
      write_file(00,name_,0,0,"S");
    }
    exit(0);
  }
  // logout
}

void Session::withdrawal() {
  // withdrawal
  std::string name, input;
  std::string::size_type no_convert;
  int value_i, account_id;
  float value_f;
  std::map<int,Account> account_map;
  Account account;
  if(!logged_){
    std::cout << "Transaction denied. Not logged in" << std::endl;
    return;
  }else if(admin_){
    std::cout << "Please enter the account holder's name:" << std::endl;
    getline(std::cin, name);
  }else{
    name = name_;
  }
  try{
    account_map = accounts_.at(name);
  }catch(const std::out_of_range& err){
    std::cout << "The account holder's name is invalid" << std::endl;
    return;
  }
  std::cout << "Please enter the account number to withdraw from:" << std::endl;
  getline(std::cin, input);
  try{
    account_id = std::stoi(input);
    account = account_map.at(account_id);
  }catch(const std::out_of_range& err){
    std::cout << "Withdrawal failed, account does not exist" << std::endl;
    return;
  }catch(const std::invalid_argument& err){
    std::cout << "Withdrawal failed, account does not exist" << std::endl;
    return;
  }
  if(!account.is_enabled()){
    std::cout << "Withdraw failed, " << account_id << " is disabled" << std::endl;
    return;
  }

  std::cout << "Please enter the amount to withdraw:" << std::endl;
  getline(std::cin, input);
  try{
    value_f = std::stof(input, &no_convert);
    value_i = value_f * 100;
    if(no_convert!=input.length()){
        throw std::invalid_argument("non numerical characters");
    }
  }catch(const std::invalid_argument& err){
    std::cout << "Withdrawal failed, value must be a numerical value." << std::endl;
    return;
  }catch(const std::out_of_range& err){
    std::cout << "Withdrawal failed, value must be a numerical value." << std::endl;
    return;
  }
  if(value_i > 50000 ){ // TODO: Check for current day maximum Need to chang the code
    std::cout << "Withdrawal of $" << value_i << " failed; cannot withdraw more than $500.00 in a single day" << std::endl;
    return;
  }else if(value_i > account.get_balance()){
    std::cout << "Withdrawal failed, you must have at least $0.00 after transaction fees in your account" << std::endl;
    return;
  }else if(fmod(value_i/100,5) != 0){
    std::cout << "Withdrawal failed, you must enter a number divisible by 5" << std::endl;
    return;
  }else if(!admin_){
    if(account.is_student() && value_i + 5 > account.get_balance()){
      std::cout << "Withdrawal failed, you must have at least $0.00 after transaction fees in your account" << std::endl;
      return;
    }else if(!(account.is_student()) && value_i + 10 > account.get_balance()){
      std::cout << "Withdrawal failed, you must have at least $0.00 after transaction fees in your account" << std::endl;
      return;
    }
  }
  std::cout << "Withdrawal of $" << value_i << " was successful" << std::endl;
  write_file(01,name,account_id,value_i,"");
}

void Session::deposit() {
  // deposit
  std::string name, input;
  std::string::size_type no_convert;
  int value_i, account_id;
  float value_f;
  std::map<int,Account> account_map;
  Account account;
  if(!logged_){
    std::cout << "Transaction denied. Not logged in" << std::endl;
    return;
  }else if(admin_){
    std::cout << "Please enter account holder's name:" << std::endl;
    getline(std::cin, name);
  }else{
    name = name_;
  }
  try{
      account_map = accounts_.at(name);
    }catch(const std::out_of_range& err){
      std::cout << "The account holder's name is invalid" << std::endl;
      return;
    }
  std::cout << "Enter the account number:" << std::endl;
  getline(std::cin, input);
  try{
    account_id = stoi(input);
    account = account_map.at(account_id);
  }catch(const std::out_of_range& err){
    std::cout << "Deposit failed, you entered an invalid account number" << std::endl;
    return;
  }catch(const std::invalid_argument& err){
    std::cout << "Deposit failed, you entered an invalid account number" << std::endl;
    return;
  }
  if(!account.is_enabled()){
    std::cout << "Deposit failed, " << account_id << " is disabled" << std::endl;
    return;
  }
  std::cout << "Enter the amount in dollars to deposit:" << std::endl;
  getline(std::cin, input);
  try{
    value_f = std::stof(input, &no_convert);
    value_i = value_f * 100;
    if(no_convert!=input.length()){
        throw std::invalid_argument("non numerical characters");
    }
  }catch(const std::invalid_argument& err){
    std::cout << "Deposit failed, you must enter a numerical value." << std::endl;
    return;
  }catch(const std::out_of_range& err){
    std::cout << "Deposit failed, you must enter a numerical value." << std::endl;
    return;
  }
  if(!admin_){
    if(account.is_student() && value_i + 5 > account.get_balance()){
      std::cout << "Deposit failed, you must have at least $0.00 after transaction fees" << std::endl;
      return;
    }else if(!(account.is_student()) && value_i + 10 > account.get_balance()){
      std::cout << "Deposit failed, you must have at least $0.00 after transaction fees" << std::endl;
      return;
    }
  }
  std::cout << "Deposit successful, your deposit will be on hold until the next day" << std::endl;
}

void Session::changeplan() {
  // changeplan
  std::string name, input;
  int account_id = 0;
  std::map<int,Account> account_map;
  Account account;
  if(!logged_){
    std::cout << "Transaction denied. Not logged in" << std::endl;
    return;
  }else if(!admin_){
    std::cout << "Transaction denied. User is not an admin" << std::endl;
    return;
  }
  std::cout << "Please enter the account holder's name:" << std::endl;
  getline(std::cin, name);
  try{
    account_map = accounts_.at(name);
  }catch(const std::out_of_range& err){
    std::cout << "The account holder's name is invalid" << std::endl;
    return;
  }
  std::cout << "Please enter the account number:" << std::endl;
  getline(std::cin, input);
  try{
    account_id = std::stoi(input);
    account = account_map.at(account_id);
  }catch(const std::out_of_range& err){
    std::cout << "The account number is invalid" << std::endl;
    return;
  }
  std::cout << "Account's plan has been changed" << std::endl;
}

void Session::transfer() {
  // transfer
  std::string name, input;
  std::string::size_type no_convert;
  int account_id_1, account_id_2, value_i;
  float value_f;
  std::map<int,Account> account_map;
  Account account_1, account_2;
  if(!logged_){
    std::cout << "Transaction denied. Not logged in" << std::endl;
    return;
  }else if(admin_){
    std::cout << "Please enter the account holder's name:" << std::endl;
    getline(std::cin, name);
  }else{
    name = name_;
  }
  try{
      account_map = accounts_.at(name);
    }catch(const std::out_of_range& err){
      std::cout << "The account holder's name is invalid" << std::endl;
      return;
    }
  std::cout << "Please enter the account number to transfer from:" << std::endl;
  getline(std::cin, input);
  try{
    account_id_1 = stoi(input);
    account_1 = account_map.at(account_id_1);
  }catch(const std::out_of_range& err){
    std::cout << "Transfer failed, account does not exist" << std::endl;
    return;
  }catch(const std::invalid_argument& err){
    std::cout << "Transfer failed, account does not exist" << std::endl;
    return;
  }
  if(!account_1.is_enabled()){
    std::cout << "Transfer unsuccessful, " << account_id_1 << " is disabled" << std::endl;
    return;
  }
  std::cout << "Please enter the account number to transfer to:" << std::endl;
  getline(std::cin, input);
  try{
    account_id_2 = std::stoi(input);
    account_2 = account_map.at(account_id_2);
  }catch(const std::out_of_range& err){
    std::cout << "Transfer failed, account does not exist" << std::endl;
    return;
  }catch(const std::invalid_argument& err){
    std::cout << "Transfer failed, account does not exist" << std::endl;
    return;
  }
  if(!account_2.is_enabled()){
    std::cout << "Transfer unsuccessful, " << account_id_2 << " is disabled" << std::endl;
    return;
  }
  std::cout << "Please enter the amount you wish to transfer:" << std::endl;
  getline(std::cin, input);
  try{
    value_f = std::stof(input, &no_convert);
    value_i = value_f * 100;
    if(no_convert!=input.length()){
        throw std::invalid_argument("non numerical characters");
    }
  }catch(const std::out_of_range& err){
    std::cout << "Invalid amount, you can only transfer numerical values" << std::endl;
    return;
  }catch(const std::invalid_argument& err){
    std::cout << "Invalid amount, you can only transfer numerical values" << std::endl;
    return;
  }
  if(value_i > 100000 && !admin_){ //TODO: Check for current day maximum Need to chang the code
    std::cout << "Transfer unsuccessful, can not transfer more than $1000.00 in a single day" << std::endl;
    return;
  }else if(value_i > account_1.get_balance()){
    std::cout << "Transfer failed, your account must have at least $0.00 after transaction fees" << std::endl;
    return;
  }else if(!admin_){
    if(account_1.is_student() && value_i + 5 > account_1.get_balance()){
      std::cout << "Transfer failed, your account must have at least $0.00 after transaction fees" << std::endl;
      return;
    }else if(!(account_1.is_student()) && value_i + 10 > account_1.get_balance()){
      std::cout << "Transfer failed, your account must have at least $0.00 after transaction fees" << std::endl;
      return;
    }
  }
  std::cout << "Transfer to " << account_id_1 << " from " << account_id_2 << " of $" << value_i << " successful" << std::endl;
}


void Session::paybill() {
  // paybill
  std::string name, company, lower_company, input;
  std::string::size_type no_convert;
  int account_id, value_i;
  float value_f;
  std::map<int,Account> account_map;
  Account account;
  if(!logged_){
    std::cout << "Transaction denied. Not logged in" << std::endl;
    return;
  }else if(admin_){
    std::cout << "Please enter the account holder's name:" << std::endl;
    getline(std::cin, name);
  }else{
    name = name_;
  }
  try{
      account_map = accounts_.at(name);
    }catch(const std::out_of_range& err){
      std::cout << "The account holder's name is invalid" << std::endl;
      return;
    }
  std::cout << "Enter the account number:" << std::endl;
  getline(std::cin, input);
  try{
    account_id = stoi(input);
    account = account_map.at(account_id);
  }catch(const std::out_of_range& err){
    std::cout << "Invalid account number: " << input << std::endl;
    return;
  }catch(const std::invalid_argument& err){
    std::cout << "Invalid account number: " << input << std::endl;
    return;
  }
  if(!account.is_enabled()){
    std::cout << "Payment failed, " << account_id << " is disabled" << std::endl;
    return;
  }
  std::cout << "Enter the company to whom you wish the pay the bill to:" << std::endl;
  getline(std::cin, company);
  char *to_lower = new char [company.length()];
  for(uint i = 0; i < company.length(); i++){
    to_lower[i] = std::tolower(company[i]);
  }
  lower_company = to_lower;

  if(lower_company == "the bright light electric company" || lower_company == "ec"){// TODO: find the company name
    company = "The Bright Light Electric Company";
  }else if(lower_company == "credit card company q" || lower_company == "cq"){
    company = "Credit Card Company Q";
  }else if(lower_company == "low definition tv, inc." || lower_company == "tv"){
    company = "Low Definition TV, Inc.";
  }else{
    std::cout << company << " is not a valid company to pay a bill to:" << std::endl;
    return;
  }
  std::cout << "Enter the amount you wish to pay:" << std::endl;
  getline(std::cin, input);
  try{
    value_f = std::stof(input, &no_convert);
    value_i = value_f * 100;
    if(no_convert!=input.length()){
        throw std::invalid_argument("non numerical characters");
    }
  }catch(const std::invalid_argument& err){
    std::cout << "Payment, you must enter a numerical value." << std::endl;
    return;
  }catch(const std::out_of_range& err){
    std::cout << "Payment, you must enter a numerical value." << std::endl;
    return;
  }
  if(value_i > 200000 && !admin_){// TODO: Check for current day maximum Need to chang the code
    std::cout << "You may not pay more than $2000 to a bill holder in a day." << std::endl;
    return;
  }else if(value_i > account.get_balance()){
    std::cout << "Payment to " << company << " of $" << value_i << " failed, you do not have at least $" << value_i << " in your account." << std::endl;
    return;
  }else if(!admin_){
    if(account.is_student() && value_i + 5 > account.get_balance()){
      std::cout << "Payment failed, you must have at least $0.00 after transaction fees in your account" << std::endl;
      return;
    }else if(!(account.is_student()) && value_i + 10 > account.get_balance()){
      std::cout << "Payment failed, you must have at least $0.00 after transaction fees in your account" << std::endl;
      return;
    }
  }
  std::cout << "Payment to " << company << " of $" << value_i  << " was successful" << std::endl;
}

void Session::create() {
  // create"";
  std::string name, input;
  std::string::size_type no_convert;
  std::size_t nonAlpha;
  int balance_i;
  float balance_f;

  if(!logged_){
    std::cout << "Transaction denied. Not logged in" << std::endl;
    return;
  }
  else if(!admin_){
    std::cout <<  "Transaction denied. User is not an admin" << std::endl;
    return;
  }

  std::cout << "Please input Account Name:" << std::endl;
  getline(std::cin, name);
  name = trim(name);
  nonAlpha = name.find_first_not_of("aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ- "); //characters that are allowed for the user to use
  if(name.length() > 20){ //more characters than allowed
    std::cout << "I'm sorry, your name is too long. Please shorten it to 20 characters or less." << std::endl;
    return;
  }
  else if(name.length() == 0){
    std::cout << "Transaction denied. No name detected"<< std::endl;
    return;
  }
  else if (nonAlpha!=std::string::npos){
    std::cout << "I'm sorry, your name is unrecognizable and cannot be used (get rid of all numbers and symbols)" << std::endl;
    return;
  }
  std::cout << "Please input your Initial Balance:" << std::endl;
  getline(std::cin, input);
  input = trim(input);
  if(input.length() == 0){
    balance_i = 0;
  }else{
    try{
      balance_f = std::stof(input,&no_convert);
      balance_i = balance_f * 100;
      if(no_convert!=input.length()){
        throw std::invalid_argument("Invalid characters");
    }
    }catch(const std::out_of_range& err){
      std::cout << "Transaction denied. Invalid characters" << std::endl;
      return;
    }catch(const std::invalid_argument&){
      std::cout << "Transaction denied. Invalid characters" << std::endl;
      return;
    }
  }
  if (balance_i > 9999999){
    std::cout << "Transaction denied. Amount entered is too large" << std::endl;
    return;
  }
  else if(balance_i == 0){
    std::cout << "An initial balance of 00000.00 has been administered" << std::endl;
  }
  std::cout << "A new account was made under the name:\n" << name << std::endl << 
  "with a current balance of:\n" << balance_i << std::endl << 
  "Your account will be available on the next day" << std::endl;
  return;
}

void Session::remove() {
  // remove
  std::string name, input;
  int account_id = 0;
  std::map<int,Account> account_map;
  std::size_t nonAlpha;
  Account account;
  if(!logged_){
    std::cout << "Transaction denied. Not logged in" << std::endl;
    return;
  }else if(!admin_){
    std::cout << "Transaction denied. User is not an admin" << std::endl;
    return;
  }

  std::cout << "Please input Account Name:" << std::endl;
  getline(std::cin, name);
  name = trim(name);
  nonAlpha = name.find_first_not_of("aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ- "); // Alphabet of usable characters
  if(name.length() > 20) //more characters than allowed
  {
    std::cout <<"Transaction denied. Name is too long" << std::endl;
    return;
  }
  else if(name.length() == 0){
    std::cout << "Transaction denied. No name detected" << std::endl;
    return;
  }
  else if (nonAlpha!=std::string::npos)
  {
    std::cout << "Transaction denied. Non-Alphabetic chracters have been found.";
    return;
  }else{
    try{
      account_map = accounts_.at(name);
    }catch(const std::out_of_range& err){
      std::cout << "I'm sorry, this name is not registered in the bank" << std::endl;
      return;
    }
  }
  std::cout << "Please input your Account Number:" << std::endl;
  getline(std::cin, input);
  try{
    account_id = std::stoi(input);
    account = account_map.at(account_id);
  }catch(const std::out_of_range& err){
    std::cout << "I'm sorry, the account number given does not match up to the Account Name." << std::endl;
    return;
  }
    std::cout << "The account has been deleted" << std::endl;
}

void Session::enable(bool enable) {
  // disable
  std::string name = "";
  std::string input;
  int account_id = 0;
  std::map<int,Account> account_map;
  Account account;
  if(!logged_){
    std::cout << "Transaction denied. Not logged in" << std::endl;
    return;
  }else if(!admin_){
    std::cout << "Transaction denied. User is not an admin" << std::endl;
  }else{
    std::cout << "Please enter account holder's name:" << std::endl;
    getline(std::cin, name);
    try{
      account_map = accounts_.at(name);
    }catch(const std::out_of_range& err){
      std::cout << "The account holder's name is invalid" << std::endl;
      return;
    }
    std::cout << "Please enter account number:" << std::endl;
    getline(std::cin, input);
    try{
      account_id = std::stoi(input);
      account = account_map.at(account_id);
    }catch(const std::out_of_range& err){
      std::cout << "The account number is invalid" << std::endl;
      return;
    }
    if(enable){
      std::cout << "Account has been successfully enabled" << std::endl;
    }else{
      std::cout << "Account has been successfully disabled" << std::endl;
    }
  }
}

std::string Session::trim(const std::string& str, const std::string& whitespace){
  const auto strBegin = str.find_first_not_of(whitespace);
  if(strBegin == std::string::npos) return ""; // nothing in string

  const auto strEnd = str.find_last_not_of(whitespace);
  const auto strRange = strEnd - strBegin + 1;
  return str.substr(strBegin,strRange);
}
