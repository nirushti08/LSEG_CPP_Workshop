#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <unordered_set>
#include <chrono>
#include <iomanip>
using namespace std;

// Write to CSV file
void writeToCSV(ofstream& csvFile, const vector<string>& order, const string& Status, const string& instrument, const string& quantity, const string& price, const string& reason) {
    // Check if the CSV file is open
    if (!csvFile.is_open()) {
        cerr << "Error: CSV file is not open." << endl;
        return;
    }
    // Get the current time
    auto now = chrono::system_clock::now();
    time_t currentTime = chrono::system_clock::to_time_t(now);
    stringstream formattedTime;
    formattedTime << put_time(localtime(&currentTime), "%Y-%m-%d %H:%M:%S");
    
    csvFile << order[0] << ","; // Order ID
    csvFile << order[1] << ","; // Client Order ID
    csvFile << instrument << ","; // Instrument
    csvFile << order[2] << ","; // Side
    csvFile << Status << ","; // Execution Status
    csvFile << quantity << ","; // Quantity
    csvFile << price << ","; // Price
    csvFile << formattedTime.str() << ","; // Transaction Time
    csvFile << reason << "\n"; // Reason
}

// Check if the order is valid
bool isValidOrder(const vector<string>& order) {
    // Check for rejection criteria
    if (order.size() < 5){
        return false; 
    }
    // Does not contain a required field
    string sideValue = order[2];
    double priceValue = stod(order[4]);
    int qtyValue = stoi(order[3]);

    if (sideValue != "1" && sideValue != "2") {
        return false; // Invalid side
    }
    if (priceValue <= 0) {
        return false; // Price is not greater than 0
    }
    if (qtyValue % 10 != 0 || qtyValue < 10 || qtyValue > 1000) {
        return false; // Invalid quantity
    }
    return true;
}

