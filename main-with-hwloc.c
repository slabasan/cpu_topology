#include <stdio.h>
#include <stdlib.h>
#include <hwloc.h>

struct map
{
    int physical_core_idx;
};

static void get_children_per_core(hwloc_topology_t topology, hwloc_obj_t obj, int curr_depth, int core_depth, int pu_depth, int log_idx)
{
    unsigned i;
    hwloc_obj_t core_obj, package_obj;
    int pdepth;

    core_obj = hwloc_get_obj_by_type(topology, HWLOC_OBJ_CORE, log_idx);
    if (curr_depth != core_depth)
    {   
        //printf("RRR coredepth=%d threaddepth=%d\n", core_obj->depth, obj->depth);
        pdepth = hwloc_get_type_depth(topology, HWLOC_OBJ_PACKAGE);
        package_obj = hwloc_get_ancestor_obj_by_depth(topology, pdepth, obj);
        
        printf("%d | %d | %d | %d | %d\n", package_obj->logical_index, core_obj->logical_index, obj->os_index, obj->logical_index, obj->sibling_rank);
    }
    for (i = 0; i < obj->arity; i++)
    {
        get_children_per_core(topology, obj->children[i], curr_depth + 1, core_depth, pu_depth, log_idx);
    }
}

void hwloc_topology(int* nsockets, int* ncores, int* nthreads)
{
    hwloc_topology_t topology;
    hwloc_obj_t obj;
    unsigned int i;
    unsigned int core_depth, pu_depth, socket_depth;

    hwloc_topology_init(&topology);
    hwloc_topology_load(topology);
    
    core_depth = hwloc_get_type_depth(topology, HWLOC_OBJ_CORE);
    pu_depth = hwloc_get_type_depth(topology, HWLOC_OBJ_PU);
    socket_depth = hwloc_get_type_depth(topology, HWLOC_OBJ_PACKAGE);

    printf("pudepth=%d coredepth=%d socketdepth=%d\n", pu_depth, core_depth, socket_depth);

    *nsockets = hwloc_get_nbobjs_by_type(topology, HWLOC_OBJ_SOCKET);
    *ncores = hwloc_get_nbobjs_by_type(topology, HWLOC_OBJ_CORE);
    *nthreads = hwloc_get_nbobjs_by_type(topology, HWLOC_OBJ_PU);

    printf("package | cpu_logical_idx | thread_os_idx | thread_logical_idx | thread_sibling\n");
    for (i = 0; i < *ncores; i++)
    {
        obj = hwloc_get_obj_by_type(topology, HWLOC_OBJ_CORE, i);
        get_children_per_core(topology, obj, obj->depth, core_depth, pu_depth, i);
    }
    
    hwloc_topology_destroy(topology);
}

int main(int argc, char **argv)
{
    static int init = 0;
    int numcores, numthreads, numsockets;

    if (!init)
    {
        hwloc_topology(&numsockets, &numcores, &numthreads);
        init = 1;
    }
    
    printf("num sockets = %d\n", numsockets);
    printf("num cores per socket = %d\n", numcores/numsockets);
    printf("num threads per core = %d\n", numthreads/numcores);

    return 0;
}
