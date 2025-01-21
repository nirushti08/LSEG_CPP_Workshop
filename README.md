### README.md

# Flower Exchange Trading Application

This is a C++ application simulating a trading platform for flowers, inspired by a workshop with **London Stock Exchange Group (LSEG)**. The program processes flower orders, validates them, and executes trades based on predefined rules. Orders are logged in a CSV file for easy tracking and analysis.

---

## Features

- **Order Validation**: Ensures orders have valid instruments, prices, quantities, and sides.
- **Trade Matching**: Matches buy and sell orders based on price and quantity.
- **Order Book Management**: Handles unmatched orders by storing them for future trades.
- **CSV Logging**: Logs execution reports, including filled, partially filled, and rejected orders.

---

## How It Works

1. **Input**: Reads orders from `order.csv`.
2. **Validation**: Rejects orders with:
   - Invalid instruments (not in a predefined list).
   - Incorrect price or quantity values.
3. **Matching**: Matches buy and sell orders based on:
   - Price and FIFO priority.
   - Quantity constraints.
4. **Output**: Writes results to `exec_rep.csv` with details about execution status and reasons for rejection.

---

## Getting Started

### Prerequisites
- C++ compiler (e.g., GCC, Clang)
- Input file named `order.csv` in the following format:
  ```
  Client Order ID,Instrument,Side,Quantity,Price
  ```

### Running the Program

1. Compile the program:
   ```bash
   g++ -std=c++17 -o flower_exchange LSEG_FlowerExchange.cpp
   ```

2. Run the program:
   ```bash
   ./flower_exchange
   ```

3. Check the output in `exec_rep.csv` for execution details.

---

## File Structure

- **LSEG_FlowerExchange.cpp**: Main source code.
- **order.csv**: Input file containing order details.
- **exec_rep.csv**: Output file with execution reports.

---

## Valid Instruments
- Rose
- Tulip
- Lavender
- Lotus
- Orchid

---

## Example

**Input (`order.csv`):**
```
Client Order ID,Instrument,Side,Quantity,Price
12345,Rose,1,100,50.0
12346,Tulip,2,100,50.0
```

**Output (`exec_rep.csv`):**
```
Order ID,Client Order ID,Instrument,Side,Execution Status,Quantity,Price,Transaction Time,Reason
ord1,12345,Rose,1,Fill,100,50.0,2025-01-21 15:00:00,-
ord2,12346,Tulip,2,Fill,100,50.0,2025-01-21 15:00:00,-
```
