#!/usr/bin/env bash

for i in {1..1000}; do
    echo "Run $i/1000"
    head -c 800 /dev/random | tee test_case_$i.txt | valgrind --leak-check=full --error-exitcode=1 -q ./scalc > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo "ERROR FOUND on run $i! Saved to test_case_$i.txt"
        cat test_case_$i.txt | valgrind --leak-check=full -s ./scalc
        break
    fi
    rm test_case_$i.txt
done && echo "All 1000 runs completed successfully!" || echo "Testing stopped at run with error"
