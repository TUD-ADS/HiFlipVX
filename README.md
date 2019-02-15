## HiFlipVX

Open Source ***Hi***gh-Level Synthesis ***F***PGA ***L***ibrary for ***I***mage ***P***rocessing

- Image procesing library for FPGAs
- Contains 28 image processing functions
- Funktions based on the OpenVX standard
- Highly optimized, parametrizable
- Most functions support auto-vectorization (parallelization)
- Most functions support additional data-types
- Functions implemented for streaming applications
- Does not require additional libraries
- Optimized for Xilinx SDSoC / Vivado HLS


## Provided Functions

Image Pixelwise Functions
- Bitwise AND
- Bitwise EXCLUSIVE OR
- Bitwise INCLUSIVE OR
- Bitwise NOT
- Arithmetic Addition
- Arithmetic Subtraction
- Min
- Max
- Data Object Copy
- Absolute Difference
- Pixel-wise Multiplication
- Magnitude

Image Filter Functions
- Box Filter
- Custom Convolution
- Dilate Image
- Erode Image
- Gaussian Filter
- Median Filter
- Scharr 3x3
- Sobel

Image Conversion and Analysis Functions
- Color Convert
- Channel Combine
- Channel Extract
- Convert Bit Depth
- Histogram
- Integral Image
- Scale Image
- TableLookup

Windowed operations support different border handling:
- Replicated: Pixels beyond borders are replicated
- Constant: Pixels beyond borders are constant zero
- Undefined: Pixels beyond borders do not contain valid data

Some operations support conversion policies against overflow:
- Wrap: Results are the least significant bits of the output operand.
- Saturate: Results are saturated to the bit depth of the output operand.

Some operations support rounding policies:
- Round to Zero: This truncates the least significant values that are lost in operations.
- Round to Nearest Even: This rounds to nearest even output value.


## Library Files:

The image processing library contains the following files:
- **main.cpp:** Contains examples of how to use the implemented functions. 
- **img_filter_base.h:**  Contains all filter functions. Call functions from here.
- **img_pixelop_base.h:** Contains all pixel-wise operation functions. Call functions from here.
- **img_other_base.h:**   Contains all other operation functions. Call functions from here.
- **img_helper.h:**       Contains The image data tpye and functions to create and destroy images
- **vx_types.h:**         Contains all OpenVX data types needed (imported from OpenVX)
- img_filter_core.h:  Contains all sub-functions that are needed for the filter functions.
- img_filter_test.h:  Contains all test function, to test the functionality of the filter functions.
- img_pixelop_core.h: Contains all sub-functions that are needed for the pixel-wise operations.
- img_pixelop_test.h: Contains all test function, to test the functionality of the pixel-wise operation functions.
- img_other_core.h:   Contains all sub-functions that are needed for the other operations.
- img_other_test.h:   Contains all test function, to test the functionality of the other functions.


## Usage

The library files explained in the previous sub-section can be used as follows:
- Add the *filter_base.h*, *pixelop_base.h* and *img_other_core.h* to call functions.
- The *img_helper.h* contains the needed image data type and functions to create and destroy images
- *vx_types.h* needed OpenVX data types
- The "main.cpp" contains examples how to call the implemented functions.
- There is an example in the main for hardware efficient streaming in 1 accelerator.


## Outlook

Further functionality will be added to the library:
- Will add more functions in future (Minimum 8 Functions in next Version)
- Add functions from the OpenVX neural network extension
- Create an upper layer for application distribution and HW/SW Codesign using the OpenVX graphs


## Citations
If you use this code as part of your work, please cite the following paper:
- L. Kalms and A. Podlubne and D.Göhringer, *HiFlipVX: an Open Source High-Level Synthesis FPGA Library for Image Processing,* 15th International Symposium on Applied Reconfigurable Computing. Architectures, Tools, and  Applications (ARC), Apr. 2019


## Contact Info
M.Sc. Lester Kalms
Technische Universität Dresden
lester.kalms@tu-dresden.de

https://scholar.google.com/citations?user=ybrapsUAAAAJ&hl=en&oi=sra

