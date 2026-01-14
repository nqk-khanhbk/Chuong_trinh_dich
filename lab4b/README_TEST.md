# Hướng dẫn Test Lab4b - Code Generation Phase 2

## 📋 Tổng quan
Project này implement code generation cho KPL compiler, tạo bytecode từ source code KPL.

## 🔧 Build project

```bash
make clean
make
```

## 🧪 Các cách test

### 1️⃣ Test tất cả examples (Recommended)

```bash
./test_all.sh
```

Output mẫu:
```
========================================
Testing lab4b - Code Generation Phase 2
========================================

=== Testing example1.kpl ===
✓ Compilation successful
✓ Output matches expected bytecode

=== Testing example2.kpl ===
✓ Compilation successful
✓ Output matches expected bytecode
...
```

### 2️⃣ Test từng example với code dump

```bash
./test_dump.sh <số>
```

Ví dụ:
```bash
./test_dump.sh 1    # Test example1.kpl
./test_dump.sh 2    # Test example2.kpl
./test_dump.sh 3    # Test example3.kpl
./test_dump.sh 4    # Test example4.kpl
```

Output sẽ hiển thị:
- Các instructions bytecode được generate
- Source code KPL tương ứng

### 3️⃣ Test manual từng file

```bash
# Compile KPL file
./kplc tests/example1.kpl output1

# Compile với dump (xem instructions)
./kplc tests/example1.kpl output1 -dump

# So sánh với expected bytecode
cmp output1 tests/example1
# Hoặc xem diff chi tiết
diff <(xxd tests/example1) <(xxd output1)
```

## 📝 Các test cases

### Example 1: Empty program
```kpl
Program Example1; 
Begin
End.
```
Test cơ bản nhất: program structure và halt

### Example 2: If-Then-Else + Function calls
```kpl
Program Example2;
Var n : Integer;
Begin
  n := ReadI;
  If (n - (n/2) * 2) = 0
  Then Call WriteC('E')
  Else Call WriteC('O');
  Call WriteLN;
End.
```
Test:
- Variable assignment
- Arithmetic expressions (-, /, *)
- Comparison operators (=)
- If-then-else control flow
- Predefined function/procedure calls (ReadI, WriteC, WriteLN)

### Example 3: While loop
```kpl
Program Example3;
Var n, i, S : Integer;
Begin
  n := ReadI;
  S := 0;
  i := 1;
  While i <= n Do
    Begin
      S := S + i;
      i := i + 1;
    End;
  Call WriteI(S);
  Call WriteLN;
End.
```
Test:
- Multiple variables
- While loop với condition
- Compound statements (Begin...End)
- Loop control flow

### Example 4: For loop
```kpl
Program Example4;
Var n, i, S : Integer;
Begin
  n := ReadI;
  S := 0;
  For i:=1 To n Do
    S := S + i;
  Call WriteI(S);
  Call WriteLN;
End.
```
Test:
- For loop implementation
- Loop variable management
- Stack frame manipulation (CV, DCT)

## 🔍 Các instructions bytecode

| Instruction | Ý nghĩa |
|------------|---------|
| **J addr** | Jump to address |
| **FJ addr** | False Jump (jump if top of stack is 0) |
| **INT n** | Increase stack top by n words |
| **DCT n** | Decrease stack top by n words |
| **LA level,offset** | Load Address |
| **LV level,offset** | Load Value |
| **LC const** | Load Constant |
| **LI** | Load Indirect |
| **ST** | Store |
| **CV** | Copy Value on stack |
| **AD** | Add (pop 2, push sum) |
| **SB** | Subtract |
| **ML** | Multiply |
| **DV** | Divide |
| **NEG** | Negate |
| **EQ, NE, LT, LE, GT, GE** | Comparisons |
| **RI** | Read Integer |
| **RC** | Read Char |
| **WRI** | Write Integer |
| **WRC** | Write Char |
| **WLN** | Write Line |
| **HL** | Halt program |

## ✅ Kết quả test hiện tại

**Status: ALL TESTS PASSED ✓**

Tất cả 4 examples compile thành công và bytecode khớp với expected output.

## 🐛 Debug tips

Nếu test fail:

1. **Xem diff bytecode:**
   ```bash
   diff <(xxd tests/example2) <(xxd output2)
   ```

2. **Xem instructions được generate:**
   ```bash
   ./kplc tests/example2.kpl output2 -dump
   ```

3. **Check compilation errors:**
   ```bash
   ./kplc tests/example2.kpl output2 2>&1 | tee error.log
   ```

4. **So sánh từng instruction:**
   ```bash
   xxd tests/example2 > expected.hex
   xxd output2 > actual.hex
   diff expected.hex actual.hex
   ```

## 📊 Frame layout

```
Stack frame structure:
0: RV  (Return Value)
1: DL  (Dynamic Link)
2: RA  (Return Address)
3: SL  (Static Link)
4+: Parameters and Local Variables
```

## 🎯 Các features đã implement

✅ Basic expressions (arithmetic, comparison)
✅ Variable assignment
✅ If-Then-Else statements
✅ While loops
✅ For loops
✅ Predefined functions/procedures (ReadI/C, WriteI/C, WriteLN)
✅ Stack frame management
✅ Code address tracking

## ⚠️ Chưa implement (có TODO/TEMPORARY trong code)

- Array indexing (`compileIndexes` - line 918)
- User-defined function calls (`genFunctionCall` - line 44 codegen.c)
- User-defined procedure calls (line 470 parser.c)
- Parameter assignment (line 425 parser.c)
- Function name assignment (line 430 parser.c)

Các phần này chưa cần thiết cho 4 test cases hiện tại.
