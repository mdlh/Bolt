#ifdef cl_amd_printf
    #pragma OPENCL EXTENSION cl_amd_printf : enable
#endif

/*
 * ScanLargeArrays : Scan is done for each block and the sum of each
 * block is stored in separate array (sumBuffer). SumBuffer is scanned
 * and results are added to every value of next corresponding block to
 * compute the scan of a large array.(not limited to 2*MAX_GROUP_SIZE)
 * Scan uses a balanced tree algorithm. See Belloch, 1990 "Prefix Sums
 * and Their Applications"
 * @param output output data
 * @param input  input data
 * @param block  local memory used in the kernel
 * @param sumBuffer  sum of blocks
 * @param length length of the input data
 */

template< typename Type, typename BinaryFunction >
kernel void perBlockAddition( 
                global Type* output,
                global Type* postSumArray,
                const uint vecSize,
                global BinaryFunction* binaryOp
                )
{
    uint gloId = get_global_id( 0 );
    uint groId = get_group_id( 0 );
    uint locId = get_local_id( 0 );

    //  Abort threads that are passed the end of the input vector
    if( gloId >= vecSize )
        return;
        
    Type scanResult = output[ gloId ];

    //  TODO:  verify; is there a memory conflict if all threads read from the same address?
    Type postBlockSum = postSumArray[ groId ];

    // printf( "output[%d] = [%d]\n", gloId, scanResult );
    // printf( "postSumArray[%d] = [%d]\n", groId, postBlockSum );
    scanResult = (*binaryOp)( scanResult, postBlockSum );
    output[ gloId ] = scanResult;
}

template< typename iType, typename BinaryFunction >
kernel void intraBlockExclusiveScan(
                global iType* postSumArray,
                global iType* preSumArray,
                const uint vecSize,
                local volatile iType* lds,
                const uint workPerThread,
                global BinaryFunction* binaryOp    // Functor operation to apply on each step
                )
{
    uint gloId = get_global_id( 0 );
    uint groId = get_group_id( 0 );
    uint locId = get_local_id( 0 );
    uint wgSize = get_local_size( 0 );
    uint mapId  = gloId * workPerThread;

    //    Initialize the padding to 0, for when the scan algorithm looks left.
    //    Then bump the LDS pointer past the padding
    lds[ locId ] = 0;
    local volatile iType* pLDS = lds + ( wgSize / 2 );

    //  Abort threads that are passed the end of the input vector
    //  TODO:  I'm returning early for threads past the input vector size; not safe for barriers in kernel if wg != wavefront
    // if( gloId >= vecSize )
        // return;

    //	Begin the loop reduction
    iType workSum = 0;
    for( uint offset = 0; offset < workPerThread; offset += 1 )
    {
        iType y = preSumArray[ mapId + offset ];
        // printf( "preSumArray[%d] = [%g]\n", mapId + offset, y );
        workSum = (*binaryOp)( workSum, y );
        postSumArray[ mapId + offset ] = workSum;
    }
    barrier( CLK_LOCAL_MEM_FENCE );
    pLDS[ locId ] = workSum;

    //	This loop essentially computes an exclusive scan within a tile, writing 0 out for first element.
    iType scanSum = workSum;
    for( uint offset = 1; offset < wgSize; offset *= 2 )
    {
        barrier( CLK_LOCAL_MEM_FENCE );

        iType y = pLDS[ locId - offset ];
        scanSum = (*binaryOp)( scanSum, y );
        pLDS[ locId ] = scanSum;
    }

    //	Write out the values of the per-tile scan
    scanSum -= workSum;
    for( uint offset = 0; offset < workPerThread; offset += 1 )
    {
        iType y = postSumArray[ mapId + offset ];
        y = (*binaryOp)( y, scanSum );
        y -= preSumArray[ mapId + offset ];
        postSumArray[ mapId + offset ] = y;
        //printf( "postSumArray[%d] = [%g]\n", mapId + offset, postSumArray[ mapId + offset ] );
    }

}

template< typename iType, typename BinaryFunction >
kernel void perBlockInclusiveScan(
                global iType* output,
                global iType* input,
                const uint vecSize,
                local volatile iType* lds,
                global BinaryFunction* binaryOp,    // Functor operation to apply on each step
                global iType* scanBuffer)            // Passed to 2nd kernel; the of each block
{
    uint gloId = get_global_id( 0 );
    uint groId = get_group_id( 0 );
    uint locId = get_local_id( 0 );
    uint wgSize = get_local_size( 0 );

    //    Initialize the padding to 0, for when the scan algorithm looks left.
    //    Then bump the LDS pointer past the padding
    lds[ locId ] = 0;
    local volatile iType* pLDS = lds + ( wgSize / 2 );

    //  Abort threads that are passed the end of the input vector
    //  TODO:  I'm returning early for threads past the input vector size; not safe for barriers in kernel if wg != wavefront
    if( gloId >= vecSize )
        return;

    iType val = input[ gloId ];
    pLDS[ locId ] = val;

    //  This loop essentially computes a scan within a workgroup
    //  No communication between workgroups
    iType sum = val;
    for( unsigned int offset = 1; offset < wgSize; offset *= 2 )
    {
        barrier( CLK_LOCAL_MEM_FENCE );

        iType y = pLDS[ locId - offset ];
        sum = (*binaryOp)( sum, y );
        pLDS[ locId ] = sum;
    }

    //  Each work item writes out its calculated scan result, relative to the beginning
    //  of each work group
    output[ gloId ] = sum;
    // printf( "Output Array work-item[%d], sum[%d]\n", gloId, sum );

    barrier( CLK_LOCAL_MEM_FENCE );

    //  TODO:  verify; is there a memory conflict if all threads write to the same address?
    scanBuffer[ groId ] = pLDS[ wgSize - 1 ];
    // printf( "Block Sum Array work-item[%d], sum[%g]\n", gloId, pLDS[ wgSize - 1 ] );
    
    //	Take the very last thread in a tile, and save its value into a buffer for further processing
    // if( locId == (wgSize-1) )
    // {
        // scanBuffer[ groId ] = pLDS[ locId ];
    // }
}
