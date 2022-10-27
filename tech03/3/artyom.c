#include <stdio.h>
#include <stdint.h>
#include <windows.h>

struct Item {
  int32_t value;
  DWORD next_pointer;
};

int main(int argc, char *argv[]) {
  const int32_t EMPTY_READ = 0;
  const int32_t ERR_CODE = -1;
  const ssize_t INPUT_FILE_COUNT = 2;
  const ssize_t INPUT_FILE = 1;

  DWORD shift_code = 0;
  DWORD read_len = 0;
  DWORD curr_pos = 0;

  if (argc < INPUT_FILE_COUNT) {
    return ERR_CODE;
  }

  HANDLE input_file = CreateFileA(argv[INPUT_FILE], GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

  if (input_file == INVALID_HANDLE_VALUE) {
    return ERR_CODE;
  }

  do {
    struct Item input_item;
    shift_code = SetFilePointer(input_file, curr_pos, NULL, FILE_BEGIN);
    ReadFile(input_file, &input_item, sizeof(input_item), &read_len, NULL);
    if (read_len == EMPTY_READ) {
      return ERR_CODE;
    }
    printf("%d ", input_item.value);
    curr_pos = input_item.next_pointer;
  } while (curr_pos != 0);

  return 0;
}