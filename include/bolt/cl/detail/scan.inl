/***************************************************************************
*   Copyright 2012 - 2013 Advanced Micro Devices, Inc.
*
*   Licensed under the Apache License, Version 2.0 (the "License");
*   you may not use this file except in compliance with the License.
*   You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.

***************************************************************************/
/******************************************************************************
 * OpenCL Scan
 *****************************************************************************/
#if !defined( BOLT_CL_SCAN_INL )
#define BOLT_CL_SCAN_INL
//#ifndef USE_AMD_HSA
#define USE_AMD_HSA 0
//#endif

#if USE_AMD_HSA
#define HSA_STAT_INIT 0 // device hasn't done pre-scan
#define HSA_STAT_DEVP1COMPLETE 1 // device has done pre-scan
#define HSA_STAT_CPUP2COMPLETE 2 // cpu has done intermediate scan
#define HSA_STAT_DEVP3COMPLETE 3 // gpu has done post scan
#endif


#pragma once



#define KERNEL02WAVES 4
#define KERNEL1WAVES 4
#define HSAWAVES 4
#define WAVESIZE 64

#ifdef BOLT_PROFILER_ENABLED
#include "bolt/AsyncProfiler.h"
//AsyncProfiler aProfiler("transform_scan");
#endif

#include <algorithm>
#include <type_traits>
#include "bolt/cl/bolt.h"


#ifdef ENABLE_TBB
//TBB Includes
#include "bolt/btbb/scan.h"

#endif


#ifdef BOLT_PROFILER_ENABLED
#define BOLT_PROFILER_START_TRIAL \
    aProfiler.setName("Scan"); \
    aProfiler.startTrial(); \
    aProfiler.setStepName("Origin"); \
    aProfiler.set(AsyncProfiler::device, control::SerialCpu);
#define BOLT_PROFILER_STOP_TRIAL \
    aProfiler.stopTrial();
#else
#define BOLT_PROFILER_START_TRIAL
#define BOLT_PROFILER_STOP_TRIAL
#endif

