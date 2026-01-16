import os
import time
import subprocess
import random

# CONFIGURATION
LOG_FILE = "benchmark_data.log"
C_TOOL = "./turbolog.exe"  # Make sure you compiled it: gcc turbolog.c -o turbolog
LINES_TO_GENERATE = 20_000_000  # 20 Million lines is roughly ~1,5GB

def generate_data():
    print(f"Generating {LINES_TO_GENERATE} log lines... (This happens only once)")
    if os.path.exists(LOG_FILE):
        print("   File already exists. Skipping generation.")
        return

    # Random data to generate the log file
    ips = ["192.168.1.1", "10.0.0.2", "127.0.0.1", "172.16.0.5"]
    paths = ["/index.html", "/api/login", "/assets/logo.png", "/about"]
    codes = [200, 200, 200, 404, 500, 301, 302, 403, 502, 503, 504]

    with open(LOG_FILE, "w") as f:
        for i in range(LINES_TO_GENERATE):
            ip = random.choice(ips)
            code = random.choice(codes)
            path = random.choice(paths)
            # CLF Format
            line = f'{ip} - - [10/Oct/2000:13:55:36] "GET {path} HTTP/1.1" {code} {random.randint(100,5000)}\n'
            f.write(line)
    print("Generation Complete.")

def run_python_logic():
    print("\nRunning Python Logic...")
    start = time.time()
    
    count_404 = 0
    with open(LOG_FILE, "r") as f:
        for line in f:
            try:
                parts = line.split('"')
                if len(parts) > 2:
                    status = int(parts[2].strip().split()[0])
                    if status == 404:
                        count_404 += 1
            except:
                continue
                
    end = time.time()
    duration = end - start
    print(f"   Python finished in {duration:.4f} seconds.")
    return duration

def run_c_tool():
    print("\nRunning TurboLog (C)...")
    start = time.time()
    
    # Run the compiled C program
    result = subprocess.run([C_TOOL, LOG_FILE, "-s", "404"], capture_output=True, text=True)
    
    end = time.time()
    duration = end - start
    
    if result.returncode != 0:
        print("❌ C Tool Failed! Did you compile it? (gcc turbolog.c -o turbolog)")
        print(result.stderr)
        return None
        
    print(f"   TurboLog finished in {duration:.4f} seconds.")
    return duration

if __name__ == "__main__":
    generate_data()
    
    # 1. Benchmark Python
    py_time = run_python_logic()
    
    # 2. Benchmark C
    if os.path.exists(C_TOOL):
        c_time = run_c_tool()
        
        if c_time:
            speedup = py_time / c_time
            print(f"\nRESULTS: Your C tool is {speedup:.1f}x faster than Python!")
    else:
        print(f"\nCould not find {C_TOOL}. Compile your code first!")