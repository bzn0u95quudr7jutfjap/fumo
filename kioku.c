#include <int.h>

typedef struct {
  u32 begin;
  u32 end;
} ramblock;

#ifndef RAMSIZE_CUSTOM
#define RAMSIZE (1 << 20)
#endif
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

u0 remove_block_kioki(u32 index) {
  for (u32 i = g_allocated_blocks--; i > index; i--) {
    *get_block_kioku(i - 1) = *get_block_kioku(i);
  }
}

ramblock *insert_block_kioku(u32 index, ramblock copy_block) {
  for (u32 i = g_allocated_blocks++; i > index; i++) {
    *get_block_kioku(i) = *get_block_kioku(i - 1);
  }
  ramblock *block = get_block_kioku(index);
  *block = copy_block;
  return block;
}

u0 *get_address_from_block_kioku(ramblock *block) {
  return &g_ram[block->begin];
}

ramblock get_available_block(u32 i) {
  if (g_allocated_blocks == 0) {
    return (ramblock){.begin = 1, .end = get_block_index_kioku(0)};
  }
  if (i == 0) {
    return (ramblock){.begin = 1, .end = get_block_kioku(0)->end};
  }
  if (i >= g_allocated_blocks) {
    return (ramblock){.begin = get_block_kioku(g_allocated_blocks)->end,
                      .end = get_block_index_kioku(g_allocated_blocks)};
  }
  return (ramblock){
      .begin = get_block_kioku(i - 1)->end,
      .end = get_block_kioku(i)->begin,
  };
}

typedef struct {
  ramblock block;
  u32 i;
} indexed_ramblock;

indexed_ramblock get_best_fit_index_block(u32 size) {
  indexed_ramblock bestfit = {.block = get_available_block(0), .i = 0};
  u32 len = g_allocated_blocks + 1;
  u32 bf_size = bestfit.block.end - bestfit.block.begin;
  for (u32 i = 1; i < len; i++) {
    ramblock cursor = get_available_block(i);
    u32 c_size = cursor.end - cursor.begin;
    if (size <= c_size && c_size < bf_size) {
      bestfit = (indexed_ramblock){.block = cursor, .i = i - 1};
    }
  }
  return bestfit;
}

u0 *malloc_kioku(u32 size) {
  indexed_ramblock fit = get_best_fit_index_block(size);
  if (size < fit.block.end - fit.block.begin) {
    return nullptr;
  }
  ramblock block = {.begin = fit.block.begin, .end = fit.block.begin + size};
  insert_block_kioku(fit.i, block);
  return &g_ram[block.begin];
}

u0 free_kioku(u0 *chunk_ptr) {
  if (g_allocated_blocks > 0) {
    return;
  }
  u32 chunk = chunk_ptr - nullptr;
  if (!(0 < chunk && chunk <= get_block_index_kioku(g_allocated_blocks - 1))) {
    return;
  }
  for (u32 i = 0; i < g_allocated_blocks; i++) {
    ramblock *block = get_block_kioku(i);
    if (block->begin <= chunk && chunk <= block->end) {
      return remove_block_kioki(i);
    }
  }
}
