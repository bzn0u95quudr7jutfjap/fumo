#include "kioku.c"
#include <stdio.h>
#include <strings.h>

u0 print_block(ramblock *block) {
  ramblock a = {.begin = 0, .end = 0};
  block = block ? block : &a;
  printf("ramblock@%p{.begin=%d,.end=%d}\n", block, block->begin, block->end);
}

u0 print_status(u8 e) { printf("[%3s] :: ", e ? "OK " : "ERR"); }

u0 test_get_block_kioku(u32 i, ramblock *ptr) {
  ramblock *r = get_block_kioku(i);
  print_status(r == ptr);
  printf("get_block_kioku :: %4d @ %p == %p\n", i, r, ptr);
}

u0 test_get_ptr_index_kioku(u0 *ptr, u32 index) {
  u8 i = get_ptr_index_kioku(ptr);
  print_status(i == index);
  printf("test_get_ptr_index :: %4d == %4d\n", i, index);
}

u8 equals_ramblock(ramblock *a, ramblock *b) {
  if (a == b) {
    return 1;
  }
  if (a == nullptr || b == nullptr) {
    return 0;
  }
  return a->begin == b->begin && a->end == b->end;
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
    e &= equals_ramblock(get_block_kioku(i), &pile[i]);
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

  printf("\n");

  test_get_block_kioku(0, (ramblock *)&g_ram[RAMSIZE - sizeof(ramblock) * 1]);
  test_get_block_kioku(1, (ramblock *)&g_ram[RAMSIZE - sizeof(ramblock) * 2]);
  test_get_block_kioku(2, (ramblock *)&g_ram[RAMSIZE - sizeof(ramblock) * 3]);
  test_get_block_kioku(4, (ramblock *)&g_ram[RAMSIZE - sizeof(ramblock) * 5]);
  test_get_block_kioku(8, (ramblock *)&g_ram[RAMSIZE - sizeof(ramblock) * 9]);
  test_get_block_kioku(6, (ramblock *)&g_ram[RAMSIZE - sizeof(ramblock) * 7]);
  test_get_block_kioku(15, (ramblock *)&g_ram[RAMSIZE - sizeof(ramblock) * 16]);

  printf("\n");

  test_get_ptr_index_kioku(NULL, 0);
  test_get_ptr_index_kioku(nullptr, 0);
  test_get_ptr_index_kioku((u0 *)&g_ram[-128], 0);
  test_get_ptr_index_kioku((u0 *)&g_ram[RAMSIZE + 128], 0);
  test_get_ptr_index_kioku((u0 *)&g_ram[1], 1);
  test_get_ptr_index_kioku((u0 *)&g_ram[128], 128);
  test_get_ptr_index_kioku((u0 *)&g_ram[200], 200);

  printf("\n");

  return 0;
}
