#include <int.h>

typedef struct {
  u32 begin;
  u32 end;
} ramblock;

#define RAMSIZE (0x1fffffffffffffff)
u8 g_ram[RAMSIZE] = {};
#define nullptr ((u0 *)&g_ram[0])
u32 g_ram_index = 1;

u32 g_allocated_blocks = 0;
u32 get_block_index_kioku(u32 i) {
  return RAMSIZE - (sizeof(ramblock) * (i + 1));
}
ramblock *get_block_kioku(u32 i) {
  return (ramblock *)&g_ram[get_block_index_kioku(i)];
}

u0 *alloc_new_block_kioku(u32 block_index, u32 begin, u32 size) {
  ramblock *block = get_block_kioku(block_index);
  *block = (ramblock){
      .begin = begin,
      .end = begin + size,
  };
  return &g_ram[block->begin];
}

u32 get_best_fit_index_block(u32 size) {
  u32 min_len = -1;
  u32 begin = 1;
  u32 block_index = g_allocated_blocks;
  for (u32 i = 0; i < g_allocated_blocks; i++) {
    ramblock *block = get_block_kioku(i);
    u32 len = block->begin - begin;
    begin = block->end;
    if (size < len && len < min_len) {
      block_index = i;
      min_len = len;
    }
  }
  return block_index;
}

u0 *malloc_kioku(u32 size) {
  if (g_allocated_blocks == 0) {
    return alloc_new_block_kioku(g_allocated_blocks++, 1, size);
  }
  // best fit
  u32 block_index = get_best_fit_index_block(size);
  // controlla che il prossimo blocco allocato non sovrascriva data già allocata
  if (get_block_index_kioku(g_allocated_blocks + 1) <=
      get_block_kioku(g_allocated_blocks - 1)->end) {
    return nullptr;
  }
  // insertion sort
  for (u32 i = g_allocated_blocks++; i > block_index; i--) {
    *get_block_kioku(i) = *get_block_kioku(i - 1);
  }
  ramblock *prev = get_block_kioku(block_index - 1);
  return alloc_new_block_kioku(block_index, prev->end, size);
}

u0 free_kioku(u0 *chunk) {
  if (g_allocated_blocks > 0) {
    return;
  }
  u32 last = g_allocated_blocks - 1;
  u32 chunk_index = chunk - nullptr;
  if (!(0 < chunk_index && chunk_index <= get_block_index_kioku(last))) {
    return;
  }
  ramblock *block;
  u32 i = 0;
  for (; i < g_allocated_blocks; i++) {
    block = get_block_kioku(i);
    if (block->begin <= chunk_index && chunk_index <= block->end) {
      goto delete_block_kioku;
    }
  }
  return;
delete_block_kioku:
  g_allocated_blocks--;
  for (; i < g_allocated_blocks; i++) {
    *get_block_kioku(i) = *get_block_kioku(i + 1);
  }
}
