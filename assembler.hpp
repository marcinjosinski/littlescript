#ifndef SCRIPT_ASSEMBLER_HPP
#define SCRIPT_ASSEMBLER_HPP

#include "utils.hpp"
#include <algorithm>
#include <cstring>
#include <map>
#include <vector>

#ifdef _WIN64
#include <windows.h>
#else
#include <sys/mman.h>
#endif
namespace script::assembler {

// http://ref.x86asm.net/coder32.html
// to complete...
enum opcode : byte {
  rcx = 0xc1,
  rdx = 0xc2,
  edi = 0xc7,
  esi = 0xc6,
  edx = 0xc3,
  ecx = 0xc1,
  ret = 0xc3,
  nop = 0x90,
};

class assembler {
public:
  void emit(byte opcode);
  char *get_code() const;
  void emit_int(int i) {
    std::memcpy(code_ + length_, &i, 4);
    length_ += 4;
  }
protected:
  char *code_;
  std::size_t length_ = 0;
  void *new_page_(std::size_t size);

#ifdef _WIN64
  std::vector<byte> return_registers{opcode::rcx, opcode::rdx};
#else
  std::vector<byte> return_registers{opcode::edi, opcode::esi, opcode::edx,
                                     opcode::ecx};
#endif
};

char *assembler::get_code() const { return code_; }

void assembler::emit(byte opcode) { code_[length_++] = opcode; }

void *assembler::new_page_(std::size_t size) {
#ifdef _WIN64
  return VirtualAlloc(0, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
#else
  return mmap(0, 1024, PROT_READ | PROT_WRITE | PROT_EXEC,
              MAP_ANON | MAP_PRIVATE, -1, 0);
#endif
}

class function : public assembler {
public:
  explicit function(space_size size) : size_{size.get()} {
    code_ = static_cast<char *>(new_page_(size_));

    std::fill(code_, code_ + size_, opcode::nop);

    code_[0] = 0x55;
    code_[1] = 0x48;
    code_[2] = 0x89;
    code_[3] = 0xe5;

    // code_[4] = 0xc7;
    // code_[5] = 0x45;
    // code_[6] = 0xfc;
    // code_[7] = 0x0a;
    // code_[8] = 0x00;
    // code_[9] = 0x00;
    // code_[10] = 0x00;

    // mov eax, ret
    // code_[11] = 0x8b;
    // code_[12] = 0x45;
    // code_[13] = 0xfc;

    code_[1022] = 0x5d; // pop rbp
    code_[1023] = 0xc3; // ret

    length_ = 4;
  }

private:
  std::map<std::string, int> names_;
  std::size_t size_;
};

} // namespace script::assembler

#endif