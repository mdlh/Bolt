############################################################################                                                                                     
#   Copyright 2012 - 2013 Advanced Micro Devices, Inc.                                     
#                                                                                    
#   Licensed under the Apache License, Version 2.0 (the "License");   
#   you may not use this file except in compliance with the License.                 
#   You may obtain a copy of the License at                                          
#                                                                                    
#       http://www.apache.org/licenses/LICENSE-2.0                      
#                                                                                    
#   Unless required by applicable law or agreed to in writing, software              
#   distributed under the License is distributed on an "AS IS" BASIS,              
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.         
#   See the License for the specific language governing permissions and              
#   limitations under the License.                                                   

############################################################################                                                                                     

# List the names of common files to compile across all platforms

set( clBolt.Test.InnerProduct.Source stdafx.cpp InnerProductTest.cpp InnerProductDeviceVector.cpp ${BOLT_CL_TEST_DIR}/common/myocl.cpp) 
set( clBolt.Test.InnerProduct.Headers stdafx.h ${BOLT_CL_TEST_DIR}/common/myocl.h targetver.h ${BOLT_INCLUDE_DIR}/bolt/cl/inner_product.h ${BOLT_INCLUDE_DIR}/bolt/cl/detail/inner_product.inl )

set( clBolt.Test.InnerProduct.Files ${clBolt.Test.InnerProduct.Source} ${clBolt.Test.InnerProduct.Headers} )

add_executable( clBolt.Test.InnerProduct ${clBolt.Test.InnerProduct.Files} )

if(BUILD_TBB)
    target_link_libraries( clBolt.Test.InnerProduct clBolt.Runtime ${OPENCL_LIBRARIES} ${GTEST_LIBRARIES} ${Boost_LIBRARIES}  ${TBB_LIBRARIES} )
else (BUILD_TBB)
    target_link_libraries( clBolt.Test.InnerProduct clBolt.Runtime ${OPENCL_LIBRARIES} ${GTEST_LIBRARIES} ${Boost_LIBRARIES}  )
endif()

set_target_properties( clBolt.Test.InnerProduct PROPERTIES VERSION ${Bolt_VERSION} )
set_target_properties( clBolt.Test.InnerProduct PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/staging" )

set_property( TARGET clBolt.Test.InnerProduct PROPERTY FOLDER "Test/OpenCL")
		
# CPack configuration; include the executable into the package
install( TARGETS clBolt.Test.InnerProduct
	RUNTIME DESTINATION ${BIN_DIR}
	LIBRARY DESTINATION ${LIB_DIR}
	ARCHIVE DESTINATION ${LIB_DIR}/import
	)