namespace bolt
{
namespace cl
{
//////////////////////////////////////////
//  Inclusive scan overloads
//////////////////////////////////////////
template< typename InputIterator, typename OutputIterator >
OutputIterator inclusive_scan(
    InputIterator first,
    InputIterator last,
    OutputIterator result,
    const std::string& user_code )
{
    typedef std::iterator_traits<InputIterator>::value_type iType;
    iType init; memset(&init, 0, sizeof(iType) );
    return detail::scan_detect_random_access(
           control::getDefault( ), first, last, result, init, true, plus< iType >( ),
           std::iterator_traits< InputIterator >::iterator_category( ) );
};

template< typename InputIterator, typename OutputIterator, typename BinaryFunction >
OutputIterator inclusive_scan(
    InputIterator first,
    InputIterator last,
    OutputIterator result,
    BinaryFunction binary_op,
            const std::string& user_code )
{
    typedef std::iterator_traits<InputIterator>::value_type iType;
    iType init; memset(&init, 0, sizeof(iType) );
    return detail::scan_detect_random_access(
           control::getDefault( ), first, last, result, init, true, binary_op,
           std::iterator_traits< InputIterator >::iterator_category( ) );
};

template< typename InputIterator, typename OutputIterator >
OutputIterator inclusive_scan(
    control &ctrl,
    InputIterator first,
    InputIterator last,
    OutputIterator result,
            const std::string& user_code )
{
    typedef std::iterator_traits<InputIterator>::value_type iType;
    iType init; memset(&init, 0, sizeof(iType) );
    return detail::scan_detect_random_access(
           ctrl, first, last, result, init, true, plus< iType >( ),
           std::iterator_traits< InputIterator >::iterator_category( ) );
};

template< typename InputIterator, typename OutputIterator, typename BinaryFunction >
OutputIterator inclusive_scan(
    control &ctrl,
    InputIterator first,
    InputIterator last,
    OutputIterator result,
    BinaryFunction binary_op,
            const std::string& user_code )
{
    typedef std::iterator_traits<InputIterator>::value_type iType;
    iType init; memset(&init, 0, sizeof(iType) );
    return detail::scan_detect_random_access(
           ctrl, first, last, result, init, true, binary_op,
           std::iterator_traits< InputIterator >::iterator_category( ) );
};

//////////////////////////////////////////
//  Exclusive scan overloads
//////////////////////////////////////////
template< typename InputIterator, typename OutputIterator >
OutputIterator exclusive_scan(
    InputIterator first,
    InputIterator last,
    OutputIterator result,
    const std::string& user_code )
{
    typedef std::iterator_traits<InputIterator>::value_type iType;
    iType init; memset(&init, 0, sizeof(iType) );
    return detail::scan_detect_random_access(
           control::getDefault( ), first, last, result, init, false, plus< iType >( ),
           std::iterator_traits< InputIterator >::iterator_category( ) );
};

template< typename InputIterator, typename OutputIterator, typename T >
OutputIterator exclusive_scan(
    InputIterator first,
    InputIterator last,
    OutputIterator result,
    T init,
    const std::string& user_code )
{
    typedef std::iterator_traits<InputIterator>::value_type iType;
    return detail::scan_detect_random_access(
           control::getDefault( ), first, last, result, init, false, plus< iType >( ),
           std::iterator_traits< InputIterator >::iterator_category( ) );
};

template< typename InputIterator, typename OutputIterator, typename T, typename BinaryFunction >
OutputIterator exclusive_scan(
    InputIterator first,
    InputIterator last,
    OutputIterator result,
    T init,
    BinaryFunction binary_op,
            const std::string& user_code )
{
    return detail::scan_detect_random_access(
           control::getDefault( ), first, last, result, init, false, binary_op,
           std::iterator_traits< InputIterator >::iterator_category( ) );
};

template< typename InputIterator, typename OutputIterator >
OutputIterator exclusive_scan(
    control &ctrl,
    InputIterator first,
    InputIterator last,
    OutputIterator result,
            const std::string& user_code ) // assumes addition of numbers
{
    typedef std::iterator_traits<InputIterator>::value_type iType;
    iType init = static_cast< iType >( 0 );
    return detail::scan_detect_random_access(
           ctrl, first, last, result, init, false, plus< iType >( ),
           std::iterator_traits< InputIterator >::iterator_category( ) );
};

template< typename InputIterator, typename OutputIterator, typename T >
OutputIterator exclusive_scan(
    control &ctrl,
    InputIterator first,
    InputIterator last,
    OutputIterator result,
    T init,
            const std::string& user_code )
{
    typedef std::iterator_traits<InputIterator>::value_type iType;
    return detail::scan_detect_random_access(
           ctrl, first, last, result, init, false, plus< iType >( ),
           std::iterator_traits< InputIterator >::iterator_category( ) );
};

template< typename InputIterator, typename OutputIterator, typename T, typename BinaryFunction >
OutputIterator exclusive_scan(
    control &ctrl,
    InputIterator first,
    InputIterator last,
    OutputIterator result,
    T init,
    BinaryFunction binary_op,
            const std::string& user_code )
{
    return detail::scan_detect_random_access(
           ctrl, first, last, result, init, false, binary_op,
           std::iterator_traits< InputIterator >::iterator_category( ) );
};

template<
    typename vType,
    typename oType,
    typename BinaryFunction,
    typename T>
oType*
Serial_scan(
    vType *values,
    oType *result,
    unsigned int  num,
    const BinaryFunction binary_op,
    const bool Incl,
    const T &init)
{
    vType  sum, temp;
    if(Incl){
      *result = *values; // assign value
      sum = *values;
    }
    else {
       temp = *values;
       *result = (oType)init;
       sum = binary_op( *result, temp);
    }
    for ( unsigned int i= 1; i<num; i++)
    {
        vType currentValue = *(values + i); // convertible
        if (Incl)
        {
            vType r = binary_op( sum, currentValue);
            *(result + i) = r;
            sum = r;
        }
        else // new segment
        {
            *(result + i) = sum;
            sum = binary_op( sum, currentValue);

        }
    }
    return result;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace detail
{

enum scanTypes {scan_iValueType, scan_iIterType, scan_oValueType, scan_oIterType, scan_initType,
    scan_BinaryFunction, scan_end };

class Scan_KernelTemplateSpecializer : public KernelTemplateSpecializer
{
public:
    Scan_KernelTemplateSpecializer() : KernelTemplateSpecializer()
        {
#if USE_AMD_HSA
        addKernelName("HSA_Scan");
#else
        addKernelName("perBlockInclusiveScan");
        addKernelName("intraBlockInclusiveScan");
        addKernelName("perBlockAddition");
#endif
    }

    const ::std::string operator() ( const ::std::vector<::std::string>& typeNames ) const
            {
#if USE_AMD_HSA
                    const std::string templateSpecializationString =
                        "// Dynamic specialization of generic template definition, using user supplied types\n"
            "template __attribute__((mangled_name(" + name(0) + "Instantiated)))\n"
            "__attribute__((reqd_work_group_size(KERNEL0WORKGROUPSIZE,1,1)))\n"
            "kernel void " + name(0) + "(\n"
            "global " + typeNames[scan_oType] + " *output,\n"
            "global " + typeNames[scan_iType] + " *input,\n"
            ""        + typeNames[scan_initType] + " init,\n"
            "const uint numElements,\n"
            "const uint numIterations,\n"
            "local "  + typeNames[scan_oType] + " *lds,\n"
            "global " + typeNames[scan_BinaryFunction] + " *binaryOp,\n"
            "global " + typeNames[scan_oType] + " *intermediateScanArray,\n"
            "global int *status1,\n"
            "global int *status2,\n"
            "int exclusive\n"
            ");\n\n";
#else
        const std::string templateSpecializationString =
            "// Template specialization\n"
            "template __attribute__((mangled_name(" + name(0) + "Instantiated)))\n"
            "__attribute__((reqd_work_group_size(KERNEL0WORKGROUPSIZE,1,1)))\n"
            "kernel void " + name(0) + "(\n"
            "global " + typeNames[scan_iValueType] + "* input,\n"
            ""        + typeNames[scan_iIterType] + " input_iter,\n"
            ""        + typeNames[scan_initType] + " identity,\n"
            "const uint vecSize,\n"
            "local "  + typeNames[scan_iValueType] + "* lds,\n"
            "global " + typeNames[scan_BinaryFunction] + "* binaryOp,\n"
            "global " + typeNames[scan_iValueType] + "* scanBuffer,\n"
            "global " + typeNames[scan_iValueType] + "* scanBuffer1,\n"
            "int exclusive\n"
            ");\n\n"

            "// Template specialization\n"
            "template __attribute__((mangled_name(" + name(1) + "Instantiated)))\n"
            "__attribute__((reqd_work_group_size(KERNEL1WORKGROUPSIZE,1,1)))\n"
            "kernel void " + name(1) + "(\n"
            "global " + typeNames[scan_iValueType] + "* postSumArray,\n"
            "global " + typeNames[scan_iValueType] + "* preSumArray,\n"
            ""        + typeNames[scan_initType]+" identity,\n"
            "const uint vecSize,\n"
            "local " + typeNames[scan_iValueType] + "* lds,\n"
            "const uint workPerThread,\n"
            "global " + typeNames[scan_BinaryFunction] + "* binaryOp\n"
            ");\n\n"

            "// Template specialization\n"
            "template __attribute__((mangled_name(" + name(2) + "Instantiated)))\n"
            "__attribute__((reqd_work_group_size(KERNEL2WORKGROUPSIZE,1,1)))\n"
            "kernel void " + name(2) + "(\n"
            "global " + typeNames[scan_oValueType] + "* output_ptr,\n"
            ""        + typeNames[scan_oIterType] + " output_iter,\n"
            "global " + typeNames[scan_iValueType] + "* input,\n"
            ""        + typeNames[scan_iIterType] + " input_iter,\n"
            "global " + typeNames[scan_iValueType] + "* postSumArray_ptr,\n"
            "global " + typeNames[scan_iValueType] + "* preSumArray_ptr1,\n"
            "local " + typeNames[scan_iValueType] + "* lds,\n"
            "const uint vecSize,\n"
            "global " + typeNames[scan_BinaryFunction] + "* binaryOp,\n"
            "int exclusive,\n"
             ""        + typeNames[scan_initType] + " identity\n"
            ");\n\n";
#endif
            return templateSpecializationString;
            }
};


        template< typename InputIterator, typename OutputIterator, typename T, typename BinaryFunction >
        OutputIterator scan_detect_random_access(
            control &ctrl,
            const InputIterator& first,
            const InputIterator& last,
            const OutputIterator& result,
            const T& init,
            const bool& inclusive,
            BinaryFunction binary_op,
            std::input_iterator_tag )
        {
            //  TODO:  It should be possible to support non-random_access_iterator_tag iterators, if we copied the data
            //  to a temporary buffer.  Should we?
            static_assert( false, "Bolt only supports random access iterator types" );
        };

        template< typename InputIterator, typename OutputIterator, typename T, typename BinaryFunction >
        OutputIterator scan_detect_random_access( control &ctrl, const InputIterator& first, const InputIterator& last,
            const OutputIterator& result,  const T& init, const bool& inclusive,
            const BinaryFunction& binary_op, std::random_access_iterator_tag )
        {
            return detail::scan_pick_iterator( ctrl, first, last, result, init, inclusive, binary_op,
                std::iterator_traits< InputIterator >::iterator_category( ),
                std::iterator_traits< OutputIterator >::iterator_category( ) );
        };

        template< typename InputIterator, typename OutputIterator, typename T, typename BinaryFunction >
        OutputIterator scan_pick_iterator( control &ctl, const InputIterator& first, const InputIterator& last,
            const OutputIterator& result, const T& init, const bool& inclusive, const BinaryFunction& binary_op,
            std::random_access_iterator_tag, bolt::cl::fancy_iterator_tag )
        {
            static_assert( false, "It is not possible to output to fancy iterators; they are not mutable" );
        }

        /*!
        * \brief This overload is called strictly for non-device_vector iterators
        * \details This template function overload is used to seperate device_vector iterators from all other iterators
        */
        template< typename InputIterator, typename OutputIterator, typename T, typename BinaryFunction >
        OutputIterator scan_pick_iterator( control &ctrl, const InputIterator& first, const InputIterator& last,
            const OutputIterator& result, const T& init, const bool& inclusive, const BinaryFunction& binary_op,
            std::random_access_iterator_tag, std::random_access_iterator_tag )
        {
            typedef typename std::iterator_traits< InputIterator >::value_type iType;
            typedef typename std::iterator_traits< OutputIterator >::value_type oType;
            static_assert( std::is_convertible< iType, oType >::value, "Input and Output iterators are incompatible" );

            unsigned int numElements = static_cast< unsigned int >( std::distance( first, last ) );
            if( numElements < 1 )
                return result;

            bolt::cl::control::e_RunMode runMode = ctrl.getForceRunMode( );

            if( runMode == bolt::cl::control::Automatic )
            {
                runMode = ctrl.getDefaultPathToRun( );
            }

            if( runMode == bolt::cl::control::SerialCpu )
            {
#ifdef BOLT_PROFILER_ENABLED
aProfiler.startTrial();
aProfiler.setStepName("serial");
aProfiler.set(AsyncProfiler::device, control::SerialCpu);
#endif
                 Serial_scan<iType, oType, BinaryFunction, T>(&(*first), &(*result), numElements, binary_op,inclusive,
                                                                                                                init);

#ifdef BOLT_PROFILER_ENABLED
aProfiler.setDataSize(numElements*sizeof(iType));
aProfiler.stopTrial();
#endif

                return result + numElements;
            }
            else if( runMode == bolt::cl::control::MultiCoreCpu )
            {
#ifdef ENABLE_TBB

               if(inclusive)
               {
                 return bolt::btbb::inclusive_scan(first, last, result, binary_op);
               }
               else
               {
                return bolt::btbb::exclusive_scan( first, last, result, init, binary_op);
               }


#else
               throw std::exception( "The MultiCoreCpu version of Scan is not enabled to be built! \n" );

#endif
            }
            else
            {
#ifdef BOLT_PROFILER_ENABLED
aProfiler.startTrial();
aProfiler.setStepName("Mapping Buffers to Device");
aProfiler.set(AsyncProfiler::device, control::OpenCL);
aProfiler.set(AsyncProfiler::memory, numElements*sizeof(iType));
#endif
                // Map the input iterator to a device_vector
                device_vector< iType > dvInput( first, last,  CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE, ctrl );
                device_vector< oType > dvOutput(result,numElements,CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,false,ctrl);

                //Now call the actual cl algorithm
                scan_enqueue( ctrl, dvInput.begin( ), dvInput.end( ), dvOutput.begin( ), init, binary_op, inclusive );

                // This should immediately map/unmap the buffer
                dvOutput.data( );

#ifdef BOLT_PROFILER_ENABLED
aProfiler.setDataSize(numElements*sizeof(iType));
aProfiler.stopTrial();
#endif


            }

            return result + numElements;
}

        /*!
        * \brief This overload is called strictly for device_vector iterators
        * \details This template function overload is used to seperate device_vector iterators from all other iterators
        */
        template< typename DVInputIterator, typename DVOutputIterator, typename T, typename BinaryFunction >
        DVOutputIterator scan_pick_iterator( control &ctrl, const DVInputIterator& first, const DVInputIterator& last,
            const DVOutputIterator& result, const T& init, const bool& inclusive, const BinaryFunction& binary_op,
            bolt::cl::device_vector_tag, bolt::cl::device_vector_tag )
        {
            typedef typename std::iterator_traits< DVInputIterator >::value_type iType;
            typedef typename std::iterator_traits< DVOutputIterator >::value_type oType;
            static_assert( std::is_convertible< iType, oType >::value, "Input and Output iterators are incompatible" );

            unsigned int numElements = static_cast< unsigned int >( std::distance( first, last ) );
            if( numElements < 1 )
                return result;

            bolt::cl::control::e_RunMode runMode = ctrl.getForceRunMode( );

            if( runMode == bolt::cl::control::Automatic )
            {
                runMode = ctrl.getDefaultPathToRun( );
            }

            if( runMode == bolt::cl::control::SerialCpu )
            {
                bolt::cl::device_vector< iType >::pointer scanInputBuffer =  first.getContainer( ).data( );
                bolt::cl::device_vector< oType >::pointer scanResultBuffer =  result.getContainer( ).data( );
                Serial_scan<iType, oType, BinaryFunction, T>(&scanInputBuffer[first.m_Index], &scanResultBuffer[result.m_Index],
                                                              numElements, binary_op, inclusive, init);
                return result + numElements;
            }
            else if( runMode == bolt::cl::control::MultiCoreCpu )
            {
#ifdef ENABLE_TBB
                bolt::cl::device_vector< iType >::pointer scanInputBuffer =  first.getContainer( ).data( );
                bolt::cl::device_vector< oType >::pointer scanResultBuffer =  result.getContainer( ).data( );

               if(inclusive)
                 bolt::btbb::inclusive_scan(&scanInputBuffer[first.m_Index], &scanInputBuffer[first.m_Index] + numElements, &scanResultBuffer[result.m_Index], binary_op);
               else
                 bolt::btbb::exclusive_scan( &scanInputBuffer[first.m_Index], &scanInputBuffer[first.m_Index] + numElements, &scanResultBuffer[result.m_Index], init, binary_op);

                return result + numElements;
#else
                //std::cout << "The MultiCoreCpu version of Scan with device vector is not enabled" << std ::endl;
         throw std::exception(  "The MultiCoreCpu version of Scan with device vector is not enabled to be built! \n" );

#endif
            }
            else{
            //Now call the actual cl algorithm
               scan_enqueue( ctrl, first, last, result, init, binary_op, inclusive );
            }

            return result + numElements;
        }

        /*!
        * \brief This overload is called strictly for non-device_vector iterators
        * \details This template function overload is used to seperate device_vector iterators from all other iterators
        */
        template< typename InputIterator, typename OutputIterator, typename T, typename BinaryFunction >
        OutputIterator scan_pick_iterator(control &ctrl,const InputIterator& fancyFirst,const InputIterator& fancyLast,
            const OutputIterator& result, const T& init, const bool& inclusive, const BinaryFunction& binary_op,
            bolt::cl::fancy_iterator_tag, std::random_access_iterator_tag )
        {
            typedef typename std::iterator_traits< InputIterator >::value_type iType;
            typedef typename std::iterator_traits< OutputIterator >::value_type oType;
            static_assert( std::is_convertible< iType, oType >::value, "Input and Output iterators are incompatible" );

            unsigned int numElements = static_cast< unsigned int >( std::distance( fancyFirst, fancyLast ) );
            if( numElements == 0 )
                return result;

            bolt::cl::control::e_RunMode runMode = ctrl.getForceRunMode( );

            if( runMode == bolt::cl::control::Automatic )
            {
                runMode = ctrl.getDefaultPathToRun( );
            }

            if( runMode == bolt::cl::control::SerialCpu )
            {
                Serial_scan<iType, oType, BinaryFunction,T>(&(*fancyFirst), &(*result), numElements, binary_op,
                                                                                            inclusive, init);
                return result + numElements;
            }
            else if( runMode == bolt::cl::control::MultiCoreCpu )
            {
#ifdef ENABLE_TBB

               if(inclusive)
               {
                 return bolt::btbb::inclusive_scan(first, last, result, binary_op);
               }
               else
               {
                return bolt::btbb::exclusive_scan( first, last, result, init, binary_op);
               }

#else
               //std::cout << "The MultiCoreCpu version of Scan is not implemented yet." << std ::endl;
               throw std::exception("The MultiCoreCpu version of Scan is not enabled to be built! \n" );

#endif
            }
            else
            {

                //Now call the actual cl algorithm
                scan_enqueue( ctrl, fancyFirst, fancyLast, result /*dvOutput.begin( )*/, init, binary_op, inclusive );

                // This should immediately map/unmap the buffer
                /*dvOutput.data( );*/
            }

            return result + numElements;
        }

static int tmp = 0;

//  All calls to inclusive_scan end up here, unless an exception was thrown
//  This is the function that sets up the kernels to compile (once only) and execute
template< typename DVInputIterator, typename DVOutputIterator, typename T, typename BinaryFunction >
void scan_enqueue(
    control &ctrl,
    const DVInputIterator& first,
    const DVInputIterator& last,
    const DVOutputIterator& result,
    const T& init_T,
    const BinaryFunction& binary_op,
    const bool& inclusive = true )
{
#ifdef BOLT_PROFILER_ENABLED
aProfiler.nextStep();
aProfiler.setStepName("Acquire Kernel");
aProfiler.set(AsyncProfiler::device, control::SerialCpu);
#endif
    cl_int l_Error = CL_SUCCESS;
    cl_uint doExclusiveScan = inclusive ? 0 : 1;
    const size_t numComputeUnits = ctrl.getDevice( ).getInfo< CL_DEVICE_MAX_COMPUTE_UNITS >( );
    const size_t numWorkGroupsPerComputeUnit = ctrl.getWGPerComputeUnit( );
    const size_t workGroupSize = HSAWAVES*WAVESIZE;

    /**********************************************************************************
     * Type Names - used in KernelTemplateSpecializer
     *********************************************************************************/
    typedef std::iterator_traits< DVInputIterator >::value_type iType;
    typedef std::iterator_traits< DVOutputIterator >::value_type oType;

    std::vector<std::string> typeNames( scan_end );
    typeNames[scan_iValueType] = TypeName< iType >::get( );
    typeNames[scan_iIterType] = TypeName< DVInputIterator >::get( );
    typeNames[scan_oValueType] = TypeName< oType >::get( );
    typeNames[scan_oIterType] = TypeName< DVOutputIterator >::get( );
    typeNames[scan_initType] = TypeName< T >::get( );
    typeNames[scan_BinaryFunction] = TypeName< BinaryFunction >::get();

    /**********************************************************************************
     * Type Definitions - directrly concatenated into kernel string
     *********************************************************************************/
    std::vector<std::string> typeDefinitions;
    PUSH_BACK_UNIQUE( typeDefinitions, ClCode< iType >::get() )
    PUSH_BACK_UNIQUE( typeDefinitions, ClCode< DVInputIterator >::get() )
    PUSH_BACK_UNIQUE( typeDefinitions, ClCode< oType >::get() )
    PUSH_BACK_UNIQUE( typeDefinitions, ClCode< DVOutputIterator >::get() )
    PUSH_BACK_UNIQUE( typeDefinitions, ClCode< T >::get() )
    PUSH_BACK_UNIQUE( typeDefinitions, ClCode< BinaryFunction  >::get() )

    /**********************************************************************************
     * Compile Options
     *********************************************************************************/
    bool cpuDevice = ctrl.getDevice().getInfo<CL_DEVICE_TYPE>() == CL_DEVICE_TYPE_CPU;
    //std::cout << "Device is CPU: " << (cpuDevice?"TRUE":"FALSE") << std::endl;
    const size_t kernel0_WgSize = (cpuDevice) ? 1 : WAVESIZE*KERNEL02WAVES;
    const size_t kernel1_WgSize = (cpuDevice) ? 1 : WAVESIZE*KERNEL1WAVES;
    const size_t kernel2_WgSize = (cpuDevice) ? 1 : WAVESIZE*KERNEL02WAVES;
    std::string compileOptions;
    std::ostringstream oss;
    oss << " -DKERNEL0WORKGROUPSIZE=" << kernel0_WgSize;
    oss << " -DKERNEL1WORKGROUPSIZE=" << kernel1_WgSize;
    oss << " -DKERNEL2WORKGROUPSIZE=" << kernel2_WgSize;

    oss << " -DUSE_AMD_HSA=" << USE_AMD_HSA;
    compileOptions = oss.str();

    /**********************************************************************************
     * Request Compiled Kernels
     *********************************************************************************/
    Scan_KernelTemplateSpecializer ts_kts;
    std::vector< ::cl::Kernel > kernels = bolt::cl::getKernels(
        ctrl,
        typeNames,
        &ts_kts,
        typeDefinitions,
        scan_kernels,
        compileOptions);
    // kernels returned in same order as added in KernelTemplaceSpecializer constructor

#ifdef BOLT_PROFILER_ENABLED
aProfiler.nextStep();
aProfiler.setStepName("Acquire Intermediate Buffers");
aProfiler.set(AsyncProfiler::device, control::SerialCpu);
#endif

    /**********************************************************************************
     * Round Up Number of Elements
     *********************************************************************************/
    //  Ceiling function to bump the size of input to the next whole wavefront size
    cl_uint numElements = static_cast< cl_uint >( std::distance( first, last ) );

    size_t numElementsRUP = numElements;
    size_t modWgSize = (numElementsRUP & ((kernel0_WgSize*2)-1));
                if( modWgSize )
                {
        numElementsRUP &= ~modWgSize;
        numElementsRUP += (kernel0_WgSize*2);
    }

    cl_uint numWorkGroupsK0 = static_cast< cl_uint >( numElementsRUP / (kernel0_WgSize*2) );//2 element per work item


    // Create buffer wrappers so we can access the host functors, for read or writing in the kernel
    ALIGNED( 256 ) BinaryFunction aligned_binary( binary_op );
    control::buffPointer userFunctor = ctrl.acquireBuffer( sizeof( aligned_binary ),
        CL_MEM_USE_HOST_PTR|CL_MEM_READ_ONLY, &aligned_binary );
    cl_uint ldsSize;



#if USE_AMD_HSA
    /**********************************************************************************
     *
     *  HSA Implementation
     *
     *********************************************************************************/
#ifdef BOLT_PROFILER_ENABLED
aProfiler.nextStep();
aProfiler.setStepName("Setup HSA Kernel");
aProfiler.set(AsyncProfiler::device, control::SerialCpu);
#endif

    ::cl::Event kernel0Event;
    size_t numWorkGroups = numComputeUnits * numWorkGroupsPerComputeUnit;
    if (numWorkGroupsK0 < numWorkGroups)
        numWorkGroups = numWorkGroupsK0; // nWG is lesser of elements vs compute units
    ldsSize = static_cast< cl_uint >( ( kernel0_WgSize ) * sizeof( oType ) );

    // allocate and initialize gpu -> cpu array
    control::buffPointer dev2hostD = ctrl.acquireBuffer( numWorkGroups*sizeof( int ),
        CL_MEM_ALLOC_HOST_PTR /*| CL_MEM_READ_WRITE*/ );
    ctrl.getCommandQueue().enqueueFillBuffer( *dev2hostD, HSA_STAT_INIT, 0, numWorkGroups*sizeof( int ) );
    int *dev2hostH = (int *) ctrl.getCommandQueue().enqueueMapBuffer( *dev2hostD, CL_TRUE, CL_MAP_READ, 0,
        numWorkGroups*sizeof( int ), NULL, NULL, &l_Error);
    V_OPENCL( l_Error, "Error: Mapping Device->Host Buffer." );

    // allocate and initialize cpu -> gpu array
    control::buffPointer host2devD = ctrl.acquireBuffer( numWorkGroups*sizeof( int ),
        CL_MEM_USE_PERSISTENT_MEM_AMD | CL_MEM_READ_ONLY /*CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_ONLY*/ );
    ctrl.getCommandQueue().enqueueFillBuffer( *host2devD, HSA_STAT_INIT, 0, numWorkGroups*sizeof( int ) );
    int *host2devH = (int *) ctrl.getCommandQueue().enqueueMapBuffer( *host2devD, CL_TRUE, CL_MAP_WRITE, 0,
        numWorkGroups*sizeof( int ), NULL, NULL, &l_Error);
    V_OPENCL( l_Error, "Error: Mapping Host->Device Buffer." );



    int *intermediateScanStatusHost = new int[ numWorkGroups ];
    memset( intermediateScanStatusHost, HSA_STAT_INIT, numWorkGroups*sizeof( int ) );
    oType *intermediateScanArrayHost = new oType[ numWorkGroups ];
    // TODO remove me b/c wrong and superfluous
    memset( intermediateScanArrayHost, init_T, numWorkGroups*sizeof( oType ) );
    control::buffPointer intermediateScanArray  = ctrl.acquireBuffer( numWorkGroups*sizeof( oType ),
        CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, intermediateScanArrayHost );
    control::buffPointer intermediateScanStatus = ctrl.acquireBuffer( numWorkGroups*sizeof( int ),
        CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, intermediateScanStatusHost );
    // how many iterations
    cl_uint numIterations = static_cast< cl_uint >( numElementsRUP / (numWorkGroups*workGroupSize) );
    if (numWorkGroups*workGroupSize*numIterations < numElementsRUP) numIterations++;

    for (size_t i = 0; i < numWorkGroups; i++ )
    {
        std::cout << "preScanStat[" << i << "]="
            << intermediateScanArrayHost[i]
        << " ( " << dev2hostH[i] << ", " << host2devH[i] << " )"<< std::endl;
    }

    /**********************************************************************************
     * Set Kernel Arguments
     *********************************************************************************/
    V_OPENCL( kernels[ 0 ].setArg( 0, result->getBuffer( ) ),   "Error: Output Buffer" );
    V_OPENCL( kernels[ 0 ].setArg( 1, first->getBuffer( ) ),    "Error: Input Buffer" );
    V_OPENCL( kernels[ 0 ].setArg( 2, init_T ),                 "Error: Initial Value" );
    V_OPENCL( kernels[ 0 ].setArg( 3, numElements ),            "Error: Number of Elements" );
    V_OPENCL( kernels[ 0 ].setArg( 4, numIterations ),          "Error: Number of Iterations" );
    V_OPENCL( kernels[ 0 ].setArg( 5, ldsSize, NULL ),          "Error: Local Memory" );
    V_OPENCL( kernels[ 0 ].setArg( 6, *userFunctor ),           "Error: Binary Function" );
    V_OPENCL( kernels[ 0 ].setArg( 7, *intermediateScanArray ), "Error: Intermediate Scan Array" );
    V_OPENCL( kernels[ 0 ].setArg( 8, *dev2hostD ),             "Error: Intermediate Scan Status" );
    V_OPENCL( kernels[ 0 ].setArg( 9, *host2devD ),             "Error: Intermediate Scan Status" );
    V_OPENCL( kernels[ 0 ].setArg( 10, doExclusiveScan ),       "Error: Do Exclusive Scan" );

#ifdef BOLT_PROFILER_ENABLED
aProfiler.nextStep();
aProfiler.setStepName("HSA Kernel");
aProfiler.set(AsyncProfiler::device, ctrl.forceRunMode());
aProfiler.set(AsyncProfiler::flops, 2*numElements);
aProfiler.set(AsyncProfiler::memory,
    1*numElements*sizeof(iType) + // read input
    3*numElements*sizeof(oType) + // write,read,write output
    1*numWorkGroups*sizeof(binary_op) + // in case the functor has state
    2*numWorkGroups*sizeof(oType)+ // write,read intermediate array
    2*numWorkGroups*sizeof(int)); // write,read intermediate array status (perhaps multiple times)
std::string strDeviceName = ctrl.getDevice().getInfo< CL_DEVICE_NAME >( &l_Error );
bolt::cl::V_OPENCL( l_Error, "Device::getInfo< CL_DEVICE_NAME > failed" );
aProfiler.setArchitecture(strDeviceName);
#endif
    /**********************************************************************************
     * Launch Kernel
     *********************************************************************************/
    l_Error = ctrl.getCommandQueue( ).enqueueNDRangeKernel(
        kernels[ 0 ],
        ::cl::NullRange,
        ::cl::NDRange( numElementsRUP ),
        ::cl::NDRange( workGroupSize ),
        NULL,
        &kernel0Event);
    ctrl.getCommandQueue().flush(); // needed

    bool printAgain = true;
    while (printAgain)
    {
        printAgain = false;
        bool writtenToDevice = false;
        for (size_t i = 0; i < numWorkGroups; i++ )
        {
            int stat = dev2hostH[i];
            std::cout << "interScan[" << i << "]="
                << intermediateScanArrayHost[i]
                << " ( " << stat << ", " << host2devH[i] << " )" << std::endl;
            switch( stat )
            {
            case HSA_STAT_INIT: // device hasn't reported P1 completion
                printAgain = true;
                break;
            case HSA_STAT_DEVP1COMPLETE: // device has reported P1 completion
                // pretend to do P2 here
                std::cout << "P1[ " << i << " ] completed" << std::endl;
                host2devH[i] = HSA_STAT_CPUP2COMPLETE; // report P2 completion to device
                printAgain = true;
                writtenToDevice = true;
                break;
            case HSA_STAT_CPUP2COMPLETE: // n/a
                // device shouldn't be reporting that host is reporting P2 completion
                std::cout << "ERROR: dev2host[" << i << "] reporting P2 completion" << std::endl;
                printAgain = true;
                break;
            case HSA_STAT_DEVP3COMPLETE: // device has reported p3 (full) completion
                // done, don't print Again
                break;
            default:
                std::cout << "ERROR: dev2host[" << i << "] reporting unrecognized" << dev2hostH[i] << std::endl;
                printAgain = true;
                break;
            } // switch
        } // for workGroups
        if (writtenToDevice)
        {
            std::cout << "\ttmp readback" << std::endl;

            for (size_t i = 0; i < numWorkGroups; i++)
            {
                tmp += host2devH[i];
            }
        }
        std::cout << std::endl;
    } // while printAgain

    V_OPENCL( l_Error, "enqueueNDRangeKernel() failed for HSA Kernel." );
    l_Error = kernel0Event.wait( );
    V_OPENCL( l_Error, "HSA Kernel failed to wait" );


    for (size_t i = 0; i < numWorkGroups; i++ )
    {
        std::cout << "inter2can[" << i << "]=" << intermediateScanArrayHost[i] << " ( " << dev2hostH[i] << " )";
        std::cout << std::endl;
                }

#ifdef BOLT_PROFILER_ENABLED
aProfiler.stopTrial();
#endif

#else
    /**********************************************************************************
     *
     *  Discrete GPU implementation
     *
     *********************************************************************************/
    // for profiling
    ::cl::Event kernel0Event, kernel1Event, kernel2Event, kernelAEvent;

                //  Ceiling function to bump the size of the sum array to the next whole wavefront size
    device_vector< iType >::size_type sizeScanBuff = numWorkGroupsK0;
    modWgSize = (sizeScanBuff & ((kernel0_WgSize*2)-1));
                if( modWgSize )
                {
                    sizeScanBuff &= ~modWgSize;
                    sizeScanBuff += (kernel0_WgSize*2);
                }

    control::buffPointer preSumArray = ctrl.acquireBuffer( (sizeScanBuff)*sizeof( iType ) );
  control::buffPointer preSumArray1 = ctrl.acquireBuffer( (sizeScanBuff)*sizeof( iType ) );
    control::buffPointer postSumArray = ctrl.acquireBuffer( (sizeScanBuff)*sizeof( iType ) );
    //::cl::Buffer userFunctor( ctrl.context( ), CL_MEM_USE_HOST_PTR, sizeof( binary_op ), &binary_op );
    //::cl::Buffer preSumArray( ctrl.context( ), CL_MEM_READ_WRITE, sizeScanBuff*sizeof(iType) );
    //::cl::Buffer postSumArray( ctrl.context( ), CL_MEM_READ_WRITE, sizeScanBuff*sizeof(iType) );


                /**********************************************************************************
                 *  Kernel 0
                 *********************************************************************************/
#ifdef BOLT_PROFILER_ENABLED
size_t k0e_stepNum, k0s_stepNum, k0_stepNum, k1s_stepNum, k1_stepNum, k2s_stepNum, k2_stepNum, ret_stepNum;
aProfiler.nextStep();
aProfiler.setStepName("Setup Kernel 0");
aProfiler.set(AsyncProfiler::device, control::SerialCpu);
#endif

    ldsSize  = static_cast< cl_uint >( ( kernel0_WgSize *2 ) * sizeof( iType ) );
    V_OPENCL( kernels[ 0 ].setArg( 0, first.getContainer().getBuffer() ),    "Error setting argument for kernels[ 0 ]" ); // Input buffer
    V_OPENCL( kernels[ 0 ].setArg( 1, first.gpuPayloadSize( ), &first.gpuPayload( ) ), "Error setting a kernel argument" );
    V_OPENCL( kernels[ 0 ].setArg( 2, init_T ),                 "Error setting argument for kernels[ 0 ]" ); // Initial value used for exclusive scan
    V_OPENCL( kernels[ 0 ].setArg( 3, numElements ),            "Error setting argument for kernels[ 0 ]" ); // Size of scratch buffer
    V_OPENCL( kernels[ 0 ].setArg( 4, ldsSize, NULL ),          "Error setting argument for kernels[ 0 ]" ); // Scratch buffer
    V_OPENCL( kernels[ 0 ].setArg( 5, *userFunctor ),           "Error setting argument for kernels[ 0 ]" ); // User provided functor class
    V_OPENCL( kernels[ 0 ].setArg( 6, *preSumArray ),           "Error setting argument for kernels[ 0 ]" ); // Output per block sum buffer
    V_OPENCL( kernels[ 0 ].setArg( 7, *preSumArray1 ),           "Error setting argument for kernels[ 0 ]" ); // Output per block
    V_OPENCL( kernels[ 0 ].setArg( 8, doExclusiveScan ),        "Error setting argument for scanKernels[ 0 ]" ); // Exclusive scan?

#ifdef BOLT_PROFILER_ENABLED
aProfiler.nextStep();
aProfiler.setStepName("Enqueue Kernel 0");
k0e_stepNum = aProfiler.getStepNum();
aProfiler.set(AsyncProfiler::device, ctrl.getForceRunMode());
aProfiler.nextStep();
aProfiler.setStepName("Submit Kernel 0");
k0s_stepNum = aProfiler.getStepNum();
aProfiler.set(AsyncProfiler::device, ctrl.getForceRunMode());
aProfiler.nextStep();
aProfiler.setStepName("Kernel 0");
k0_stepNum = aProfiler.getStepNum();
aProfiler.set(AsyncProfiler::device, ctrl.getForceRunMode());
aProfiler.set(AsyncProfiler::flops, 2*numElements);
aProfiler.set(AsyncProfiler::memory, 2*numElements*sizeof(iType) + 1*sizeScanBuff*sizeof(oType));
#endif

    l_Error = ctrl.getCommandQueue( ).enqueueNDRangeKernel(
        kernels[ 0 ],
                    ::cl::NullRange,
        ::cl::NDRange( numElementsRUP/2 ),
                    ::cl::NDRange( kernel0_WgSize ),
                    NULL,
                    &kernel0Event);
                V_OPENCL( l_Error, "enqueueNDRangeKernel() failed for perBlockInclusiveScan kernel" );

                /**********************************************************************************
                 *  Kernel 1
                 *********************************************************************************/
        ldsSize  = static_cast< cl_uint >( ( kernel0_WgSize ) * sizeof( iType ) );
                cl_uint workPerThread = static_cast< cl_uint >( (sizeScanBuff) / kernel1_WgSize  );
        workPerThread = workPerThread ? workPerThread : 1;

    V_OPENCL( kernels[ 1 ].setArg( 0, *postSumArray ),  "Error setting 0th argument for kernels[ 1 ]" );          // Output buffer
    V_OPENCL( kernels[ 1 ].setArg( 1, *preSumArray ),   "Error setting 1st argument for kernels[ 1 ]" );            // Input buffer
    V_OPENCL( kernels[ 1 ].setArg( 2, init_T ),         "Error setting     argument for kernels[ 1 ]" );   // Initial value used for exclusive scan
    V_OPENCL( kernels[ 1 ].setArg( 3, numWorkGroupsK0 ),"Error setting 2nd argument for kernels[ 1 ]" );            // Size of scratch buffer
    V_OPENCL( kernels[ 1 ].setArg( 4, ldsSize, NULL ),  "Error setting 3rd argument for kernels[ 1 ]" );  // Scratch buffer
    V_OPENCL( kernels[ 1 ].setArg( 5, workPerThread ),  "Error setting 4th argument for kernels[ 1 ]" );           // User provided functor class
    V_OPENCL( kernels[ 1 ].setArg( 6, *userFunctor ),   "Error setting 5th argument for kernels[ 1 ]" );           // User provided functor class

#ifdef BOLT_PROFILER_ENABLED
aProfiler.nextStep();
aProfiler.setStepName("Submit Kernel 1");
k1s_stepNum = aProfiler.getStepNum();
aProfiler.set(AsyncProfiler::device, ctrl.getForceRunMode());
aProfiler.nextStep();
k1_stepNum = aProfiler.getStepNum();
aProfiler.setStepName("Kernel 1");
aProfiler.set(AsyncProfiler::device, ctrl.getForceRunMode());
aProfiler.set(AsyncProfiler::flops, 2*sizeScanBuff);
aProfiler.set(AsyncProfiler::memory, 4*sizeScanBuff*sizeof(oType));
#endif

    l_Error = ctrl.getCommandQueue( ).enqueueNDRangeKernel(
        kernels[ 1 ],
                    ::cl::NullRange,
                    ::cl::NDRange( kernel1_WgSize ),
                    ::cl::NDRange( kernel1_WgSize ),
                    NULL,
                    &kernel1Event);
                V_OPENCL( l_Error, "enqueueNDRangeKernel() failed for perBlockInclusiveScan kernel" );

    /**********************************************************************************
     *  Kernel 2
     *********************************************************************************/

    V_OPENCL( kernels[ 2 ].setArg( 0, result.getContainer().getBuffer()), "Error setting 0th argument for scanKernels[ 2 ]" );          // Output buffer
    V_OPENCL( kernels[ 2 ].setArg( 1, result.gpuPayloadSize( ), &result.gpuPayload( ) ), "Error setting a kernel argument" );
    V_OPENCL( kernels[ 2 ].setArg( 2, first.getContainer().getBuffer() ),    "Error setting argument for kernels[ 0 ]" ); // Input buffer
    V_OPENCL( kernels[ 2 ].setArg( 3, first.gpuPayloadSize( ), &first.gpuPayload( ) ), "Error setting a kernel argument" );
    V_OPENCL( kernels[ 2 ].setArg( 4, *postSumArray ), "Error setting 1st argument for scanKernels[ 2 ]" );            // Input buffer
    V_OPENCL( kernels[ 2 ].setArg( 5, *preSumArray1 ),           "Error setting argument for kernels[ 0 ]" ); // Output per block
    V_OPENCL( kernels[ 2 ].setArg( 6, ldsSize, NULL ),          "Error setting argument for kernels[ 0 ]" ); // Scratch buffer
    V_OPENCL( kernels[ 2 ].setArg( 7, numElements ), "Error setting 2nd argument for scanKernels[ 2 ]" );   // Size of scratch buffer
    V_OPENCL( kernels[ 2 ].setArg( 8, *userFunctor ), "Error setting 3rd argument for scanKernels[ 2 ]" );           // User provided functor class
    V_OPENCL( kernels[ 2 ].setArg( 9, doExclusiveScan ),        "Error setting argument for scanKernels[ 0 ]" ); // Exclusive scan?
    V_OPENCL( kernels[ 2 ].setArg( 10, init_T ),                 "Error setting argument for kernels[ 0 ]" ); // Initial value used for exclusive scan

#ifdef BOLT_PROFILER_ENABLED
aProfiler.nextStep();
aProfiler.setStepName("Submit Kernel 2");
k2s_stepNum = aProfiler.getStepNum();
aProfiler.set(AsyncProfiler::device, ctrl.getForceRunMode());
aProfiler.nextStep();
k2_stepNum = aProfiler.getStepNum();
aProfiler.setStepName("Kernel 2");
aProfiler.set(AsyncProfiler::device, ctrl.getForceRunMode());
aProfiler.set(AsyncProfiler::flops, numElements);
aProfiler.set(AsyncProfiler::memory, 2*numElements*sizeof(oType) + 1*sizeScanBuff*sizeof(oType));
#endif
                try
                {
                    l_Error = ctrl.getCommandQueue( ).enqueueNDRangeKernel(
                    kernels[ 2 ],
                    ::cl::NullRange,
                    ::cl::NDRange( numElementsRUP ), // remove /2 to return to 1 element per thread
                    ::cl::NDRange( kernel2_WgSize ),
                    NULL,
                    &kernel2Event );
                    V_OPENCL( l_Error, "enqueueNDRangeKernel() failed for perBlockInclusiveScan kernel" );
                }
                catch ( ::cl::Error& e )
                {
                    std::cout << ( "Kernel 3 enqueueNDRangeKernel error condition reported:" ) << std::endl;
                    std::cout << e.what() << std::endl;
                    return;
                }
                l_Error = kernel2Event.wait( );
                V_OPENCL( l_Error, "perBlockInclusiveScan failed to wait" );

#ifdef BOLT_PROFILER_ENABLED
aProfiler.nextStep();
aProfiler.setStepName("Returning Control To Device");
ret_stepNum = aProfiler.getStepNum();
aProfiler.set(AsyncProfiler::device, ctrl.getForceRunMode());
aProfiler.nextStep();
aProfiler.setStepName("Querying Kernel Times");
aProfiler.set(AsyncProfiler::device, control::SerialCpu);

aProfiler.setDataSize(numElements*sizeof(iType));
std::string strDeviceName = ctrl.getDevice().getInfo< CL_DEVICE_NAME >( &l_Error );
bolt::cl::V_OPENCL( l_Error, "Device::getInfo< CL_DEVICE_NAME > failed" );
aProfiler.setArchitecture(strDeviceName);

                try
                {
        cl_ulong k0enq, k0sub, k0start, k0stop;
        cl_ulong k1sub, k1start, k1stop;
        cl_ulong k2sub, k2start, k2stop;
        //cl_ulong ret;

        //cl_ulong k0_start, k0_stop, k1_stop, k2_stop;
        //cl_ulong k1_start, k2_start;

        V_OPENCL( kernel0Event.getProfilingInfo<cl_ulong>(CL_PROFILING_COMMAND_QUEUED, &k0enq),   "getProfInfo" );
        V_OPENCL( kernel0Event.getProfilingInfo<cl_ulong>(CL_PROFILING_COMMAND_SUBMIT, &k0sub),   "getProfInfo" );
        V_OPENCL( kernel0Event.getProfilingInfo<cl_ulong>(CL_PROFILING_COMMAND_START,  &k0start), "getProfInfo" );
        V_OPENCL( kernel0Event.getProfilingInfo<cl_ulong>(CL_PROFILING_COMMAND_END,    &k0stop),  "getProfInfo" );

        V_OPENCL( kernel1Event.getProfilingInfo<cl_ulong>(CL_PROFILING_COMMAND_SUBMIT, &k1sub),   "getProfInfo" );
        V_OPENCL( kernel1Event.getProfilingInfo<cl_ulong>(CL_PROFILING_COMMAND_START,  &k1start), "getProfInfo" );
        V_OPENCL( kernel1Event.getProfilingInfo<cl_ulong>(CL_PROFILING_COMMAND_END,    &k1stop),  "getProfInfo" );

        V_OPENCL( kernel2Event.getProfilingInfo<cl_ulong>(CL_PROFILING_COMMAND_SUBMIT, &k2sub),   "getProfInfo" );
        V_OPENCL( kernel2Event.getProfilingInfo<cl_ulong>(CL_PROFILING_COMMAND_START,  &k2start), "getProfInfo" );
        V_OPENCL( kernel2Event.getProfilingInfo<cl_ulong>(CL_PROFILING_COMMAND_END,    &k2stop),  "getProfInfo" );
#if 0
        printf("BEFORE\n");
        printf("K0 Enque %10u ns CPU\n", aProfiler.get(k0e_stepNum, AsyncProfiler::startTime));
        printf("K0 Enque %10u ns\n", k0enq);
        printf("K0 Submt %10u ns\n", k0sub);
        printf("K0 Start %10u ns\n", k0start);
        printf("K0 Stop  %10u ns\n", k0stop);
        printf("K1 Submt %10u ns\n", k1sub);
        printf("K1 Start %10u ns\n", k1start);
        printf("K1 Stop  %10u ns\n", k1stop);
        printf("K2 Submt %10u ns\n", k2sub);
        printf("K2 Start %10u ns\n", k2start);
        printf("K2 Stop  %10u ns\n", k2stop);
        printf("Return   %10u ns\n", aProfiler.get(ret_stepNum, AsyncProfiler::startTime) );
        printf("Returned %10u ns\n", aProfiler.get(ret_stepNum, AsyncProfiler::stopTime) );
#endif
        // determine shift between cpu and gpu clock according to kernel 0 enqueue time
        size_t k0_enq_cpu = aProfiler.get(k0e_stepNum, AsyncProfiler::startTime);
        size_t k0_enq_gpu = static_cast<size_t>( k0enq );
        long long shift = k0enq - k0_enq_cpu; // must be signed because can be '-'
        //printf("\nSHIFT % 10u ns\n", shift );

        // apply shift to all steps
        aProfiler.set(k0e_stepNum, AsyncProfiler::startTime, static_cast<size_t>(k0enq  -shift) ); // same
        aProfiler.set(k0e_stepNum, AsyncProfiler::stopTime,  static_cast<size_t>(k0sub  -shift) );
        aProfiler.set(k0s_stepNum, AsyncProfiler::startTime, static_cast<size_t>(k0sub  -shift) );
        aProfiler.set(k0s_stepNum, AsyncProfiler::stopTime,  static_cast<size_t>(k0start-shift) );
        aProfiler.set(k0_stepNum,  AsyncProfiler::startTime, static_cast<size_t>(k0start-shift) );
        aProfiler.set(k0_stepNum,  AsyncProfiler::stopTime,  static_cast<size_t>(k0stop -shift) );

        aProfiler.set(k1s_stepNum, AsyncProfiler::startTime, static_cast<size_t>(k0sub  -shift) );
        aProfiler.set(k1s_stepNum, AsyncProfiler::stopTime,  static_cast<size_t>(k1start-shift) );
        aProfiler.set(k1_stepNum,  AsyncProfiler::startTime, static_cast<size_t>(k1start-shift) );
        aProfiler.set(k1_stepNum,  AsyncProfiler::stopTime,  static_cast<size_t>(k1stop -shift) );

        aProfiler.set(k2s_stepNum, AsyncProfiler::startTime, static_cast<size_t>(k1stop -shift) );
        aProfiler.set(k2s_stepNum, AsyncProfiler::stopTime,  static_cast<size_t>(k2start-shift) );
        aProfiler.set(k2_stepNum,  AsyncProfiler::startTime, static_cast<size_t>(k2start-shift) );
        aProfiler.set(k2_stepNum,  AsyncProfiler::stopTime,  static_cast<size_t>(k2stop -shift) );

        aProfiler.set(ret_stepNum, AsyncProfiler::startTime, static_cast<size_t>(k2stop -shift) );
        // aProfiler.set(ret_stepNum, AsyncProfiler::stopTime,  static_cast<size_t>(k2_stop-shift) ); // same
#if 0
        printf("\nAFTER\n");
        printf("K0 Enque %10u ns CPU\n", aProfiler.get(k0e_stepNum, AsyncProfiler::startTime) );
        printf("K0 Enque %10u ns GPU\n", aProfiler.get(k0e_stepNum, AsyncProfiler::startTime) );
        printf("K0 Submt %10u ns GPU\n", aProfiler.get(k0s_stepNum, AsyncProfiler::startTime) );
        printf("K0 Start %10u ns GPU\n", aProfiler.get(k0_stepNum,  AsyncProfiler::startTime) );
        printf("K0 Stop  %10u ns GPU\n", aProfiler.get(k0_stepNum,  AsyncProfiler::stopTime ) );
        printf("K1 Submt %10u ns GPU\n", aProfiler.get(k1s_stepNum, AsyncProfiler::startTime) );
        printf("K1 Start %10u ns GPU\n", aProfiler.get(k1_stepNum,  AsyncProfiler::startTime) );
        printf("K1 Stop  %10u ns GPU\n", aProfiler.get(k1_stepNum,  AsyncProfiler::stopTime ) );
        printf("K2 Submt %10u ns GPU\n", aProfiler.get(k2s_stepNum, AsyncProfiler::startTime) );
        printf("K2 Start %10u ns GPU\n", aProfiler.get(k2_stepNum,  AsyncProfiler::startTime) );
        printf("K2 Stop  %10u ns GPU\n", aProfiler.get(k2_stepNum,  AsyncProfiler::stopTime ) );
        printf("Return   %10u ns GPU\n", aProfiler.get(ret_stepNum, AsyncProfiler::startTime) );
        printf("Returned %10u ns CPU\n", aProfiler.get(ret_stepNum, AsyncProfiler::stopTime ) );
#endif
                }
                catch( ::cl::Error& e )
                {
                    std::cout << ( "Scan Benchmark error condition reported:" ) << std::endl << e.what() << std::endl;
                    return;
                }


#endif // ENABLE_PROFILING

#endif

}   //end of inclusive_scan_enqueue( )

}   //namespace detail
}   //namespace cl
}//namespace bolt

#endif // BOLT_CL_SCAN_INL
