data = {}
data["mrbc_gc_b8_count"] = 64
data["mrbc_gc_b8_align_start"] = 0
data["mrbc_gc_b8_align_region_start"] = data["mrbc_gc_b8_align_start"] + (data["mrbc_gc_b8_count"] / 32 * 3)
data["mrbc_gc_b16_count"] = 64
data["mrbc_gc_b16_align_start"] = data["mrbc_gc_b8_align_region_start"]  + data["mrbc_gc_b8_count"] * 2
data["mrbc_gc_b16_align_region_start"] = data["mrbc_gc_b16_align_start"] + (data["mrbc_gc_b16_count"] / 32 * 3)
data["mrbc_gc_b32_count"] = 32
data["mrbc_gc_b32_align_start"] =  data["mrbc_gc_b16_align_region_start"]  + data["mrbc_gc_b16_count"] * 4
data["mrbc_gc_b32_align_region_start"] = data["mrbc_gc_b32_align_start"] + (data["mrbc_gc_b32_count"] / 32 * 3)
data["mrbc_gc_bigger_count"] = 64
data["mrbc_gc_bigger_start"] = data["mrbc_gc_b32_align_region_start"] +  data["mrbc_gc_b32_count"] * 8
data["mrbc_gc_bigger_region_start"] = data["mrbc_gc_bigger_start"]  + (data["mrbc_gc_bigger_count"] / 32 * 4)
data["mrbc_gc_bigger_right"] = data["mrbc_gc_bigger_region_start"] + data["mrbc_gc_bigger_count"] * 8

pp data