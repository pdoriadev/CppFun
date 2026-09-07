walkthrough/tutorial - https://www.cppbetterexplained.com/posts/debugging-cpp-gdb/

# Compiling with Debug Symbols
To run the program with gdb, you need to compile with debug symbols. 
```
# Without debug symbols (debugger can't help much)
g++ -O2 program.cpp -o program

# With debug symbols (what you want)
g++ -g -O0 program.cpp -o program

```
'-O0' removes compiler optimizations that make debugging a pain, i.e. Reorders code or skips variables.
- stack overflow - tldr for fcc optimization options. 
- gcc docs - Options that control optimization: https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html#index-O1


