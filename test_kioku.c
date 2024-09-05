#include "kioku.c"
#include <stdio.h>

u0 print_block(ramblock *block) {
  ramblock a = {.begin = 0, .end = 0};
  block = block ? block : &a;
  printf("ramblock@%p{.begin=%d,.end=%d}\n", block, block->begin, block->end);
}

u8 ramblock_equals(ramblock *a, ramblock *b) {
  if (a == b) {
    return 1;
  }
  if (a == nullptr || b == nullptr) {
    return 0;
  }
  return a->begin == b->begin && a->end == b->end;
}

u0 test_get_block_kioku(u32 i, ramblock *ptr) {
  ramblock *r = get_block_kioku(i);
  u8 e = r == ptr;
  printf("get_block_kioku :: %4d @ %p == %p = %d\n", i, r, ptr, e);
}

typedef struct {
  ramblock a[1 << 6];
} ramblock_array;

u0 test_insert_block_kioku(u32 i, u32 bb, u32 be) {
  static ramblock pile[1 << 6] = {};
  for (u32 j = g_allocated_blocks; j > i; i--) {
    pile[j] = pile[j - 1];
  }
  pile[i] = (ramblock){.begin = bb, .end = be};
  insert_block_kioku(i, pile[i]);
  u8 e = 1;
  for (u32 i = 0; i < g_allocated_blocks; i++) {
    e &= ramblock_equals(get_block_kioku(i), &pile[i]);
  }
  if (e) {
    return (u0)printf("test_insert_block :: 1\n");
  }
  printf("test_insert_block :: ERR :: BEGIN\n");
  for (u32 i = 0; i < g_allocated_blocks; i++) {
    printf("test_insert_block :: ERR :: ");
    ramblock *b0 = get_block_kioku(i);
    ramblock *b1 = &pile[i];
    printf("{.b = %4d, .e = %4d} ", b0->begin, b1->end);
    printf("{.b = %4d, .e = %4d} ", b1->begin, b1->end);
    if (b0->begin != b1->begin || b0->end != b1->end) {
      printf("<--");
    }
    printf("\n");
  }
  printf("test_insert_block :: ERR :: END\n");
}

int main(int argc, char *argv[]) {
  test_get_block_kioku(0, (ramblock *)&g_ram[RAMSIZE - sizeof(ramblock) * 1]);
  test_get_block_kioku(1, (ramblock *)&g_ram[RAMSIZE - sizeof(ramblock) * 2]);
  test_get_block_kioku(2, (ramblock *)&g_ram[RAMSIZE - sizeof(ramblock) * 3]);
  test_get_block_kioku(4, (ramblock *)&g_ram[RAMSIZE - sizeof(ramblock) * 5]);
  test_get_block_kioku(8, (ramblock *)&g_ram[RAMSIZE - sizeof(ramblock) * 9]);
  test_get_block_kioku(6, (ramblock *)&g_ram[RAMSIZE - sizeof(ramblock) * 7]);
  test_get_block_kioku(15, (ramblock *)&g_ram[RAMSIZE - sizeof(ramblock) * 16]);
  printf("\n");

  test_insert_block_kioku(0, 1, 2);
  test_insert_block_kioku(0, 1, 3);
  test_insert_block_kioku(2, 1, 4);
  test_insert_block_kioku(2, 1, 4);
  test_insert_block_kioku(5, 1, 4);
  test_insert_block_kioku(2, 1, 6);
  test_insert_block_kioku(3, 1, 7);
  test_insert_block_kioku(3, 9, 12);
  return 0;
}
