/* vim: set expandtab ts=4 sw=4: */
/*
 * You may redistribute this program and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "util/Assert.h"
#include "util/platform/libc/string.h"
#include <stdint.h>
#include <stdio.h>

#include "memory/Allocator.h"
#include "memory/Allocator_pvt.h"
#include "memory/MallocAllocator.h"

#ifdef Allocator_USE_CANARIES
    #define ALLOCATION_SIZE sizeof(struct Allocator_Allocation_pvt) + sizeof(long)
#else
    #define ALLOCATION_SIZE sizeof(struct Allocator_Allocation_pvt)
#endif
#define ALLOCATOR_SIZE sizeof(struct Allocator_pvt)

int main()
{
    struct Allocator* alloc = MallocAllocator_new(2048);
    size_t bytesUsed;

    bytesUsed = Allocator_bytesAllocated(alloc);
    Assert_always(bytesUsed == ALLOCATION_SIZE + sizeof(struct Allocator_FirstCtx));
    Allocator_malloc(alloc, 25);
    bytesUsed += (((25 / sizeof(char*)) + 1) * sizeof(char*)) + ALLOCATION_SIZE;
    Assert_always(Allocator_bytesAllocated(alloc) == bytesUsed);

    struct Allocator* child = Allocator_child(alloc);
    bytesUsed += ALLOCATION_SIZE + ALLOCATOR_SIZE;
    Assert_always(Allocator_bytesAllocated(alloc) == bytesUsed);

    Allocator_malloc(child, 30);
    bytesUsed += 32 + ALLOCATION_SIZE;
    Assert_always(Allocator_bytesAllocated(alloc) == bytesUsed);

    Allocator_free(child);
    bytesUsed -= 32 + ALLOCATION_SIZE;
    bytesUsed -= ALLOCATION_SIZE + ALLOCATOR_SIZE;
    Assert_always(Allocator_bytesAllocated(alloc) == bytesUsed);

    Allocator_free(alloc);
    return 0;
}
