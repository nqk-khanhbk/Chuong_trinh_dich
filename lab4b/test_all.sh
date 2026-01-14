#!/bin/bash
# Script test tất cả các examples

echo "========================================"
echo "Testing lab4b - Code Generation Phase 2"
echo "========================================"
echo ""

for i in 1 2 3 4; do
    echo "=== Testing example${i}.kpl ==="
    
    # Compile KPL file to bytecode
    ./kplc tests/example${i}.kpl output${i} 2>&1
    
    if [ $? -eq 0 ]; then
        echo "✓ Compilation successful"
        
        # So sánh bytecode với expected
        if cmp -s output${i} tests/example${i}; then
            echo "✓ Output matches expected bytecode"
        else
            echo "✗ Output differs from expected bytecode"
            echo "  Run: diff <(xxd tests/example${i}) <(xxd output${i})"
        fi
    else
        echo "✗ Compilation failed"
    fi
    echo ""
done

echo "Done!"
