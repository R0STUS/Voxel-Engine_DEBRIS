function on_update(x, y, z)
	if (get_block(x, y - 1, z) == block_index('core:air') or get_block(x, y - 1, z) == block_index('base:grass') or get_block(x, y - 1, z) == block_index('base:flower') or get_block(x, y - 1, z) == block_index('base:water')) then
		block.set(x, y - 1, z, block_index('base:sand'), states)
		if (get_block(x, y, z + 1) == block_index('base:water') or get_block(x, y, z - 1) == block_index('base:water') or get_block(x - 1, y, z) == block_index('base:water') or get_block(x + 1, y, z) == block_index('base:water')) then
			block.set(x, y, z, block_index('base:water'), states)
		else
			block.set(x, y, z, 0, states)
		end
	end
end

function on_placed(x, y, z)
	if (get_block(x, y - 1, z) == block_index('core:air') or get_block(x, y - 1, z) == block_index('base:grass') or get_block(x, y - 1, z) == block_index('base:flower') or get_block(x, y - 1, z) == block_index('base:water')) then
		block.set(x, y - 1, z, block_index('base:sand'), states)
		if (get_block(x, y, z + 1) == block_index('base:water') or get_block(x, y, z - 1) == block_index('base:water') or get_block(x - 1, y, z) == block_index('base:water') or get_block(x + 1, y, z) == block_index('base:water')) then
			block.set(x, y, z, block_index('base:water'), states)
		else
			block.set(x, y, z, 0, states)
		end
	end
end