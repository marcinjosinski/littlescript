# littlescript 

Recursive descent parser based on std::variant that builds simple assembler.

## Example
```cpp
script::parser::parser p;

auto code = std::string{"
function test(param1, param2) {
	int var1; int var2;
	var1 = param1;
	var1 = (10 / (12 / (3 + 1) - 1234)) / (21313 + 21) - 5 - 3 + (3);
	var2 = var1 - 100 + param2;
	while(var1) {
		dec var1;
		var2 = var2 + var1;
	}
	return var2;
}
"};

typedef int(*func)(int, int);
func f = reinterpret_cast<func>(p.generate_function(code));

std::cout << f(10, 12);

```

## Run
```
mkdir build
cd build
cmake --build .
./littlescript
```

Tested on Linux with g++ 7.2.1 and Windows with GCC 7.2.0.

## Useful resources
https://en.wikipedia.org/wiki/X86_calling_conventions  
http://ref.x86asm.net/coder64.html  
https://godbolt.org/  
objdump && NASM

## Inspirations
[Björn Fahller - Lightning Talks Meeting C++ 2017](https://www.youtube.com/watch?v=_7IB1Y27AwE)
https://www.reddit.com/r/cpp/comments/7wvmbi/embedded_compiling_script_engine_in_under_100/