int main() {
    // Open the CSV file
    ifstream file("order.csv"); // Change the file name here
    if (!file.is_open()) {
        cout << "Could not open the file." << endl;
        return 1;
    }
    
    // Open the output CSV file
    ofstream csvFile("exec_rep.csv"); // Change the file name here
    csvFile << "Order ID,Client Order ID,Instrument,Side,Execution Status,Quantity,Price,Transaction Time,Reason\n";
    
    string line, cell;
    getline(file, line); // Read the header row

    // Input Order
    int clOrdIDColumnIndex = 0; 
    int instrumentColumnIndex = 1;
    int sideColumnIndex = 2;
    int qtyColumnIndex = 3;
    int priceColumnIndex = 4;

    int orderIDCounter = 1;
    
    map<string, vector<vector<string>>> instrumentData; // Map to store instrument-specific data
    unordered_set<string> validInstruments = {"Rose", "Tulip", "Lavender", "Lotus", "Orchid"}; // Set of valid instruments

    // Read the CSV file line by line
    while (getline(file, line)) {
        istringstream ss(line);
        vector<string> row;
        while (getline(ss, cell, ',')) {
            row.push_back(cell);
        }
        if (row.size() > clOrdIDColumnIndex && row.size() > instrumentColumnIndex &&
            row.size() > sideColumnIndex && row.size() > qtyColumnIndex && row.size() > priceColumnIndex) {
            
            // Get the values from the row
            string clOrdIDValue = row[clOrdIDColumnIndex];
            string instrumentValue = row[instrumentColumnIndex];
            string sideValue = row[sideColumnIndex];
            string qtyValue = row[qtyColumnIndex];
            string priceValue = row[priceColumnIndex];
            string orderIDValue = "ord" + to_string(orderIDCounter++);

            // Store the values in a vector
            vector<string> orderValues;
            orderValues = { orderIDValue, clOrdIDValue, sideValue, qtyValue, priceValue};

            // Check if the instrument is valid
            if(validInstruments.find(instrumentValue) == validInstruments.end()){
                writeToCSV(csvFile, orderValues, "Reject", instrumentValue, orderValues[3], orderValues[4],"Invalid Instrument");
            }
            else{
                instrumentData[instrumentValue].push_back(orderValues);
            }  
        }
    }

    vector<vector<string>> arr2; // Store Step 1 data
    vector<vector<string>> arr1; // Store Step 2 data

    // Write the header row to the CSV file
    for (const auto& instrumentEntry : instrumentData) {
        for (const auto& order : instrumentEntry.second) {
            // Check if the order is valid
            if(isValidOrder(order)){
                if(order[2] == "1"){
                    arr1.push_back(order);
                }
                else if(order[2] == "2"){
                    arr2.push_back(order);
                }
            }
            else if (order[2] != "1" && order[2] != "2"){ 
                writeToCSV(csvFile, order, "Reject", instrumentEntry.first, order[3], order[4],"Invalid Side");
            }
            // Check if the price and quantity are valid
            else if (stod(order[4]) <= 0){ 
                writeToCSV(csvFile, order, "Reject", instrumentEntry.first, order[3], order[4],"Invalid Price");
            }
            else if (stoi(order[3]) % 10 != 0 || stoi(order[3]) < 10 || stoi(order[3]) > 1000){ 
                writeToCSV(csvFile, order, "Reject", instrumentEntry.first, order[3], order[4],"Invalid Size");
            }
        };
        // Fill the CSV file using the lists in arr2 if arr1 is empty
        if(arr1.size() == 0){
            for (size_t i = 0; i < arr2.size(); ++i) {
                writeToCSV(csvFile, arr2[i], "New", instrumentEntry.first, arr2[i][3], arr2[i][4],"-");;
            }
            arr2.clear();
        }
        // Fill the CSV file using the lists in arr1 if arr2 is empty
        else if(arr2.size() == 0){
            for (size_t i = 0; i < arr1.size(); ++i) {
                writeToCSV(csvFile, arr1[i], "New", instrumentEntry.first, arr1[i][3], arr1[i][4],"-");
            }
            arr1.clear();
        
        }
        // Fill the CSV file using the lists in arr2 if the first order in arr1 is less than the first order in arr2
        else if(stod(arr1[0][0].substr(3)) < stod(arr2[0][0].substr(3))){
            for (size_t i = 0; i < arr1.size(); ++i) {
                if(stod(arr1[i][0].substr(3)) > stod(arr2[0][0].substr(3))){
                    break;
                }
                writeToCSV(csvFile, arr1[i], "New", instrumentEntry.first, arr1[i][3], arr1[i][4],"-");
            }
        }
        // Fill the CSV file using the lists in arr2 if the first order in arr1 is greater than the first order in arr2
        else if(stod(arr1[0][0].substr(3)) > stod(arr2[0][0].substr(3))){
            // Step 1: Fill the CSV file using the lists in arr2
            for (size_t i = 0; i < arr2.size(); ++i) {
                if(stod(arr1[0][0].substr(3)) < stod(arr2[i][0].substr(3))){
                    break;
                }
                writeToCSV(csvFile, arr2[i], "New", instrumentEntry.first, arr2[i][3], arr2[i][4],"-");
            }
        }
        // Check conditions
        int j = 0;
        while (j < arr1.size()) {
            int k = j;
            for (size_t i = 0; i < arr2.size(); ++i) { 
                // Check if the price of the order in arr2 is less than or equal to the price of the order in arr1
                if(stod(arr2[i][4]) <= stod(arr1[j][4])) {
                    // Check if the quantity of the order in arr2 is equal to the quantity of the order in arr1
                    if(stod(arr2[i][3]) == stod(arr1[j][3])){
                        writeToCSV(csvFile, arr1[j], "Fill", instrumentEntry.first, arr1[j][3], arr1[j][4],"-");
                        writeToCSV(csvFile, arr2[i], "Fill", instrumentEntry.first, arr2[i][3], arr1[j][4],"-");

                        j=j+1;
                        break;
                    }
                    // Check if the quantity of the order in arr2 is greater than the quantity of the order in arr1
                    else if(stod(arr2[i][3]) > stod(arr1[j][3])){
                        writeToCSV(csvFile, arr1[j], "Fill", instrumentEntry.first, arr1[j][3], arr1[j][4],"-");
                        writeToCSV(csvFile, arr2[i], "PFill", instrumentEntry.first, arr1[j][3], arr1[j][4],"-");

                        arr2[i][3] = to_string(stoi(arr2[i][3]) - stoi(arr1[j][3]));
                        j=j+1;
                        break;
                    }
                    // Check if the quantity of the order in arr2 is less than the quantity of the order in arr1
                    else if(stod(arr2[i][3]) < stod(arr1[j][3])){
                        writeToCSV(csvFile, arr1[j], "PFill", instrumentEntry.first, arr2[i][3], arr1[j][4],"-");
                        writeToCSV(csvFile, arr2[i], "Fill", instrumentEntry.first, arr2[i][3], arr1[j][4],"-");

                        arr1[j][3] = to_string(stoi(arr1[j][3]) - stoi(arr2[i][3]));
                        j=j+1;
                        break;
                    }
                }
                else if ((stod(arr2[i][4]) > stod(arr1[j][4])) && (stod(arr1[j][0].substr(3)) < stod(arr2[i][0].substr(3)))){
                    writeToCSV(csvFile, arr2[i], "New", instrumentEntry.first, arr2[j][3], arr2[j][4],"-");
                }
            }
            // Check if the order in arr1 is not filled
            if (j == k) {
                writeToCSV(csvFile, arr1[j], "New", instrumentEntry.first, arr1[j][3], arr1[j][4],"-");
                j = j + 1;
            }
        }    
        arr2.clear();
        arr1.clear();
    }
    csvFile.close();
    return 0;
}
