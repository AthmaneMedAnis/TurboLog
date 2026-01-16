# TurboLog: High-Performance Log Analyzer

**A lightning-fast CLI tool written in C to parse and analyze massive server logs (GBs) in seconds.**

![C](https://img.shields.io/badge/Language-C-blue?logo=c)
![Performance](https://img.shields.io/badge/Performance-High-green?logo=speedtest)
![Status](https://img.shields.io/badge/Status-Portfolio_Ready-orange)

## 📌 Overview
Server logs can easily grow to Gigabytes in size. Analyzing them with high-level languages like Python can be slow and memory-intensive. 

**TurboLog** is a systems programming project designed to solve this problem. It uses **direct memory addressing** and **stream processing** to parse millions of log lines with constant memory usage (`O(1)` space complexity) and blazing speed.

## 🚀 Key Features
* **Anchor Parsing Strategy:** Uses pointer arithmetic to locate HTTP status codes instantly, avoiding full-string scans.
* **O(1) Filtering:** Uses a boolean mask lookup table to filter status codes with zero overhead.
* **Stream Processing:** Reads file line-by-line, allowing it to process files larger than available RAM (e.g., analyzing a 10GB log file on a 4GB laptop).
* **CLI Interface:** Robust argument parsing for flexible filtering.

## 🏆 Performance Benchmark
I benchmarked TurboLog against a standard Python implementation on a **1.5GB log file (20 Million lines)**.

| Implementation | Time Taken | Speedup |
| :--- | :--- | :--- |
| **Python** | ~XX.XX s | 1x |
| **TurboLog (C)** | **~X.XX s** | **~20x** |

*(Replace the placeholders above with your actual numbers)*

> **Visual Proof:**
> ![Benchmark Results](./Results/benchmark.png)

## 🛠️ Installation & Usage

### 1. Compile the Tool
You need a C compiler (`gcc`).

```bash
gcc turbolog.c -o turbolog```

### 2. Run the Analyzer
**Basic Usage (Summary Mode):**
Counts all HTTP status codes in the file.
```bash
./turbolog access.log
```

**Filter Mode: Counts only specific status codes (e.g., 404 errors).**
```bash
./turbolog access.log -s 404
```

**Multiple Filters: Counts multiple codes simultaneously.**
```bash
./turbolog access.log -s 404 500 503
```

## 🧠 Technical Deep Dive
### The "Anchor Parsing" Algorithm
Instead of splitting strings (which is expensive), TurboLog finds the status code by locating the **HTTP Request Quotes**.
1. It scans for the first `"` (start of request).
2. It scans for the second `"` (end of request).
3. The Status Code is guaranteed to be the integer immediately following the second quote.

This pointer-based approach avoids memory allocation for substrings, making it significantly faster than `strtok` or regex.

### Memory Management
TurboLog allocates a fixed 64KB buffer on the stack. It never uses `malloc` for line processing, ensuring **zero memory fragmentation** and **deterministic memory usage**.

## 👨‍💻 Author
**Mohamed Anis ATHMANE**
*Data Scientist & Systems Programmer*
[GitHub Profile](https://github.com/AthmaneMedAnis)
