-- Explode function just for now

function nextBoolean() 
    return (random() / 20000000000 % 2) >= 1
end

-- Pseudorandom

local X = 887712735
local A1 = 710425958647
local B1 = 813638512810
local M1 = 711719770602

function resetSeed()
    X = 887712735
end

function random()
    X = (A1 * X + B1) % M1
    return X
end

function explode(id, x, y, z, strength, hasParent, parent, px, py, pz)

    strengthMedian = strength / 2

    startX = x - strengthMedian
    startY = y + strengthMedian
    startZ = z - strengthMedian

    endX = x + strengthMedian
    endZ = z + strengthMedian
    endY = y - strengthMedian

    fx = 0
    fy = 0
    fz = 0

    canExplosion = false
    blockName = ''
    for sx = startX, endX, 1 do
        for sy = startY, endY, -1 do
            for sz = startZ, endZ, 1 do
                if (hasParent == true) then
                    if (get_block(sx, sy, sz) == block_index(parent)) then
                        explode(id, sx, sy, sz, strength, hasParent, parent, 0, 0, 0)
                    end
                end
                if blockName ~= 'base:bazalt' and (sy ~= endY or nextBoolean()) then
                    fx = sx
                    fy = sy
                    fz = sz

                    if fx == startX then
                        if nextBoolean() then
                            fx = fx + strengthMedian
                        end
                    elseif fx == endX then
                        if nextBoolean() then
                            fx = fx - strengthMedian
                        end
                    end

                    if fz == startZ then
                        if nextBoolean() then
                            fz = fz + strengthMedian
                        end
                    elseif fz == endZ then
                        if nextBoolean() then
                            fz = fz - strengthMedian
                        end
                    end
                    if (fx - x) ^ 2 + (fy - y) ^ 2 + (fz - z) ^ 2 < strengthMedian ^ 2 + 32 then
                        block.set(fx, fy, fz, id, 0)
                    end
                end
            end
        end
    end
end