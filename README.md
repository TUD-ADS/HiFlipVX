## HiFlipVX

Open Source **Hi**gh-Level Synthesis **F**PGA **L**ibrary for **I**mage **P**rocessing

- Image procesing library for FPGAs
- Contains 41 image processing functions
- Funktions based on the OpenVX standard
- Highly optimized, parametrizable and streaming capable
- Most functions support auto-vectorization (parallelization)
- Most functions support additional data-types
- Does not require additional libraries
- Optimized for Xilinx SDSoC / SDAccel / Vivado HLS


## Provided Functions

Image Pixelwise Functions
- Absolute Difference
- Arithmetic Addition
- Arithmetic Subtraction
- Bitwise AND
- Bitwise EXCLUSIVE OR
- Bitwise INCLUSIVE OR
- Bitwise NOT
- Data Object Copy
- Magnitude
- Max
- Min
- Phase
- Pixel-wise Multiplication
- Thresholding
- Weighted Average

Image Filter Functions
- Box Filter
- Custom Convolution
- Dilate Image
- Erode Image
- Gaussian Filter
- Hysteresis Filter
- Median Filter
- Non-Maxima Suppression
- Oriented Non-Maxima Suppression
- Scharr 3x3
- Segment Test Detector
- Sobel

Image Conversion Functions
- Channel Combine
- Channel Extract
- Color Convert
- Convert Bit Depth
- Convert Data Width
- Multicast
- Scale Image

Image Analysis Functions
- Control Flow
- Equalize Histogram
- Histogram
- Integral Image
- Mean & Standard Deviation
- Min, Max Location
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
- **main.cpp:**              Contains examples of how to use the implemented functions. 
- **img_filter_base.h:**     Contains all filter functions. Call functions from here.
- **img_pixelop_base.h:**    Contains all pixel-wise operation functions. Call functions from here.
- **img_conversion_base.h:** Contains all conversion functions. Call functions from here.
- **img_analysis_base.h:**   Contains all analysis functions. Call functions from here.
- **vx_helper.h:**           Contains all own data types, enums and macros. Contains own image data type (*vx_image_data*) and functions to create and destroy these images. 
- **vx_types.h:**            Contains all OpenVX data types, enums and macros (imported from OpenVX).
- img_filter_core.h:     Contains all sub-functions that are needed for the filter functions.
- img_filter_test.h:     Contains all test function, to test the functionality of the filter functions.
- img_pixelop_core.h:    Contains all sub-functions that are needed for the pixel-wise operations.
- img_pixelop_test.h:    Contains all test function, to test the functionality of the pixel-wise operation functions.
- img_conversion_core.h: Contains all sub-functions that are needed for the conversion operations.
- img_analysis_core.h:   Contains all sub-functions that are needed for the analysis operations.
- img_reader.h:          Contains files to read/write pgm images for testing,  


## Usage

The library files explained in the previous sub-section can be used as follows:
- Add the *filter_base.h*, *pixelop_base.h*, *img_analysis_base.h:* and *img_conversion_base.h* to call functions.
- The *vx_helper.h* contains the needed image data type and functions to create and destroy images
- *vx_types.h* contains the needed OpenVX data types
- The *main.cpp* contains examples how to call the implemented functions.
- There are example applications in the main for efficient streaming within one accelerator.


## Needed macros (*vx_helper.h*)
- **__XILINX__** -> Set this flag when using the library in the Xilinx tools
- **__LAST__** -> Creates a LAST signal to the *vx_image_data* that indicates the EOF (start of frame) - Set for VDMA & DMA
- **__USER__** -> Creates a USER signal to the *vx_image_data* that indicates the SOF (start of frame) - Set for VDMA


## Outlook

Further functionality will be added to the library:
- Add functions for feature detection
- Add functions for neural networks
- Create an OpenVX framework including the graph-based approach and memory management


## Citations
If you use this code as part of your work, please cite the following paper:
- L. Kalms and A. Podlubne and D.Göhringer, *HiFlipVX: an Open Source High-Level Synthesis FPGA Library for Image Processing,* 15th International Symposium on Applied Reconfigurable Computing. Architectures, Tools, and  Applications (ARC), Apr. 2019


## Contact Info
M.Sc. Lester Kalms,
Technische Universität Dresden,
lester.kalms@tu-dresden.de,

https://scholar.google.com/citations?user=ybrapsUAAAAJ&hl=en&oi=sra

## Acknowledgment
This work has been partially supported by European Union’s Horizon 2020 research and innovation programme as part of the TULIPP project under grant agreement No 688403 and partially by the German Federal Ministry of Education and Research BMBF as part of the PARIS project under grant agreement number 16ES0657.
